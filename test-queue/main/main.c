#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define MSG_QUE_LEN 5
#define FREQUENCY_MS_PRINT 1000
#define FREQUENCY_MS_SEND 100

static QueueHandle_t msg_queue;

// print all queued messages
void printMessages(void* param)
{
	int item;

	while(1)
	{
		while (xQueueReceive(msg_queue, (void*)&item, 0) == pdTRUE)
		{
			printf("%d ", item);
		}
		printf("\n");
		vTaskDelay(pdMS_TO_TICKS(FREQUENCY_MS_PRINT));
	}

	vTaskDelete(NULL);
}

// send messages to queue
void sendMessages(void* param)
{
	static int num = 0;

	while (1) {
		if (xQueueSend(msg_queue, (void*)&num, 10) != pdTRUE) {
			ESP_LOGE("SENDER","Faild to send: %d", num);
		}
		num++;
		vTaskDelay(pdMS_TO_TICKS(FREQUENCY_MS_SEND));
	}

	vTaskDelete(NULL);
}

void app_main(void) {
	printf("MCU STARTED \n\n");

	msg_queue = xQueueCreate(MSG_QUE_LEN, sizeof(int));

	xTaskCreate(printMessages, "PRINT", 1024*2, NULL, 1, NULL);
	xTaskCreate(sendMessages, "SEND", 1024*2, NULL, 10, NULL);
}