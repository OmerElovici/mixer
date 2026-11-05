#include "Client.hpp"
#include <spdlog/spdlog.h>
#include <RtpCpp.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <cassert>
#include <expected>
#include <string_view>
#include <system_error>
#include <utility>
#include "MicRecorder.hpp"

Client::Client()
    : io_ctx_(1)
    , io_wrk_guard_(asio::make_work_guard(io_ctx_))
    , packet_pacer_(io_ctx_.get_executor()) {
    // packet_pacer_.set_rtp_sender(std::move(rtp_sender));
    mic_input_.set_processor(&packet_pacer_);
};

std::error_code Client::bind() {
    constexpr auto kPayloadType = std::to_underlying(RtpCpp::AudioPt::kPCMA);
    const RtpCpp::RtpSenderConfig cfg{
        .payload_type_ = kPayloadType,
        .default_timestamp_step_ = 160, // NOLINT
    }; // NOLINT

    auto sender = RtpCpp::make_rtp_sender(io_ctx_.get_executor(), cfg);
    assert(sender != nullptr);

    auto bind_result = sender->bind();
    if (!bind_result.has_value()) {
        return bind_result.error();
    }

    constexpr std::string_view kServerAddress = "127.0.0.1";
    constexpr unsigned short kServerPort = 30000;
    auto dst_result = sender->set_default_dst(kServerAddress, kServerPort);
    if (!dst_result.has_value()) {
        return dst_result.error();
    }

    packet_pacer_.set_rtp_sender(std::move(sender));

    return {};
}

std::expected<void, std::string> Client::load_input_device() {
    std::vector<std::string> available_devices = MicRecorder::getAvailableDevices();

    const std::string inputDevice = available_devices[4]; // NOLINT


    if (!mic_input_.setDevice(inputDevice)) {
        return std::unexpected(
            std::format("Failed to start mic recorder with input device : {}", inputDevice));
    }


    mic_input_.setChannelCount(1); // NOLINT
    if (MicRecorder::isAvailable()) { // NOLINT
        return std::unexpected(
            std::format("mic recorder setup with device : {} has failed", inputDevice));
    }

    return {};
}

void Client::run() {
    if (MicRecorder::isAvailable() && !mic_input_.start(8000)) { // NOLINT
        spdlog::critical("Failed to start mic recorder with device : {} ", mic_input_.getDevice());
    }


    for (;;) {
        spdlog::info("Client started");
        io_ctx_.run();
    }
}
