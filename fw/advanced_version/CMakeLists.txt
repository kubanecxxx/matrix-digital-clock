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
wifi/wifi.c
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

#set (LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32F100xB.ld")

set (MAIN_STACK_SIZE 0x400)
set (PROCESS_STACK_SIZE 0x400)
set (OPTIMIZE_RTOS 1)

set (GLOBAL_FLAGS
"-O0  -ggdb "
)

set (OPTIMIZED_FLAGS
"-Os"
)

set (CFLAGS

)

set (CPPFLAGS
"-fno-rtti -fno-exceptions"
)

#family or chip
set (CHIP "STM32F100C8")
#set (CHIP "STM32F407VG")
# chibiOS stable v3.0.x branch commit ed8937f718ace39051768bb45c66a195b48270b3
set (CHIBIOS_DIR "/home/kubanec/workspaces/chibios_3stable")
if (NOT EXISTS ${CHIBIOS_DIR})
	set (CHIBIOS_DIR "/home/kuba/chibi/ChibiOS")
endif()
#set (BOARD "ST_STM32F4_DISCOVERY")

set (FLASH_SIZE "64k")
set (RAM_SIZE "8k")

set (TOOLCHAIN "arm-none-eabi")

include (${CHIBIOS_DIR}/CMakeLists.txt)

set (PROJECT_DIRECTORY ${CMAKE_SOURCE_DIR}/../..)

add_subdirectory(${PROJECT_DIRECTORY}/piris/framework ${CMAKE_CURRENT_BINARY_DIR}/piris)
target_link_libraries(${PROJECT_NAME} piris)

set (LIBRARY_MODULES scheduler)
add_subdirectory("/home/kubanec/workspace/library" "${CMAKE_CURRENT_BINARY_DIR}/library")
target_link_libraries(${PROJECT_NAME} library)

