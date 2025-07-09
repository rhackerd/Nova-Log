file(GLOB_RECURSE core_sources
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(core STATIC ${core_sources})

target_include_directories(core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link required libraries
target_link_libraries(core PUBLIC

)
