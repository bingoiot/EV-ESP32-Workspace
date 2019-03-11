/*
 * zb_zdo.h
 *
 *  Created on: 2017��7��11��
 *      Author: lort
 */

#ifndef APP_ZB_ZDO_H_
#define APP_ZB_ZDO_H_

#include "osCore.h"

osState zb_permite_join(uint16 addr,uint8 duration, uint8 significance);
osState zb_req_read_associate(uint16 addr,uint8 startID);
osState zb_req_remove_device(uint16 addr, uint8 *exAddr);
void zb_zdo_message_process(uint8 cmd1,uint8 *pdata, uint8 len);
void zdo_add_new_device(uint16 addr, uint8 *exAddr);

#endif /* APP_ZB_ZDO_H_ */
