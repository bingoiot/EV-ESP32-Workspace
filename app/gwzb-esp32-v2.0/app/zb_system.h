/*
 * zb_system.h
 *
 *  Created on: 2017��7��10��
 *      Author: lort
 */

#ifndef APP_ZB_SYSTEM_H_
#define APP_ZB_SYSTEM_H_

#include "osCore.h"

osState zb_factory_reset(void);
osState zb_reset(void);
osState zb_read_network_info(void);
osState zb_set_network(uint8 channel, uint16 panID, uint16 devAddr, uint8 *exAddr);


#endif /* APP_ZB_SYSTEM_H_ */
