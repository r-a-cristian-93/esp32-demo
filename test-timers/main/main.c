#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/uart.h"

QueueHandle_t uart_queue;

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
    char* msg = "MACHINE IS ALIVE";
    ESP_LOGI("BEACON", "MACHINE IS ALIVE");
    uart_write_bytes(UART_NUM_1, (const char*) msg, strlen(msg));
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
    // settup serial communication
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    const int uart_buffer_size = (1024 * 2);

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, 4, 5, 18, 19));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

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

