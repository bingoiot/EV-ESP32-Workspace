/*
 * zb_app.h
 *
 *  Created on: 2017��7��11��
 *      Author: lort
 */

#ifndef APP_ZB_APP_H_
#define APP_ZB_APP_H_

#include "osCore.h"
#include "pluto.h"

osState zb_app_init(void);
void pluto_recieve_message_cb(address_t *src,uint8 seq, command_t *cmd, uint8 *pdata,uint32 len);
void pluto_send_status_cb(address_t *src,uint8 seq, command_t *cmd, uint8 state);
void zb_app_receive_message(uint16 addr, uint8  ep,uint8 seq, uint16 cID, uint8 cmd, uint8 specific, uint8 *pdata, uint8 len);

#endif /* APP_ZB_APP_H_ */
