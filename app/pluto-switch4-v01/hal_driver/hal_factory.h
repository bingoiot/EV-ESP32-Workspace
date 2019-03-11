/*
 * hal_factory.h
 *
 *  Created on: Feb 26, 2019
 *      Author: lort
 */

#ifndef HAL_DRIVER_HAL_FACTORY_H_
#define HAL_DRIVER_HAL_FACTORY_H_

typedef enum {
  FA_RPC_CMD_POLL = 0x00,
  FA_RPC_CMD_SREQ = 0x20,
  FA_RPC_CMD_AREQ = 0x40,
  FA_RPC_CMD_SRSP = 0x60,
} mt_cmd0_h;

typedef enum {
  FA_RPC_SYS_RES0,   /* Reserved. */
  FA_RPC_SYS_SYS,
  FA_RPC_SYS_MAC,
  FA_RPC_SYS_NWK,
  FA_RPC_SYS_APS,
  FA_RPC_SYS_PDO,
  FA_RPC_SYS_SAPI,   /* Simple API. */
  FA_RPC_SYS_UTIL,
  FA_RPC_SYS_DBG,
  FA_RPC_SYS_APP,
  FA_RPC_SYS_OTA,
  FA_RPC_SYS_MAX     /* Maximum value, must be last */
  /* 10-32 available, not yet assigned. */
}mt_cmd0_l;

typedef enum
{
	FA_SYS_TEST =1,
	FA_SYS_RESET,
	FA_SYS_READ_INFO,
	FA_SYS_READ_SN,
	FA_SYS_WRITE_SN
}mt_sys_cmd_t;

typedef void (*factory_receive_cb_t)(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);

extern void hal_factory_init(factory_receive_cb_t cb);
extern void hal_factory_poll(void);
extern osState hal_factory_write(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);

#endif /* APP_GWZB_ESP32_V2_0_HAL_DRIVER_HAL_FACTORY_H_ */
