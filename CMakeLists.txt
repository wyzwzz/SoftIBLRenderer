cmake_minimum_required(VERSION 3.16)

project(SoftPBRRenderer LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include(third_party/glm.cmake)
include(third_party/sdl2.cmake)
include(third_party/spdlog.cmake)

find_package(OpenMP)

file(
        GLOB
        SoftPBRRenderer_SRCS
        src/*.cpp
        src/*.hpp
)

add_executable(SoftPBRRenderer ${SoftPBRRenderer_SRCS})

target_link_libraries(SoftPBRRenderer PRIVATE
        glm
        SDL2-static
        spdlog::spdlog
        OpenMP::OpenMP_CXX
        )

target_include_directories(SoftPBRRenderer PRIVATE
        third_party)

target_compile_features(
        SoftPBRRenderer PRIVATE
        cxx_std_17
)
target_compile_options(
        SoftPBRRenderer PRIVATE /openmp:experimental
)