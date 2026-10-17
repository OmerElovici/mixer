#pragma once

#include <SFML/Audio.hpp>
#include "ISamplesProcessor.hpp"

class MicRecorder : public sf::SoundRecorder {
public:
    // static std::expected<std::unique_ptr<MicRecorder>, std::error_code> create(
    //     const std::string& input_device);


    MicRecorder(const MicRecorder& other) = delete;
    MicRecorder& operator=(const MicRecorder& other) = delete;

    MicRecorder(MicRecorder&& other) noexcept = delete;
    MicRecorder& operator=(MicRecorder&& other) noexcept = delete;


    void set_processor(ISamplesProcessor* processor);
    ~MicRecorder() override = default;

    MicRecorder() = default;

protected:
    bool onStart() override;

    bool onProcessSamples(const std::int16_t* samples, std::size_t sample_count) override;


private:
    ISamplesProcessor* processor_ = nullptr;
};
