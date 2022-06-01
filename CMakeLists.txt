cmake_minimum_required(VERSION 3.16)

project(SoftIBLRenderer LANGUAGES CXX)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

option(USE_OMP "Use OpenMP to parallel accelerate instead of multi-threads" ON)


include(third_party/glm.cmake)
include(third_party/sdl2.cmake)
include(third_party/spdlog.cmake)

if(USE_OMP)
find_package(OpenMP REQUIRED)
endif()

configure_file(
        "${PROJECT_SOURCE_DIR}/config.hpp.in"
        "${PROJECT_SOURCE_DIR}/src/config.hpp"
)

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
        )

if(USE_OMP)
    target_link_libraries(SoftPBRRenderer PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(
            SoftPBRRenderer PRIVATE /openmp:experimental
    )
    add_compile_definitions(USE_OMP)
endif()

target_include_directories(SoftPBRRenderer PRIVATE
        third_party)

target_compile_features(
        SoftPBRRenderer PRIVATE
        cxx_std_17
)

