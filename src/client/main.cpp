

#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <asio.hpp>
#include "Client.hpp"

int main(int /*argc*/, const char* const* const /*argv*/) {
    Client client;
    if (const auto bind_err = client.bind(); bind_err) {
        spdlog::critical("Failed to bind client sender: {}", bind_err.message());
        return -1;
    }

    // auto input_dev_err = client.load_input_device();
    if (auto input_dev_err = client.load_input_device(); input_dev_err) {
        spdlog::critical("Failed to bind client sender: {}", input_dev_err.error());
        return -1;
    }

    client.run();
}
