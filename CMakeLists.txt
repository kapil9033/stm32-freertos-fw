cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

project(stm32-freertos-fw C CXX ASM)

set(CMAKE_C_STANDARD 11)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Core/STM32F446RETx_FLASH.ld)
set(CPU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CPU_FLAGS} -Wall -g -D__FPU_PRESENT=1 -D__FPU_USED=1")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${CPU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-T${LINKER_SCRIPT} ${CPU_FLAGS} -Wl,-Map=${PROJECT_NAME}.map -Wl,--gc-sections --specs=nosys.specs")

include_directories(
    Core/Inc
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
)

file(GLOB CORE_SOURCES "Core/Src/*.c" "Core/Src/*.s")
file(GLOB FREERTOS_SOURCES "Middlewares/Third_Party/FreeRTOS/Source/*.c")

add_executable(${PROJECT_NAME}.elf 
    ${CORE_SOURCES}
    ${FREERTOS_SOURCES}
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
)
