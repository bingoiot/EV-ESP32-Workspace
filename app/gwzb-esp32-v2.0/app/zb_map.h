/*
 * zb_map.h
 *
 *  Created on: 2017��7��12��
 *      Author: lort
 */

#ifndef APP_ZB_MAP_H_
#define APP_ZB_MAP_H_

#include "osCore.h"

osState 	zb_create_port(uint8 *exaddr, uint16 addr, uint8 ep, uint16 device_id, uint16 *cid, uint8 cid_num);
uint8 		zb_check_addr_exist(uint16 addr);
uint8 		zb_check_exaddr_exist(uint8* exaddr);
uint8 		zb_delete_device(uint16 addr);
uint8 		zb_delete_device_by_exaddr(uint8 *exaddr);
uint8 		zb_get_port(uint16 addr, uint8 ep);
uint16 		zb_get_addr(uint8 port);
uint8 		zb_get_ep(uint8 port);
osState 	zb_save_lqi(uint16 addr, uint8 lqi);
uint8 		zb_get_port_lqi(uint8 port);

uint16 		zb_attributeID_to_clusteID(uint32 aID);
uint32 		zb_clusteID_to_attributeID(uint16 cID);
uint32 		zb_deviceID_to_applicationID(uint16 device_id);
uint16 		zb_applicationID_to_deviceID(uint32 appID);

#endif /* APP_ZB_MAP_H_ */
