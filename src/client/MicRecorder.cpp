#include "MicRecorder.hpp"
#include <expected>
#include <memory>
#include <system_error>
#include <utility>

std::expected<std::unique_ptr<MicRecorder>, std::error_code>
MicRecorder::create(const std::string& input_device) {
    
    auto recorder = std::unique_ptr<MicRecorder>(new MicRecorder{});
    if (!recorder->setDevice(input_device)) {
        return std::unexpected(make_error_code(std::errc::no_such_device));
    }

    return std::move(recorder);
}

bool MicRecorder::onProcessSamples(const std::int16_t* /*samples*/, std::size_t /*sample_count*/) {
    return true;
}
