#include <stdint.h>
#include <sys/stat.h>

// Direct hardware address registers for STM32F446
#define RCC_AHB1ENR   (*((volatile uint32_t *)0x40023830))
#define RCC_APB1ENR   (*((volatile uint32_t *)0x40023840))
#define GPIOA_MODER   (*((volatile uint32_t *)0x40020000))
#define GPIOA_AFRL    (*((volatile uint32_t *)0x40020020))
#define USART2_SR     (*((volatile uint32_t *)0x40004400))
#define USART2_DR     (*((volatile uint32_t *)0x40004404))
#define USART2_BRR    (*((volatile uint32_t *)0x40004408))
#define USART2_CR1    (*((volatile uint32_t *)0x4000440C))

// System interrupt handler required by vector table
void SysTick_Handler(void) {
    // SysTick dummy handler for non-OS testing
}

// POSIX System Calls required to silence Newlib linker warnings
int _read(int file, char *ptr, int len) { return 0; }
int _write(int file, char *ptr, int len) { return len; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _close(int file) { return -1; }
int _fstat(int file, struct stat *st) { st->st_mode = S_IFCHR; return 0; }
int _isatty(int file) { return 1; }
int _kill(int pid, int sig) { return -1; }
int _getpid(void) { return 1; }

int main(void) {
    // 1. Enable Clocks (GPIOA + USART2)
    RCC_AHB1ENR |= (1 << 0);
    RCC_APB1ENR |= (1 << 17);

    // 2. Configure PA2 as Alternate Function AF7 (USART2_TX)
    GPIOA_MODER &= ~(3 << (2 * 2));
    GPIOA_MODER |=  (2 << (2 * 2));
    GPIOA_AFRL  &= ~(0xF << 8);
    GPIOA_AFRL  |=  (7 << 8);

    // 3. Set Baud Rate (115200 @ 16MHz default HSI clock)
    USART2_BRR = 0x008B;

    // 4. Enable Transmitter & USART2
    USART2_CR1 |= (1 << 3) | (1 << 13);

    const char *msg = "PING FROM STM32 HARDWARE\r\n";

    while (1) {
        for (const char *p = msg; *p; p++) {
            while (!(USART2_SR & (1 << 7))); // Wait until TXE
            USART2_DR = *p;
        }
        for (volatile int i = 0; i < 1000000; i++); // Hardware delay
    }
}
