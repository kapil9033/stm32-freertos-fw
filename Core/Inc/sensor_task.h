#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

typedef struct {
    float temperature;
    float vibration;
    uint32_t timestamp_ms;
} SensorData_t;

extern QueueHandle_t xSensorQueue;

void vSensorTask(void *pvParameters);

#endif // SENSOR_TASK_H
