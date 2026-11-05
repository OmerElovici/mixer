#include "AudioChunk.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

AudioChunkBuffer::AudioChunkBuffer() : chunk_buffer_(kBufferSIze) {
    std::size_t chunk_offset = 0;
    for (std::size_t idx = 0; idx < kListSize; ++idx) {
        assert(chunk_offset < chunk_buffer_.size());
        chunks_indexs_free_.emplace_back(chunk_offset);
        chunk_offset += kAudioChunkSize;
    }
}

void AudioChunkBuffer::push_chunk() {
    assert(current_incomplate_chunk_size_ == kAudioChunkSize);
    assert(in_progress_ == true);

    chunks_indexs_.push_back(current_incomplate_chunk_idx_);
    in_progress_ = false;
}

void AudioChunkBuffer::set_buffering() {
    assert(in_progress_ == false);
    assert(!chunks_indexs_free_.empty());


    if (chunks_indexs_free_.empty()) {
        chunk_buffer_.resize(chunk_buffer_.size() + kAudioChunkSize);
        current_incomplate_chunk_idx_ = chunk_buffer_.size() - kAudioChunkSize;
    } else {
        current_incomplate_chunk_idx_ = chunks_indexs_free_.back();
    }


    current_incomplate_chunk_size_ = 0;
    in_progress_ = true;
}

[[nodiscard]] bool AudioChunkBuffer::buffering() const {
    return in_progress_;
}

void AudioChunkBuffer::add_samples(std::span<const std::int16_t> samples) {
    auto itr = samples.begin();
    while (itr != samples.end()) {
        if (!buffering()) {
            set_buffering();
        }


        assert(current_incomplate_chunk_size_ <= kAudioChunkSize);
        const std::size_t remaining = kAudioChunkSize - current_incomplate_chunk_size_;

        if (remaining == 0) {
            push_chunk();
            continue;
        }

        const std::size_t remaining_samples = static_cast<std::size_t>(samples.end() - itr);
        const std::size_t samples_to_copy = std::min(remaining_samples, remaining);

        assert(current_incomplate_chunk_idx_ < chunk_buffer_.size());
        std::copy_n(itr, samples_to_copy, &chunk_buffer_[current_incomplate_chunk_idx_]); // NOLINT
        current_incomplate_chunk_size_ += samples_to_copy;

        itr += static_cast<std::ptrdiff_t>(samples_to_copy);
    }
}

std::span<const std::int16_t> AudioChunkBuffer::front_chunk() {
    if (chunks_indexs_.empty()) {
        return {};
    }

    const std::size_t front_idx = chunks_indexs_.front();

    assert(front_idx < chunk_buffer_.size());
    return std::span<const std::int16_t>{chunk_buffer_.begin() + front_idx,
                                         kAudioChunkSize}; // NOLINT
}

void AudioChunkBuffer::pop_front_chunk() {
    if (!chunks_indexs_.empty()) {
        const std::size_t idx = chunks_indexs_.front();
        chunks_indexs_.erase(chunks_indexs_.begin());
        chunks_indexs_free_.push_back(idx);
    }
}
