cmake_minimum_required(VERSION 3.9)
project(Example01_SingleThread_C_IMPL)

set(Example01_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" MATCHES ^$)
    set(CMAKE_BUILD_TYPE "Debug")
    message("CMAKE_BUILD_TYPE is set to Debug by default")
endif()

add_executable(Example01
    ${Example01_SOURCES})

target_link_libraries(Example01 PUBLIC pthread)
