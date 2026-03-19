## Include this file if you want to use the Pico-DMX library
## in YOUR (Pico-C-SDK-based) project.

cmake_minimum_required(VERSION 3.12)

# Define the Pico-DMX library
add_library(picodmx INTERFACE)

target_sources(picodmx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/DmxInput.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DmxOutput.cpp
)

pico_generate_pio_header(picodmx
    ${CMAKE_CURRENT_LIST_DIR}/DmxInput.pio
)
pico_generate_pio_header(picodmx
    ${CMAKE_CURRENT_LIST_DIR}/DmxOutput.pio
)

target_include_directories(picodmx INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
