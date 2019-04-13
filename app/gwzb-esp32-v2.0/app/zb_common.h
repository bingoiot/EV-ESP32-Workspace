/*
 * zb_common.h
 *
 *  Created on: 2017��7��10��
 *      Author: lort
 */

#ifndef APP_ZB_COMMON_H_
#define APP_ZB_COMMON_H_

#include "external_include.h"

#define DBG_MT_ERROR	1
#define DBG_MT_UART		0
#define DBG_MT_TASK		0
#define DBG_MT_ZDO		0
#define DBG_MT_SYS 		0
#define DBG_MT_APP 		1
#define DBG_MT_DEV 		0
#define DBG_MT_COMMON	1
#define DBG_MT_LQI 		0

#define Mgmt_NWK_Disc_req       ((uint16)0x0030)
#define Mgmt_Lqi_req            ((uint16)0x0031)
#define Mgmt_Rtg_req            ((uint16)0x0032)
#define Mgmt_Bind_req           ((uint16)0x0033)
#define Mgmt_Leave_req          ((uint16)0x0034)
#define Mgmt_Direct_Join_req    ((uint16)0x0035)
#define Mgmt_Permit_Join_req    ((uint16)0x0036)
#define Mgmt_NWK_Update_req     ((uint16)0x0038)
#define Mgmt_NWK_Disc_rsp       (Mgmt_NWK_Disc_req | ZDO_RESPONSE_BIT)
#define Mgmt_Lqi_rsp            (Mgmt_Lqi_req | ZDO_RESPONSE_BIT)
#define Mgmt_Rtg_rsp            (Mgmt_Rtg_req | ZDO_RESPONSE_BIT)
#define Mgmt_Bind_rsp           (Mgmt_Bind_req | ZDO_RESPONSE_BIT)
#define Mgmt_Leave_rsp          (Mgmt_Leave_req | ZDO_RESPONSE_BIT)
#define Mgmt_Direct_Join_rsp    (Mgmt_Direct_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_Permit_Join_rsp    (Mgmt_Permit_Join_req | ZDO_RESPONSE_BIT)
#define Mgmt_NWK_Update_notify  (Mgmt_NWK_Update_req | ZDO_RESPONSE_BIT)

uint8  	zb_get_seq(void);
uint16 	zb_btou16(uint8 *pdata);
void 	zb_u16tob(uint8 *pdata, uint16 val);
uint32 	zb_btou32(uint8 *pdata);
void 	zb_u32tob(uint8 *pdata, uint32 val);


#endif /* APP_ZB_COMMON_H_ */
