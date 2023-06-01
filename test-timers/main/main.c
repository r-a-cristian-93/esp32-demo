#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "esp_log.h"

enum {
    TIMER_BEACON,
    TIMER_WORK_1,
    TIMER_WORK_2,
    TIMER_NUM
};

TimerHandle_t timers[TIMER_NUM];

int pots;
SemaphoreHandle_t pot_mutex;

void beaconSignal(TimerHandle_t timer) {
    ESP_LOGI("BEACON", "MACHINE IS ALIVE");
}

void potFurnace(TimerHandle_t timer) {
    if (xSemaphoreTake(pot_mutex, 0) == pdTRUE) {
        pots++;
        xSemaphoreGive(pot_mutex);
    }
}

void productionReport(TimerHandle_t timer) {
    if (xSemaphoreTake(pot_mutex, 0) == pdTRUE) {
        printf("We have %d pots in stock\n", pots);
        xSemaphoreGive(pot_mutex);
    }
}

void app_main(void)
{
    pot_mutex = xSemaphoreCreateMutex();

    timers[TIMER_BEACON] = xTimerCreate(
            "BEACON",
            pdMS_TO_TICKS(500),
            pdTRUE,
            (void*)0,
            beaconSignal);
    
    timers[TIMER_WORK_1] = xTimerCreate(
            "FURNACE",
            pdMS_TO_TICKS(100),
            pdTRUE,
            (void*)0,
            potFurnace);

    timers[TIMER_WORK_2] = xTimerCreate(
            "REPORT",
            pdMS_TO_TICKS(2000),
            pdTRUE,
            (void*)0,
            productionReport);

    for (int i = 0; i < TIMER_NUM; i++) {
        if (timers[i] != NULL) {
            xTimerStart(timers[i], 0);
        }
    }
}

