cmake_minimum_required(VERSION 3.10)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(template_cmake)

find_package(fmt REQUIRED)

add_subdirectory(src/core)
add_subdirectory(src/app)
