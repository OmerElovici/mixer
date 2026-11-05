add_executable(client src/main.cpp
  src/MicRecorder.cpp
  src/Pacer.cpp
  src/Client.cpp
  src/AudioChunk.cpp)

target_link_libraries(
  client
  PRIVATE mixer::mixer_options
          mixer::mixer_warnings
          SFML::Audio
          rtpcpp
          spdlog::spdlog
          readerwriterqueue
)

mixer_configure_linker(client)
