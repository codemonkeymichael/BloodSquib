# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Prefer a repo-local Pico SDK (under lib/pico-sdk) if present.
# This lets the project be self-contained without relying on a global SDK install.
if (EXISTS "${CMAKE_SOURCE_DIR}/lib/pico-sdk/CMakeLists.txt")
    set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/lib/pico-sdk" CACHE PATH "Path to Raspberry Pi Pico SDK" FORCE)
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(lib/pico_sdk_import.cmake)
include(lib/pico_extras_import_optional.cmake)

add_compile_definitions(PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64)

project(bloodsquib C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

# Pull in the DMX library (from the repo-local lib/ folder)
include(lib/picodmx.cmake)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

add_executable(bloodsquib)

target_sources(bloodsquib PUBLIC
        main.cpp
)

# pull in common dependencies
target_link_libraries(bloodsquib pico_stdlib picodmx hardware_dma hardware_pio)

# Enable the shell over UART (which is the picoprobe!)
pico_enable_stdio_usb(bloodsquib 1)
pico_enable_stdio_uart(bloodsquib 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(bloodsquib)
