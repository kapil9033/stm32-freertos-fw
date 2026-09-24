#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "sensor_task.h"
#include "uart_task.h"

int main(void) {
    // 1. Initialize Hardware (HAL, Clocks, GPIO, USART2 @ 115200 8N1)
    // HAL_Init();
    // SystemClock_Config();
    // MX_USART2_UART_Init();

    // 2. Create FreeRTOS Inter-Task Communication Queue (depth: 10 items)
    xSensorQueue = xQueueCreate(10, sizeof(SensorData_t));

    if (xSensorQueue != NULL) {
        // 3. Create Sensor Reader Task (Priority 2)
        xTaskCreate(vSensorTask, "SensorTask", 256, NULL, 2, NULL);

        // 4. Create UART Transmission Task (Priority 1)
        xTaskCreate(vUARTTask, "UARTTask", 256, NULL, 1, NULL);

        // 5. Start Scheduler (Control transferred to FreeRTOS)
        vTaskStartScheduler();
    }

    // Infinite loop if scheduler allocation fails
    while (1);
}
