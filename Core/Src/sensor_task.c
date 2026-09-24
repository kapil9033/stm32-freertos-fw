#include "sensor_task.h"
#include "task.h"
#include <stdlib.h>

QueueHandle_t xSensorQueue = NULL;

void vSensorTask(void *pvParameters) {
    SensorData_t sensorFrame;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // 10Hz sampling frequency

    for (;;) {
        // Simulate reading hardware sensors (ADC / I2C)
        sensorFrame.temperature = 22.0f + ((float)(rand() % 100) / 10.0f);
        sensorFrame.vibration = 0.05f + ((float)(rand() % 50) / 100.0f);
        sensorFrame.timestamp_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;

        // Push frame to UART Queue without blocking if full
        if (xSensorQueue != NULL) {
            xQueueSend(xSensorQueue, &sensorFrame, (TickType_t)0);
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}
