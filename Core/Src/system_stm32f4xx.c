#include <stdint.h>

// Direct memory address for ARM Cortex-M4 SCB->CPACR (Coprocessor Access Control Register)
#define SCB_CPACR (*((volatile uint32_t *)0xE000ED88))

uint32_t SystemCoreClock = 16000000;

void SystemInit(void) {
    // Enable FPU (Full Access to CP10 and CP11 coprocessors)
    SCB_CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
}
