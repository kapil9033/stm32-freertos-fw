#include "uart_task.h"
#include "sensor_task.h"
#include <stdio.h>
#include <string.h>

// Direct STM32F446 Register definitions for USART2 & GPIOA
#define RCC_AHB1ENR   (*((volatile uint32_t *)0x40023830))
#define RCC_APB1ENR   (*((volatile uint32_t *)0x40023840))
#define GPIOA_MODER   (*((volatile uint32_t *)0x40020000))
#define GPIOA_AFRH    (*((volatile uint32_t *)0x40020024))
#define GPIOA_AFRL    (*((volatile uint32_t *)0x40020020))
#define USART2_SR     (*((volatile uint32_t *)0x40004400))
#define USART2_DR     (*((volatile uint32_t *)0x40004404))
#define USART2_BRR    (*((volatile uint32_t *)0x40004408))
#define USART2_CR1    (*((volatile uint32_t *)0x4000440C))

static void init_usart2_hardware(void) {
    // 1. Enable Clocks for GPIOA and USART2
    RCC_AHB1ENR |= (1 << 0);  // GPIOA clock enable
    RCC_APB1ENR |= (1 << 17); // USART2 clock enable

    // 2. Configure PA2 (TX) and PA3 (RX) as Alternate Function AF7 (USART2)
    GPIOA_MODER &= ~((3 << (2 * 2)) | (3 << (2 * 3)));
    GPIOA_MODER |=  ((2 << (2 * 2)) | (2 << (2 * 3))); // AF mode
    GPIOA_AFRL  &= ~((0xF << 8) | (0xF << 12));
    GPIOA_AFRL  |=  ((7 << 8) | (7 << 12));            // AF7 for PA2/PA3

    // 3. Configure Baud Rate to 115200 @ 16MHz default HSI clock
    USART2_BRR = 0x008B; // 16MHz / (16 * 115200)

    // 4. Enable Transmitter, Receiver, and USART2 module
    USART2_CR1 |= (1 << 3) | (1 << 2) | (1 << 13);
}

static void usart2_send_char(char c) {
    while (!(USART2_SR & (1 << 7))); // Wait until TX buffer empty (TXE)
    USART2_DR = (c & 0xFF);
}

static void usart2_send_string(const char *str) {
    while (*str) {
        usart2_send_char(*str++);
    }
}

void vUARTTask(void *pvParameters) {
    SensorData_t receivedData;
    char txBuffer[128];

    // Initialize physical UART hardware
    init_usart2_hardware();

    for (;;) {
        if (xQueueReceive(xSensorQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            snprintf(txBuffer, sizeof(txBuffer),
                     "{\"time_ms\":%lu,\"temp_c\":%.2f,\"vib_g\":%.2f}\r\n",
                     receivedData.timestamp_ms,
                     receivedData.temperature,
                     receivedData.vibration);

            // Transmit JSON frame directly over physical USART2 wire to USB
            usart2_send_string(txBuffer);
        }
    }
}
