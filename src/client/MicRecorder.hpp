#pragma once

#include <SFML/Audio.hpp>
#include <expected>
#include <memory>
#include <string>
#include <system_error>

class MicRecorder : public sf::SoundRecorder {
public:
    static std::expected<std::unique_ptr<MicRecorder>, std::error_code> create(
        const std::string& input_device);

    MicRecorder(const MicRecorder& other) = delete;
    MicRecorder& operator=(const MicRecorder& other) = delete;

    MicRecorder(MicRecorder&& other) noexcept = delete;
    MicRecorder& operator=(MicRecorder&& other) noexcept = delete;

    ~MicRecorder() override = default;

protected:
    bool onProcessSamples(const std::int16_t* samples, std::size_t sample_count) override;


    // bool onProcessSamples(const std::int16_t* samples, std::size_t sampleCount) override
    // {
    //     // do something useful with the new chunk of samples
    //     ...

    //     // return true to continue the capture, or false to stop it
    //     return true;
    // }

private:
    MicRecorder() = default;
};