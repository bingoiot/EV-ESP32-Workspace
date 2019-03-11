/*
 * hal_uart.h
 *
 *  Created on: 2017Äê7ÔÂ24ÈÕ
 *      Author: lort
 */

#ifndef HAL_DRIVER_HAL_UART_H_
#define HAL_DRIVER_HAL_UART_H_

#include "osCore.h"

void 	hal_uart_init(void);
void 	hal_uart_set_baud(uint32 b);
uint16 	hal_uart_read(uint8 *pbuf, uint16 len,uint16 wait);
uint16 	hal_uart_write(uint8 *pdata, uint16 len);

#endif /* HAL_DRIVER_HAL_UART_H_ */
