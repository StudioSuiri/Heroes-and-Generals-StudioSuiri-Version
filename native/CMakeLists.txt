cmake_minimum_required(VERSION 3.25)
project(HnGNative VERSION 0.1.0 LANGUAGES CXX)
include(CTest)
option(HNG_BUILD_CLIENT "Build the native SDL3 desktop client" ON)
option(HNG_BUILD_SERVER "Build the headless dedicated server" ON)
option(HNG_BUILD_TESTS "Build and register local unit tests" ON)
option(HNG_FETCH_SDL3 "Fetch pinned SDL3 source if no system package is found" OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(hng_core src/core/protocol.cpp src/core/simulation.cpp src/core/udp_socket.cpp)
target_include_directories(hng_core PUBLIC include)
target_compile_features(hng_core PUBLIC cxx_std_20)
if(WIN32)
  target_link_libraries(hng_core PUBLIC ws2_32)
endif()
if(MSVC)
  target_compile_options(hng_core PRIVATE /W4 /permissive-)
else()
  target_compile_options(hng_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(HNG_BUILD_SERVER)
  add_executable(hng-server src/server/main.cpp)
  target_link_libraries(hng-server PRIVATE hng_core)
endif()

if(HNG_BUILD_CLIENT)
  find_package(SDL3 3.2 CONFIG QUIET)
  if(NOT TARGET SDL3::SDL3 AND HNG_FETCH_SDL3)
    include(FetchContent)
    set(SDL_TESTS OFF CACHE BOOL "Disable SDL tests in this build" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "Disable SDL examples in this build" FORCE)
    FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.4.18
      GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif()
  if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "SDL3 3.2+ is required. Install it or configure with -DHNG_FETCH_SDL3=ON. Fetching happens only when this project is built locally.")
  endif()
  add_executable(hng-client src/client/main.cpp)
  target_link_libraries(hng-client PRIVATE hng_core SDL3::SDL3)
  if(WIN32 AND TARGET SDL3::SDL3-shared)
    add_custom_command(TARGET hng-client POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:hng-client>
      VERBATIM)
  endif()
endif()

if(HNG_BUILD_TESTS)
  enable_testing()
  add_executable(hng-core-tests tests/core_tests.cpp)
  target_link_libraries(hng-core-tests PRIVATE hng_core)
  add_test(NAME hng-core-tests COMMAND hng-core-tests)
endif()
