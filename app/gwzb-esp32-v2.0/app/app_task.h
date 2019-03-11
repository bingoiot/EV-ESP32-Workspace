/*
 * app_task.h
 *
 *  Created on: 2017Äê5ÔÂ28ÈÕ
 *      Author: lort
 */

#ifndef APP_APP_TASK_H_
#define APP_APP_TASK_H_

#include "osCore.h"

typedef enum
{
	app_sig_keyscan = 1
}apptask_e;

void app_task_init(uint8 is_connect);
void app_task(int sig,void *arg,int len);

#endif /* APP_APP_TASK_H_ */
