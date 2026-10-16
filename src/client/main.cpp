

#include <spdlog/spdlog.h>
#include <asio.hpp>
#include <memory>
#include "RtpCpp.hpp"

int main(int /*argc*/, const char* const* const /*argv*/) {
    spdlog::info("client started");
    asio::io_context ioc;
}
