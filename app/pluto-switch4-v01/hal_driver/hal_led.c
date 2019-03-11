/*
 * Hal_LED.c
 *
 *  Created on: 2014-8-29
 *      Author: Administrator
 */

#include "osCore.h"
#include "hal_conf.h"
#include "hal_led.h"
#include "external_include.h"

struct Hal_led_t Led;

#define GPIO_LED_NUM    18//23
#define GPIO_LED_MASK	(1<<GPIO_LED_NUM)

void hal_led_task(int sig, void *param,int len);

void FUC_ATTR hal_led_init(void)
{
	osLogI(DBG_HAL,"hal_led_init: \r\n");
    gpio_config_t io_conf;
    osMemset(&io_conf,0,sizeof(io_conf));
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_LED_MASK;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

	hal_led_set(LED_STATUS_OFF);
}
void FUC_ATTR hal_led_set(uint8 val)
{
	osLogI(DBG_HAL,"hal_led_set: state:%d  \r\n",val);
	Led.lastStat = val;
	gpio_set_level(GPIO_LED_NUM,val);
}
void FUC_ATTR hal_led_blink(int blinkNum, int blinkTimeH, int blinkTimeL)
{
	osLogI(DBG_HAL,"hal_led_blink:num:%d h:%d l:%d \r\n",blinkNum,blinkTimeH,blinkTimeL);
	Led.status = LED_STATUS_OFF;
	Led.blinkNum=blinkNum;
	if(blinkTimeH==0)
		blinkTimeH=DEFAULT_BLINK_TIME_H;
	if(blinkTimeL==0)
		blinkTimeL=DEFAULT_BLINK_TIME_L;
	Led.blinkTimeH=(blinkTimeH);
	Led.blinkTimeL=(blinkTimeL);
	if(Led.blinkNum)
	{
		if(Led.status==LED_STATUS_OFF)
			osStartTimer(Led.blinkTimeL,hal_led_task,1,NULL);
		else
			osStartTimer(Led.blinkTimeH,hal_led_task,1,NULL);
	}
	else
		osSendMessage(hal_led_task,1,NULL,0);
}
void FUC_ATTR hal_led_task(int sig, void *param,int len)
{
	if(Led.blinkNum>0)
	{
		if(Led.status==LED_STATUS_OFF)
		{
			osLogI(DBG_HAL,"hal_led_task: high num:%d \r\n",Led.blinkNum);
			gpio_set_level(GPIO_LED_NUM,LED_STATUS_ON);
			osStartTimer(Led.blinkTimeH,hal_led_task,1,NULL);
		}
		else
		{
			osLogI(DBG_HAL,"hal_led_task: low num:%d \r\n",Led.blinkNum);
			gpio_set_level(GPIO_LED_NUM,LED_STATUS_OFF);
			Led.blinkNum--;
			osStartTimer(Led.blinkTimeL,hal_led_task,1,NULL);
		}
		Led.status = !Led.status;
	}
	else
		gpio_set_level(GPIO_LED_NUM,Led.lastStat);
}
