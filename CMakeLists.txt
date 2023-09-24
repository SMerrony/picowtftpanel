# SPDX-FileCopyrightText: 2023 Stephen Merrony
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(
	picowtftpanel
	VERSION 0.1.0
	LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

set(PICO_BOARD pico_w)
pico_sdk_init()

add_subdirectory(pico_dht/dht)

add_executable(picowtftpanel ${SOURCES})

pico_generate_pio_header(picowtftpanel ${CMAKE_CURRENT_LIST_DIR}/src/lcd.pio)

target_sources(picowtftpanel PUBLIC
	${CMAKE_SOURCE_DIR}/src/graphics.c
	${CMAKE_SOURCE_DIR}/src/info_items.c
	${CMAKE_SOURCE_DIR}/src/lcd.c
	${CMAKE_SOURCE_DIR}/src/mqtt.c
	${CMAKE_SOURCE_DIR}/src/picowtftpanel.c
	${CMAKE_SOURCE_DIR}/src/picowtftpanel.c
)

target_include_directories(picowtftpanel PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# target_compile_definitions(picowtftpanel PRIVATE
#	PICO_DEFAULT_UART_TX_PIN=28
# 	PICO_DEFAULT_UART_RX_PIN=29
# )

# target_compile_options(picowtftpanel PRIVATE -Werror -Wall -Wextra)
# target_compile_options(picowtftpanel PRIVATE -Wall -Wextra)

pico_enable_stdio_usb(picowtftpanel 1) # STDIO to USB for development

target_link_libraries(picowtftpanel 
	dht
	hardware_pio
	hardware_pwm
	pico_cyw43_arch_lwip_threadsafe_background 
	pico_lwip_mqtt
	pico_mem_ops
	pico_multicore
	pico_stdlib
	pico_sync 
)

pico_add_extra_outputs(picowtftpanel)
