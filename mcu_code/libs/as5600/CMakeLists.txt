cmake_minimum_required(VERSION 3.12)
project(as5600)

# Static library
add_library(as5600 STATIC
    src/as5600.c
)

# Include directories
target_include_directories(as5600 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link Pico SDK libraries
target_link_libraries(as5600 PUBLIC pico_stdlib hardware_i2c)