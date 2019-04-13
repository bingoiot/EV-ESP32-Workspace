/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

static void my_first_task(void* arg);
static void my_second_task(void* arg);
static int global_value = 0;
static portMUX_TYPE myMutex;
static void my_first_task(void* arg)
{
	int count = 0;
    while(1)
    {
    	printf("set global value to :%d \r\n",count);
    	count++;
    	portENTER_CRITICAL(&myMutex);
    	global_value = count;
    	portEXIT_CRITICAL(&myMutex);
    	vTaskDelay(2000 / portTICK_RATE_MS);
    }
}
static void my_second_task(void* arg)
{
	int temp;
    while(1)
    {
    	vTaskDelay(1000 / portTICK_RATE_MS);
    	portENTER_CRITICAL(&myMutex);
    	temp = global_value;
    	portEXIT_CRITICAL(&myMutex);
    	printf("check global value:%d \r\n",temp);
    }
}
void app_main()
{
	vPortCPUInitializeMutex(&myMutex);
    xTaskCreate(my_first_task, "my_first_task", 2048, NULL, 10, NULL);
    xTaskCreate(my_second_task, "my_second_task", 2048, NULL, 10, NULL);
    int cnt = 0;
    while(1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

