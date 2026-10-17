#include "Pacer.hpp"
#include <spdlog/spdlog.h>

#include <array>
#include <asio/dispatch.hpp>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>
#include <system_error>
#include <utility>
#include "AudioChunk.hpp"

Pacer::Pacer(asio::any_io_executor executor, std::uint32_t packet_time)
    : executor_(std::move(executor))
    , audio_queue_(executor_, 32)
    , packet_time_(packet_time)
    , timer_(executor_, packet_time_) {
    start();
};

constexpr std::size_t Pacer::calculate_packet_size() {
    return static_cast<std::size_t>(packet_time_.count() * 8000);
}

bool Pacer::push_chunk(std::span<const std::int16_t> chunk) {
    if (chunk.size() == kMaxAudioChunkSize) {
        return audio_queue_.try_send(std::error_code{}, AudioChunk{chunk.begin(), chunk.end()});
    }

    if (chunk.size() < kMaxAudioChunkSize) {
        return audio_queue_.try_send(std::error_code{}, AudioChunk{chunk.begin(), chunk.end()});
    }


    // push chunks in multiply of Audiochunk
    std::span<const std::int16_t> chunk_view = chunk;
    while (chunk_view.size() >= kMaxAudioChunkSize) {
        const std::span<const std::int16_t> chunk_element{chunk_view.begin(), kMaxAudioChunkSize};
        audio_queue_.try_send(std::error_code{}, AudioChunk{chunk_element.begin(), chunk.end()});
        chunk_view = chunk_view.subspan(kMaxAudioChunkSize);
    }


    // push remaining data
    const std::size_t diff = chunk.size() % kAudioChunkSize;
    if (diff > 0) {
        return audio_queue_.try_send(std::error_code{},
                                     AudioChunk{chunk_view.begin(), chunk_view.end()});

        // spdlog::warn("ignoring remaining samples from chunk :  {}",diff);
        // return true;
    }


    return false;
}

bool Pacer::start() {
    if (is_ready_.exchange(true, std::memory_order_relaxed)) {
        // asio::post(executor_, [this] { tick(); });
        tick();
        return true;
    }

    return false;
}

void Pacer::send_chunk(std::span<const std::uint8_t> chunk) {
    if (rtp_sender_) {
        rtp_sender_->async_send_pkt(
            chunk, [](std::size_t bytes_sent, const std::error_code& err, std::uint16_t seq) {
                if (err) {
                    spdlog::error("error occured during sending audio chunk {}", err.message());
                    return;
                }

                spdlog::info("bytes sent {}, sequence : {}", bytes_sent, seq);
            });
    } else {
        spdlog::warn("Sender is not set in pacer");
    }
}

void Pacer::on_audio_chunk_data(std::span<const std::int16_t> chunk_data) {
    const auto chunk = incomplete_audio_chunks_buffer_.front_chunk();


    // Send ready chunks from buffer first and add them to buffer for later sending
    if (!chunk.empty()) {
        send_chunk({});
        incomplete_audio_chunks_buffer_.pop_front_chunk();
        incomplete_audio_chunks_buffer_.add_samples(chunk_data);


        return;

        // send incoming chunk if we arent buffering and its in the right size
    } else if (chunk_data.size() == kAudioChunkSize) {
        const std::span<const std::int16_t> full_chunk{chunk.begin(), kAudioChunkSize};
        send_chunk({});
        return;
    }

    incomplete_audio_chunks_buffer_.add_samples(chunk_data);
    const auto combined_chunk = incomplete_audio_chunks_buffer_.front_chunk();


    // check if we have chunk after we added the samples
    if (!combined_chunk.empty()) {
        send_chunk({});
        incomplete_audio_chunks_buffer_.pop_front_chunk();
    }
}

void Pacer::on_empty_channel() {
    constexpr std::array<std::uint8_t, 160> silence_pkt{0xD5}; // NOLINT
    if (rtp_sender_) {
        rtp_sender_->async_send_pkt(
            silence_pkt, [](std::size_t bytes_sent, const std::error_code& err, std::uint16_t seq) {
                if (err) {
                    spdlog::error("error during send {}", err.message());
                    return;
                }

                spdlog::info("silence sent : bytes sent {}, sequence : {}", bytes_sent, seq);
            });
    } else {
        spdlog::warn("Sender is not set in pacer");
    }
}

std::error_code Pacer::on_tick() {
    std::error_code channel_err{};

    const bool res = audio_queue_.try_receive([channel_err, this](std::error_code err,
                                                                  const AudioChunk& chunk) mutable {
        channel_err = err;

        if (err) {
            spdlog::critical("error occured during reading from audio queue : {}", err.message());
        } else {
            on_audio_chunk_data(chunk);
        }
    });

    if (!res) {
        on_empty_channel();
    }


    return channel_err;
}

void Pacer::tick() {
    timer_.async_wait([this](const std::error_code& err) {
        if (err) {
            spdlog::critical("error occurred in pacer timer {}", err.message());
            // TODO check if we need to handle specific errors
            return;
        }

        const std::error_code channel_err = on_tick();
        if (channel_err) {
            spdlog::critical("error occured during channel read : {}", err.message());
            return;
        }

        // Advance relative to the previous expiry
        timer_.expires_at(timer_.expiry() + packet_time_);
        tick();
    });
}
