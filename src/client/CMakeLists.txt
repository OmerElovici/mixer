add_executable(client main.cpp
  MicRecorder.cpp)

target_link_libraries(
  client
  PRIVATE mixer::mixer_options
          mixer::mixer_warnings
          SFML::Audio
          rtpcpp
          spdlog::spdlog
)

mixer_configure_linker(client)
