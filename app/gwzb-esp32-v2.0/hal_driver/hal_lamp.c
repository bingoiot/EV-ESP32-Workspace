/*
 * hal_lamp.c
 *
 *  Created on: 2016年11月10日
 *      Author: Administrator
 */

#include "osCore.h"
#include "hal_conf.h"
#include "hal_lamp.h"
#include "external_include.h"

#define GPIO_LAMP0_NUM    	19
#define GPIO_LAMP0_MASK		(1<<GPIO_LAMP0_NUM)

#define GPIO_LAMP1_NUM    	23
#define GPIO_LAMP1_MASK		(1<<GPIO_LAMP1_NUM)

#define GPIO_LAMP2_NUM    	5
#define GPIO_LAMP2_MASK		(1<<GPIO_LAMP2_NUM)

#define set_lamp0()		do{gpio_set_level(GPIO_LAMP0_NUM,LAMP_ON_LEVEL);lamp_state[0] = LAMP_ON_LEVEL;}while(0)
#define reset_lamp0()	do{gpio_set_level(GPIO_LAMP0_NUM,LAMP_OFF_LEVEL);lamp_state[0] = LAMP_OFF_LEVEL;}while(0)

#define set_lamp1()		do{gpio_set_level(GPIO_LAMP1_NUM,LAMP_ON_LEVEL);lamp_state[1] = LAMP_ON_LEVEL;}while(0)
#define reset_lamp1()	do{gpio_set_level(GPIO_LAMP1_NUM,LAMP_OFF_LEVEL);lamp_state[1] = LAMP_OFF_LEVEL;}while(0)

#define set_lamp2()		do{gpio_set_level(GPIO_LAMP2_NUM,LAMP_ON_LEVEL);lamp_state[2] = LAMP_ON_LEVEL;}while(0)
#define reset_lamp2()	do{gpio_set_level(GPIO_LAMP2_NUM,LAMP_OFF_LEVEL);lamp_state[2] = LAMP_OFF_LEVEL;}while(0)

static uint8 lamp_state[4];

void  hal_lamp_init(void)
{
	osLogI(DBG_HAL,"hal_led_init: \r\n");
    gpio_config_t io_conf;
    osMemset(&io_conf,0,sizeof(io_conf));
    //disable interrupt
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_LAMP0_MASK|GPIO_LAMP1_MASK|GPIO_LAMP2_MASK;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //disable pull-up mode
    io_conf.pull_up_en = 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
	hal_lamp_set(LAMP0|LAMP1|LAMP2,LAMP_OFF);
}

void  hal_lamp_set(uint8 lamps, uint8 state)
{
  osLogI(DBG_HAL,"hal_lamp_set:lamps:%02x val:%d \r\n",lamps,state);
  if(lamps&LAMP0)
  {
    if(state==LAMP_ON)
    	set_lamp0();
    else
    	reset_lamp0();
  }
  if(lamps&LAMP1)
  {
    if(state==LAMP_ON)
    	set_lamp1();
    else
    	reset_lamp1();
  }
  if(lamps&LAMP2)
  {
    if(state==LAMP_ON)
    	set_lamp2();
    else
    	reset_lamp2();
  }
}
void  hal_lamp_tolgle(uint8 lamps)
{
	if(lamps&LAMP0)
	{
		if(lamp_state[0]==LAMP_OFF_LEVEL)
		{
			set_lamp0();
		}
		else
		{
			reset_lamp0();
		}
	}
	if(lamps&LAMP1)
	{
		if(lamp_state[1]==LAMP_OFF_LEVEL)
		{
			set_lamp1();
		}
		else
		{
			reset_lamp1();
		}
	}
	if(lamps&LAMP2)
	{
		if(lamp_state[2]==LAMP_OFF_LEVEL)
		{
			set_lamp2();
		}
		else
		{
			reset_lamp2();
		}
	}
}
uint8  hal_lamp_get(uint8 lamps)
{
	osLogI(DBG_HAL,"hal_lamp_get:lamps:%02x \r\n",lamps);
  if(lamps&LAMP0)
  {
	if(lamp_state[0]==LAMP_ON_LEVEL)
    {
      return LAMP_ON;
    }
    else
    {
       return LAMP_OFF;
    }
  }
  else if(lamps&LAMP1)
  {
	if(lamp_state[1]==LAMP_ON_LEVEL)
    {
      return LAMP_ON;
    }
    else
    {
       return LAMP_OFF;
    }
  }
  else if(lamps&LAMP2)
  {
	if(lamp_state[2]==LAMP_ON_LEVEL)
    {
      return LAMP_ON;
    }
    else
    {
       return LAMP_OFF;
    }
  }
  return LAMP_OFF;
}
