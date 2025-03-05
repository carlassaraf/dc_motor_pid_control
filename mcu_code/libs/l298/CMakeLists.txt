cmake_minimum_required(VERSION 3.12)
project(l298)

# Static library
add_library(l298 STATIC
    src/l298.c
)

# Include directories
target_include_directories(l298 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link Pico SDK libraries
target_link_libraries(l298 PUBLIC pico_stdlib hardware_pwm)