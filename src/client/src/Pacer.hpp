#pragma once


#include <RtpCpp.hpp>
#include <asio/any_io_executor.hpp>
#include <asio/experimental/concurrent_channel.hpp>
#include <asio/post.hpp>
#include <asio/steady_timer.hpp>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <system_error>
#include <utility>
#include "ISamplesProcessor.hpp"

#include "AudioChunk.hpp"

class Pacer : public ISamplesProcessor {
public:
    static constexpr std::uint32_t kDefaultPacketTIme = 20;


    explicit Pacer(asio::any_io_executor executor,
                   std::uint32_t packet_time = kDefaultPacketTIme); // NOLINT

    void set_rtp_sender(std::unique_ptr<RtpCpp::RtpSender> sender) {
        // asio::post(executor_, [sender = std::move(sender), this] mutable { rtp_sender_ =
        // std::move(sender); });
        rtp_sender_ = std::move(sender);
    }

    bool process_samples(std::span<const std::int16_t> samples) override {
        return push_chunk(samples);
    }

    bool start() override;

    bool is_ready() override { return is_ready_.load(std::memory_order_relaxed); }

private:
    constexpr std::size_t calculate_packet_size();

    void send_chunk(std::span<const std::uint8_t> chunk);
    void on_audio_chunk_data(std::span<const std::int16_t> chunk_data);
    void on_empty_channel();
    bool push_chunk(std::span<const std::int16_t> chunk);
    void tick();
    std::error_code on_tick();


    AudioChunkBuffer incomplete_audio_chunks_buffer_; // buffer for holding merged audio chunks
                                                        // that are less then 160 samples.
    asio::any_io_executor executor_;
    asio::experimental::concurrent_channel<void(std::error_code, AudioChunk)>
        audio_queue_;
    std::chrono::milliseconds packet_time_;
    asio::steady_timer timer_;
    std::unique_ptr<RtpCpp::RtpSender> rtp_sender_ = nullptr;

    std::atomic<bool> is_ready_ = false;
};
