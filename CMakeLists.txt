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
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-windrose-embedded C CXX ASM)

file(GLOB SRC_FILES src/*.c)
file(GLOB CJSON_FILES
    external/cJSON/cJSON.c
    external/cJSON/cJSON_Utils.c
    )
file(GLOB_RECURSE DRIVER_FILES src/drivers/*.c)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adicionando arquivo principal do projeto
add_executable(pico-windrose-embedded 
    ${DRIVER_FILES}
    ${SRC_FILES}
    ${CJSON_FILES}
    )

pico_set_program_name(pico-windrose-embedded "pico-windrose-embedded")
pico_set_program_version(pico-windrose-embedded "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pico-windrose-embedded 0)
pico_enable_stdio_usb(pico-windrose-embedded 1)

# Add the standard library to the build
target_link_libraries(pico-windrose-embedded
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(pico-windrose-embedded PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${CMAKE_CURRENT_LIST_DIR}/external/cJSON
)

# Add any user requested libraries
target_link_libraries(pico-windrose-embedded 
        pico_cyw43_arch_lwip_threadsafe_background
        hardware_adc
        )

pico_add_extra_outputs(pico-windrose-embedded)

