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
static xQueueHandle myQueue = NULL;

static void my_first_task(void* arg)
{
	char buf[100];
	int count = 0;
    while(1)
    {
    	sprintf(buf,"send a message of counter to second task:%d \r\n",count++);
    	xQueueSend(myQueue,buf,portMAX_DELAY);
    	vTaskDelay(2000 / portTICK_RATE_MS);
    }
}
static void my_second_task(void* arg)
{
	char buf[100];
	int temp;
    while(1)
    {
    	vTaskDelay(1000 / portTICK_RATE_MS);
        if(xQueueReceive(myQueue, buf, portMAX_DELAY)) {
            printf("recieve message from first task :%s \n", buf);
        }
    }
}
void app_main()
{
	myQueue = xQueueCreate(10, sizeof(char)*100);//100 byte size and 10 deep just like "char array[10][100]"

    xTaskCreate(my_first_task, "my_first_task", 2048, NULL, 10, NULL);
    xTaskCreate(my_second_task, "my_second_task", 2048, NULL, 10, NULL);
    int cnt = 0;
    while(1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

