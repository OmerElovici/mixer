#include "MicRecorder.hpp"
#include <spdlog/spdlog.h>
#include <span>
#include "ISamplesProcessor.hpp"

bool MicRecorder::onStart() {
    const bool proc_res = processor_->start();
    if (!proc_res) {
        spdlog::critical("Failed to start processor when initlizing MicRecorder");
        return false;
    }
    return true;
}

bool MicRecorder::onProcessSamples(const std::int16_t* samples,
                                   std::size_t sample_count) { // NOLINT

    spdlog::info("packet size {}", sample_count);

    if (processor_ != nullptr) {
        const std::span<const std::int16_t> samples_view{samples, sample_count};
        const bool pros_res = processor_->process_samples(samples_view);

        if (!pros_res) {
            spdlog::error("samples processor failed during process_samples");
        }
    } else {
        return false;
    }

    return true;
}

void MicRecorder::set_processor(ISamplesProcessor* processor) {
    processor_ = processor;
}
