add_executable(client main.cpp
  MicRecorder.cpp
  Pacer.cpp
  Client.cpp
  AudioChunk.cpp)

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
