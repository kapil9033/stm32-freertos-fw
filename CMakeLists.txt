cmake_minimum_required(VERSION 3.16)
project(stm32-freertos-fw C CXX ASM)

set(CMAKE_C_STANDARD 11)

# Target Board Definitions
add_definitions(-DSTM32F446xx -DUSE_HAL_DRIVER)

include_directories(
    Core/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
    Drivers/STM32F4xx_HAL_Driver/Inc
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)

file(GLOB_RECURSE SOURCES
    "Core/Src/*.c"
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})
