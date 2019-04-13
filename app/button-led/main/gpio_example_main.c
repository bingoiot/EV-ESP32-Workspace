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

#include "hal_key.h"
#include "hal_lamp.h"

void app_main()
{
	unsigned char key;//button value.
	unsigned char shift;//press state
	hal_key_init();
	hal_lamp_init();

    while(1) {
    	shift = hal_get_key(&key,20);
    	if(shift!=0)
    	{
    		if(shift==HAL_KEY_DOWN)//button press down
    		{
    			switch(key)
    			{
    			case KEY0:
    				hal_lamp_tolgle(LAMP0);
    				break;
    			case KEY1:
    				hal_lamp_tolgle(LAMP1);
    				break;
    			case KEY2:
    				hal_lamp_tolgle(LAMP2);
    				break;
    			case KEY3:
    				hal_lamp_tolgle(LAMP3);
    				break;
    			}
    		}
    		else if(shift==HAL_KEY_KEEP)//butto hold up
    		{
    			printf("key press key:%02x keep time:%d \r\n",key,hal_key_keeptime());
    		}
    	}
        vTaskDelay(20 / portTICK_RATE_MS);
    }
}

