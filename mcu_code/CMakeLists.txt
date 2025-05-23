# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(mcu_code C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3" CACHE STRING "Debug C Flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3" CACHE STRING "Debug C++ Flags" FORCE)

# Add executable. Default name is the project name, version 0.1

add_executable(mcu_code mcu_code.c)

pico_set_program_name(mcu_code "mcu_code")
pico_set_program_version(mcu_code "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(mcu_code 0)
pico_enable_stdio_usb(mcu_code 1)

# Aditional libraries
add_subdirectory(libs/l298)
add_subdirectory(libs/pid)
add_subdirectory(libs/as5600)

# Add the standard library to the build
target_link_libraries(mcu_code
    pico_stdlib
    hardware_adc
    l298
    pid
    as5600
)

# Add the standard include files to the build
target_include_directories(mcu_code PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(mcu_code)

