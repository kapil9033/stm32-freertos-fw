#include "uart_task.h"
#include "sensor_task.h"
#include <stdio.h>
#include <string.h>

void vUARTTask(void *pvParameters) {
    SensorData_t receivedData;
    char txBuffer[128];

    for (;;) {
        // Wait indefinitely for new sensor telemetry from the queue
        if (xQueueReceive(xSensorQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            // Format telemetry frame into JSON
            int len = snprintf(txBuffer, sizeof(txBuffer),
                               "{\"time_ms\":%lu,\"temp_c\":%.2f,\"vib_g\":%.2f}\r\n",
                               receivedData.timestamp_ms,
                               receivedData.temperature,
                               receivedData.vibration);

            // Transmit frame via UART (e.g. HAL_UART_Transmit or stdout redirect)
            // Replace stdout/printf with HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, len, HAL_MAX_DELAY);
            printf("%s", txBuffer);
        }
    }
}
