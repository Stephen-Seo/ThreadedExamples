cmake_minimum_required(VERSION 3.9)
project(Example02)

set(Example02_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/argParse.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rayTracer.cpp"
)

set(Example02_ALL_FILES
    ${Example02_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/argParse.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rayTracer.hpp"
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" MATCHES ^$)
    set(CMAKE_BUILD_TYPE "Debug")
    message("CMAKE_BUILD_TYPE is set to Debug by default")
endif()

add_executable(Example02
    ${Example02_SOURCES})

target_compile_features(Example02 PUBLIC cxx_std_17)

find_package(Threads REQUIRED)
target_link_libraries(Example02 PUBLIC ${CMAKE_THREAD_LIBS_INIT})

find_package(glm REQUIRED)
if(TARGET glm::glm)
    get_target_property(GLM_INCLUDE_DIRS glm::glm INTERFACE_INCLUDE_DIRECTORIES)
elseif(TARGET glm)
    get_target_property(GLM_INCLUDE_DIRS glm INTERFACE_INCLUDE_DIRECTORIES)
else()
    message(FATAL_ERROR "Failed to set up glm for project")
endif()
target_include_directories(Example02 PUBLIC ${GLM_INCLUDE_DIRS})

find_package(PNG REQUIRED)
target_include_directories(Example02 PUBLIC ${PNG_INCLUDE_DIRS})
target_link_libraries(Example02 PUBLIC ${PNG_LIBRARIES})

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(
        clang-format ALL
        COMMAND /usr/bin/clang-format
        -i
        -style=file
        ${Example02_ALL_FILES}
    )
    add_dependencies(Example02 clang-format)
endif()

find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    add_custom_target(
        clang-tidy
        COMMAND /usr/bin/clang-tidy
        ${Example02_ALL_FILES}
        -config-file="${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy"
        --
        -std=c++17
    )
endif()
