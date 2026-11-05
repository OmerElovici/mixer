#pragma once


#include <RtpCpp.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <expected>
#include <string>
#include <system_error>
#include "MicRecorder.hpp"
#include "Pacer.hpp"

class Client {
public:
    std::error_code bind();
    std::expected<void, std::string> load_input_device();
    void  run();
    Client();
private:

    asio::io_context io_ctx_;
    asio::executor_work_guard<asio::io_context::executor_type> io_wrk_guard_;
    Pacer packet_pacer_;
    MicRecorder mic_input_;
};
