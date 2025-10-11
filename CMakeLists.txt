cmake_minimum_required(VERSION 3.10)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

project(template_cmake)

CPMAddPackage("gh:gabime/spdlog@1.15.3")

add_subdirectory(src/core)
add_subdirectory(src/app)
