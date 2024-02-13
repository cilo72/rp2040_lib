cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

project(rp2040_lib C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

if (DEFINED ENV{PICO_EXAMPLES_PATH} AND NOT PICO_EXAMPLES_PATH)
    set(PICO_EXAMPLES_PATH $ENV{PICO_EXAMPLES_PATH})
    message("Using PICO_EXAMPLES_PATH from environment ('${PICO_EXAMPLES_PATH}')")
endif()

# Initialize the SDK
pico_sdk_init()

include(example_auto_set_url.cmake)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_library(${PROJECT_NAME}
        src/cilo72/core/statemachine.cpp
        src/cilo72/hw/pio.cpp
        src/cilo72/hw/blink_forever.cpp
        src/cilo72/hw/repeating_timer.cpp
        src/cilo72/hw/i2c_bus.cpp
        src/cilo72/hw/spi_bus.cpp
        src/cilo72/hw/spi_device.cpp
        src/cilo72/hw/uart.cpp
        src/cilo72/hw/pwm.cpp
        src/cilo72/hw/gpiokey.cpp
        src/cilo72/hw/adc.cpp
        src/cilo72/ic/ws2812.cpp
        src/cilo72/ic/ky_040.cpp
        src/cilo72/ic/ssd1306.cpp
        src/cilo72/ic/mcp2515.cpp
        src/cilo72/ic/tmc5160.cpp
        src/cilo72/ic/bh1750fvi.cpp
        src/cilo72/ic/sd2405.cpp
        src/cilo72/ic/st7735s.cpp
        src/cilo72/ic/df_player_pro.cpp
        src/cilo72/motion/tmc5160.cpp
        src/cilo72/fonts/font_8x5.cpp
        src/cilo72/fonts/font_acme.cpp
        src/cilo72/fonts/font_bmspa.cpp
        src/cilo72/fonts/font_bubblesstandard.cpp
        src/cilo72/fonts/font_crackers.cpp
        src/cilo72/graphic/COLOR.cpp
        src/cilo72/graphic/framebuffer.cpp
        src/cilo72/graphic/framebuffer_monochrome.cpp
        src/cilo72/graphic/framebuffer_rgb565.cpp
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_generate_pio_header(${PROJECT_NAME} ${PICO_EXAMPLES_PATH}/pio/pio_blink/blink.pio)
pico_generate_pio_header(${PROJECT_NAME} ${PICO_EXAMPLES_PATH}/pio/ws2812/ws2812.pio)

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib hardware_pio)
target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib hardware_i2c)
target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib hardware_spi)
target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib hardware_pwm)
target_link_libraries(${PROJECT_NAME} PRIVATE pico_stdlib hardware_adc)

#pico_add_extra_outputs(${PROJECT_NAME})
