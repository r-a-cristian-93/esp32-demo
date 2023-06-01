#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "esp_log.h"

SemaphoreHandle_t counting_semaphore;

void printSymbol(void* param)
{
	char symbol = '0';
	
	while(1) 
	{
		bool print_nl = false;

		// Take all semaphores each 1000 ms if any
		ESP_LOGI("PRINT", "Semaphores accumulated: %u.", uxSemaphoreGetCount(counting_semaphore));
		while (xSemaphoreTake(counting_semaphore, 0))
		{
			print_nl = true;
			printf("%c", symbol);
		}
		if (print_nl) 
		{
			printf("\n");
		}

		vTaskDelay(pdMS_TO_TICKS(3000));
	}
	vTaskDelete(NULL);
}

void readCommand(void* param)
{
	const uart_port_t uart_port = UART_NUM_0;
	const int pin_tx = 4;
	const int pin_rx = 5;
	const int pin_rts = 18;
	const int pin_cts = 19;
	const int buffer_size_rx = (1024 * 2);
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
		.rx_flow_ctrl_thresh = 122,
	};

	ESP_ERROR_CHECK(uart_driver_install(uart_port, buffer_size_rx, 0, 0, NULL, 0));
	ESP_ERROR_CHECK(uart_param_config(uart_port, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(uart_port, pin_rx, pin_tx, pin_rts, pin_cts));

	// Read one character from UART.
	uint32_t data_len = 1;
	unsigned char data;
	int len = 0;
	
	while (1)
	{
		len = uart_read_bytes(uart_port, &data, data_len, 0);
		if (len) {
			if (data == 'X')
			{
				if (xSemaphoreGive(counting_semaphore) == pdTRUE) 
				{
					ESP_LOGI("READ", "Added one semaphore. Total: %u.", uxSemaphoreGetCount(counting_semaphore));
				}
			}
			else
			{
				ESP_LOGW("READ", "Unknown command: %c received.", data);
			}
		}
		vTaskDelay(10);
	}
	vTaskDelete(NULL);
}


void app_main(void)
{
	counting_semaphore = xSemaphoreCreateCounting(5, 0);

	xTaskCreate(printSymbol, "PRINT", 1024*2, NULL, 1, NULL);
	xTaskCreate(readCommand, "READ", 1024*2, NULL, 10, NULL);
}
