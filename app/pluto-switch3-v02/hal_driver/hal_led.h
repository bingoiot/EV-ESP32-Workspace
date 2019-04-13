/*
 * Hal_LED.h
 *
 *  Created on: 2014-8-29
 *      Author: Administrator
 */

#include "osCore.h"

#ifndef HAL_LED_H_
#define HAL_LED_H_

#define LED_STATUS_ON	0x00
#define LED_STATUS_OFF	0x01

#define DEFAULT_BLINK_TIME_H	(uint16)100
#define DEFAULT_BLINK_TIME_L	(uint16)100

struct Hal_led_t
{
  uint32 blinkNum;
  uint8 	status;
  uint8 	lastStat;
  uint32 blinkTimeH;
  uint32 blinkTimeL;
  uint32 runtime;
};

extern struct Hal_led_t Led;


void hal_led_init(void);
void hal_led_set(uint8 val);
void hal_led_blink(int blinkNum, int blinkTimeH,int blinkTimeL);
void hal_led_poll(void);
#endif /* HAL_LED_H_ */
