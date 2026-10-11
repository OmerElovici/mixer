add_library(rtpcpp STATIC RtpCpp.cpp)

set_target_properties(rtpcpp PROPERTIES
  SYSTEM TRUE
  CXX_CLANG_TIDY ""
)

target_include_directories(rtpcpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_compile_definitions(rtpcpp PUBLIC RTPCPP_USE_STANDALONE_ASIO)

target_link_libraries(rtpcpp PUBLIC asio::asio)