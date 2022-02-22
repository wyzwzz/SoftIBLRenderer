cmake_minimum_required(VERSION 3.16)

project(SoftPBRRenderer LANGUAGES CXX)

include(third_party/glm.cmake)
include(third_party/sdl2.cmake)

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
        )

target_include_directories(SoftPBRRenderer PRIVATE
        third_party)

target_compile_features(
        SoftPBRRenderer PRIVATE
        cxx_std_17
)