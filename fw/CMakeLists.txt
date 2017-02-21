cmake_minimum_required(VERSION 3.4)

project(hodiny)


#todo
#cmakefiles for my modules


set (USER_SOURCES 
main.cpp
board_f100/board.c
matrix_driver/matrix_abstraction.cpp

fonts/terminus16_bold.c
fonts/terminus12_bold.c

menu/menu.cpp

menu/screens.cpp
menu/configuration.c
menu/crc8.c
menu/print.cpp

rtc/rtc_control.c
wifi/wifi.cpp
flash/stm32f10x_flash.c

)

set (EXTRA_HEADERS
halconf.h
chconf.h
mcuconf.h
platform.h
board_f100/board.h
matrix_driver/matrix_driver.h
matrix_driver/matrix_abstraction.h
#fonts/ptypes.h
#fonts/passert.h
#fonts/pfont.h

menu/menu.h
menu/configuration.h
menu/crc8.h
menu/screens.h
menu/print.h

rtc/rtc_control.h
wifi/wifi.h

flash/stm32f10x_flash.h
)

set (OPTIMIZED_SOURCES
matrix_driver/matrix_driver.c
)

include_directories(
../../../../bootloader/boot
board_f100
)

set (INCLUDE_DIRS
matrix_driver
menu
wifi
dcf
rtc
flash

)

set (USER_PROGRAM_START 0x08003800)
set (USE_BOOTLOADER 0)

#set (LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32F100xB.ld")

set (MAIN_STACK_SIZE 0x400)
set (PROCESS_STACK_SIZE 0x400)
set (OPTIMIZE_RTOS 1)

set (GLOBAL_FLAGS
"-O0  -ggdb "
)

set (OPTIMIZED_FLAGS
"-O2"
)

set (CFLAGS

)

set (CPPFLAGS
"-fno-rtti -fno-exceptions"
)

#family or chip
set (CHIP "STM32F100C8")
#set (CHIP "STM32F407VG")
#chibios from https://github.com/kubanecxxx/chibios_fork commit hash
#ccb51e8f5e04b7aec1c0fe87b7e3d1dd5d634bb4
set (CHIBIOS_DIR "/home/kubanec/workspaces/chibios_3stable")
if (NOT EXISTS ${CHIBIOS_DIR})
	set (CHIBIOS_DIR "/home/kuba/chibi/chibios_fork")
endif()
#set (BOARD "ST_STM32F4_DISCOVERY")

set (FLASH_SIZE "64k")
set (RAM_SIZE "8k")

set (TOOLCHAIN "arm-none-eabi")


set(USE_COMMON_LIBRARY 1)
set(LIBRARY_MODULES esp crc scheduler)

set(USE_PIRIS 1)

include (${CHIBIOS_DIR}/CMakeLists.txt)
