include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(mixer_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.17.0
      GITHUB_REPOSITORY
      "gabime/spdlog"
      SYSTEM
      YES)
  endif()

  if(NOT TARGET asio::asio)
    cpmaddpackage(
      NAME
      asio
      VERSION
      1.30.2
      GIT_TAG
      asio-1-30-2
      GITHUB_REPOSITORY
      "chriskohlhoff/asio"
      SYSTEM
      YES)

    find_package(Threads REQUIRED)

   add_library(asio INTERFACE)
   add_library(asio::asio ALIAS asio)

    target_include_directories(asio SYSTEM INTERFACE
      "${asio_SOURCE_DIR}/asio/include"
    ) 

    target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
    target_link_libraries(asio INTERFACE Threads::Threads)

    if(WIN32)
      target_link_libraries(asio INTERFACE ws2_32 mswsock)
    endif()

  endif()


  if(NOT TARGET SFML::Audio)
  cpmaddpackage(
    NAME SFML
    VERSION 3.1.0
    GIT_TAG 3.1.0
    GITHUB_REPOSITORY "SFML/SFML"
    SYSTEM YES
    OPTIONS
      "SFML_BUILD_AUDIO ON"
      "SFML_BUILD_GRAPHICS OFF"
      "SFML_BUILD_WINDOW OFF"
      "SFML_BUILD_NETWORK OFF"
      "SFML_CONFIGURE_EXTRAS OFF"
      "SFML_USE_SYSTEM_DEPS OFF"
  )
endif()



endfunction()
