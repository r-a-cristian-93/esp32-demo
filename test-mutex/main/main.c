#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"

static int shared_var = 0;
static SemaphoreHandle_t mutex;

void increment(void* param) {
    int local_var;

    while(1) 
    {
        if (xSemaphoreTake(mutex, 0) == pdTRUE)
        {
            local_var = shared_var;
            local_var++;
            vTaskDelay(pdMS_TO_TICKS((rand() % 10 )*20));        
            shared_var = local_var;
            ESP_LOGI("INC","%d", shared_var);
            xSemaphoreGive(mutex);
        }
        else {
            ESP_LOGW("INC", "Can't take mutex.");
        }
        vTaskDelay(pdMS_TO_TICKS(400));
    }
    vTaskDelete(NULL);
}

void workloadSuperviser(void* param) {
    while (1) 
    {
        if (xSemaphoreTake(mutex, 0) == pdTRUE)
        {
            if (shared_var > 10) {
                esp_err_t sleep_error = esp_light_sleep_start();
                if (sleep_error != ESP_OK)
                {
                    ESP_LOGE("SUPERVISER", "Can't put to sleep %s", esp_err_to_name(sleep_error));
                }
                else {
                    shared_var = 0;
                }
            }
            xSemaphoreGive(mutex);
            vTaskDelay(pdMS_TO_TICKS(300));
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    mutex = xSemaphoreCreateMutex();

    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(1 * 1000000));

    xTaskCreate(increment, "INCREMENT", 1024*2, NULL, 1, NULL);
    xTaskCreate(increment, "INCREMENT", 1024*2, NULL, 1, NULL);
    xTaskCreate(workloadSuperviser, "SUPERVISE", 1024*2, NULL, 1, NULL);
}
