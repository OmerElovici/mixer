#pragma once


#include <RtpCpp.hpp>
#include <cassert>
#include <cstddef>
#include <span>

inline constexpr std::size_t kAudioChunkSize = 160;
inline constexpr std::size_t kMaxAudioChunkSize = 256;
using AudioChunk = RtpCpp::llvm::SmallVector<std::int16_t, kAudioChunkSize>;

class AudioChunkBuffer {
public:
    AudioChunkBuffer();

    void add_samples(std::span<const std::int16_t> samples);

    std::span<const std::int16_t> front_chunk();
    
    void pop_front_chunk();

    [[nodiscard]] bool buffering() const;

private:
    static constexpr std::size_t kBufferSIze = kAudioChunkSize * 2;
    static constexpr std::size_t kListSize = kBufferSIze / kAudioChunkSize;


    void push_chunk();
    void set_buffering();

    RtpCpp::llvm::SmallVector<std::size_t, kListSize> chunks_indexs_free_;
    RtpCpp::llvm::SmallVector<std::int16_t, kBufferSIze> chunk_buffer_;
    RtpCpp::llvm::SmallVector<std::size_t, kListSize> chunks_indexs_;
    std::size_t current_incomplate_chunk_idx_ = 0;
    std::size_t current_incomplate_chunk_size_ = 0;
    bool in_progress_ = false;
};
