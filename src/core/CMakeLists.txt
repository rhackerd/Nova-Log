# Collect all source and header files recursively
file(GLOB_RECURSE logger_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Define the static library target
add_library(Nova_Logger STATIC ${logger_sources})

# Set include directories for the library
target_include_directories(Nova_Logger PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link required libraries
target_link_libraries(Nova_Logger PUBLIC
    spdlog::spdlog
)

# Create an alias to use it like Nova::Logger
add_library(Nova::Logger ALIAS Nova_Logger)
