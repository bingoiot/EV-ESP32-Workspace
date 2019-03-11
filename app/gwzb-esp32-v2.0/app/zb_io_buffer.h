/*
 * zb_io_buffer.h
 *
 *  Created on: 2017��7��11��
 *      Author: lort
 */

#ifndef APP_ZB_IO_BUFFER_H_
#define APP_ZB_IO_BUFFER_H_

#define IO_SEND_TIMEOUT			(250)
#define IO_SEND_TRY				(3)

osState zb_io_init(void);
osState zb_io_req_send(uint8  cmd0, uint8 cmd1,uint16 addr, uint8 seq, uint8 *pdata, uint8 len, uint8 req_ack);
osState zb_io_remove_send(uint16 addr, uint8 seq, uint8 same_addr);

#endif /* APP_ZB_IO_BUFFER_H_ */
