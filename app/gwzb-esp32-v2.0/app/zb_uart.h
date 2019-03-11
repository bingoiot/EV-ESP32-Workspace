/*
 * zb_uart.h
 *
 *  Created on: 2017��7��10��
 *      Author: lort
 */

#ifndef APP_ZB_UART_H_
#define APP_ZB_UART_H_


/***************************************************************************************************
 * SYS COMMANDS
 ***************************************************************************************************/
/* AREQ from host */
#define MT_SYS_RESET_REQ                     0x00
/* SREQ/SRSP */
#define MT_SYS_PING                          0x01
#define MT_SYS_VERSION                       0x02
#define MT_SYS_SET_EXTADDR                   0x03
#define MT_SYS_GET_EXTADDR                   0x04
#define MT_SYS_RAM_READ                      0x05
#define MT_SYS_RAM_WRITE                     0x06
#define MT_SYS_OSAL_NV_ITEM_INIT             0x07
#define MT_SYS_OSAL_NV_READ                  0x08
#define MT_SYS_OSAL_NV_WRITE                 0x09
#define MT_SYS_OSAL_START_TIMER              0x0A
#define MT_SYS_OSAL_STOP_TIMER               0x0B
#define MT_SYS_RANDOM                        0x0C
#define MT_SYS_ADC_READ                      0x0D
#define MT_SYS_GPIO                          0x0E
#define MT_SYS_STACK_TUNE                    0x0F
#define MT_SYS_SET_TIME                      0x10
#define MT_SYS_GET_TIME                      0x11
#define MT_SYS_OSAL_NV_DELETE                0x12
#define MT_SYS_OSAL_NV_LENGTH                0x13
#define MT_SYS_SET_TX_POWER                  0x14

/* AREQ to host */
#define MT_SYS_RESET_IND                     0x80
#define MT_SYS_OSAL_TIMER_EXPIRED            0x81


/***************************************************************************************************
 * ZDO COMMANDS
 ***************************************************************************************************/

/* SREQ/SRSP */
#define MT_ZDO_NWK_ADDR_REQ                  0x00
#define MT_ZDO_IEEE_ADDR_REQ                 0x01
#define MT_ZDO_NODE_DESC_REQ                 0x02
#define MT_ZDO_POWER_DESC_REQ                0x03
#define MT_ZDO_SIMPLE_DESC_REQ               0x04
#define MT_ZDO_ACTIVE_EP_REQ                 0x05
#define MT_ZDO_MATCH_DESC_REQ                0x06
#define MT_ZDO_COMPLEX_DESC_REQ              0x07
#define MT_ZDO_USER_DESC_REQ                 0x08
#define MT_ZDO_END_DEV_ANNCE                 0x0A
#define MT_ZDO_USER_DESC_SET                 0x0B
#define MT_ZDO_SERVICE_DISC_REQ              0x0C
#define MT_ZDO_END_DEV_BIND_REQ              0x20
#define MT_ZDO_BIND_REQ                      0x21
#define MT_ZDO_UNBIND_REQ                    0x22

#define MT_ZDO_SET_LINK_KEY                  0x23
#define MT_ZDO_REMOVE_LINK_KEY               0x24
#define MT_ZDO_GET_LINK_KEY                  0x25
#define MT_ZDO_NWK_DISCOVERY_REQ             0x26
#define MT_ZDO_JOIN_REQ                      0x27

#define MT_ZDO_MGMT_NWKDISC_REQ              0x30
#define MT_ZDO_MGMT_LQI_REQ                  0x31
#define MT_ZDO_MGMT_RTG_REQ                  0x32
#define MT_ZDO_MGMT_BIND_REQ                 0x33
#define MT_ZDO_MGMT_LEAVE_REQ                0x34
#define MT_ZDO_MGMT_DIRECT_JOIN_REQ          0x35
#define MT_ZDO_MGMT_PERMIT_JOIN_REQ          0x36
#define MT_ZDO_MGMT_NWK_UPDATE_REQ           0x37

/* AREQ optional, but no AREQ response. */
#define MT_ZDO_MSG_CB_REGISTER               0x3E
#define MT_ZDO_MSG_CB_REMOVE                 0x3F
#define MT_ZDO_STARTUP_FROM_APP              0x40

/* AREQ from host */
#define MT_ZDO_AUTO_FIND_DESTINATION_REQ     0x41

/* AREQ to host */
#define MT_ZDO_AREQ_TO_HOST                  0x80 /* Mark the start of the ZDO CId AREQs to host. */
#define MT_ZDO_NWK_ADDR_RSP                	 0x80 // ((uint8)NWK_addr_req | 0x80)
#define MT_ZDO_IEEE_ADDR_RSP               	 0x81 // ((uint8)IEEE_addr_req | 0x80)
#define MT_ZDO_NODE_DESC_RSP               	 0x82  //((uint8)Node_Desc_req | 0x80)
#define MT_ZDO_POWER_DESC_RSP              	 0x83  //((uint8)Power_Desc_req | 0x80)
#define MT_ZDO_SIMPLE_DESC_RSP             	 0x84 // ((uint8)Simple_Desc_req | 0x80)
#define MT_ZDO_ACTIVE_EP_RSP              	 0x85 // ((uint8)Active_EP_req | 0x80)
#define MT_ZDO_MATCH_DESC_RSP              	 0x86  //((uint8)Match_Desc_req | 0x80)

#define MT_ZDO_COMPLEX_DESC_RSP              0x87
#define MT_ZDO_USER_DESC_RSP                 0x88
//                                        /* 0x92 */ ((uint8)Discovery_Cache_req | 0x80)
#define MT_ZDO_USER_DESC_CONF                0x89
#define MT_ZDO_SERVER_DISC_RSP               0x8A

#define MT_ZDO_END_DEVICE_BIND_RSP        /* 0xA0 */ ((uint8)End_Device_Bind_req | 0x80)
#define MT_ZDO_BIND_RSP                   /* 0xA1 */ ((uint8)Bind_req | 0x80)
#define MT_ZDO_UNBIND_RSP                 /* 0xA2 */ ((uint8)Unbind_req | 0x80)

#define MT_ZDO_MGMT_NWK_DISC_RSP          /* 0xB0 */ ((uint8)Mgmt_NWK_Disc_req | 0x80)
#define MT_ZDO_MGMT_LQI_RSP               /* 0xB1 */ ((uint8)Mgmt_Lqi_req | 0x80)
#define MT_ZDO_MGMT_RTG_RSP               /* 0xB2 */ ((uint8)Mgmt_Rtg_req | 0x80)
#define MT_ZDO_MGMT_BIND_RSP              /* 0xB3 */ ((uint8)Mgmt_Bind_req | 0x80)
#define MT_ZDO_MGMT_LEAVE_RSP             /* 0xB4 */ ((uint8)Mgmt_Leave_req | 0x80)
#define MT_ZDO_MGMT_DIRECT_JOIN_RSP       /* 0xB5 */ ((uint8)Mgmt_Direct_Join_req | 0x80)
#define MT_ZDO_MGMT_PERMIT_JOIN_RSP       /* 0xB6 */ ((uint8)Mgmt_Permit_Join_req | 0x80)
//                                        /* 0xB8 */ ((uint8)Mgmt_NWK_Update_req | 0x80)
#define MT_ZDO_STATE_CHANGE_IND              0xC0
#define MT_ZDO_END_DEVICE_ANNCE_IND          0xC1
#define MT_ZDO_MATCH_DESC_RSP_SENT           0xC2
#define MT_ZDO_STATUS_ERROR_RSP              0xC3
#define MT_ZDO_SRC_RTG_IND                   0xC4
#define MT_ZDO_BEACON_NOTIFY_IND             0xC5
#define MT_ZDO_JOIN_CNF                      0xC6
#define MT_ZDO_NWK_DISCOVERY_CNF             0xC7
#define MT_ZDO_CONCENTRATOR_IND_CB           0xC8
#define MT_ZDO_LEAVE_IND                     0xC9
#define MT_ZDO_TC_DEVICE_IND                 0xCA
#define MT_ZDO_PERMIT_JOIN_IND               0xCB
#define MT_ZDO_SET_REJOIN_PARAMS             0xCC
#define MT_ZDO_MSG_CB_INCOMING               0xFF
// Some arbitrarily chosen value for a default error status msg.
#define MtZdoDef_rsp                         0x0040


#define MT_RPC_SUBSYSTEM_MASK 				0x0F

/* SREQ/SRSP: */
#define MT_APP_MSG                           0x00
#define MT_APP_USER_TEST                     0x01
/* SRSP */
#define MT_APP_RSP                           0x80


typedef enum {
  MT_RPC_CMD_POLL = 0x00,
  MT_RPC_CMD_SREQ = 0x20,
  MT_RPC_CMD_AREQ = 0x40,
  MT_RPC_CMD_SRSP = 0x60,
 /* MT_RPC_CMD_RES4 = 0x80,
  MT_RPC_CMD_RES5 = 0xA0,
  MT_RPC_CMD_RES6 = 0xC0,
  MT_RPC_CMD_RES7 = 0xE0*/
} zb_cmd0_h;

typedef enum {
  MT_RPC_SYS_RES0,   /* Reserved. */
  MT_RPC_SYS_SYS,
  MT_RPC_SYS_MAC,
  MT_RPC_SYS_NWK,
  MT_RPC_SYS_AF,
  MT_RPC_SYS_ZDO,
  MT_RPC_SYS_SAPI,   /* Simple API. */
  MT_RPC_SYS_UTIL,
  MT_RPC_SYS_DBG,
  MT_RPC_SYS_APP,
  MT_RPC_SYS_OTA,
  MT_RPC_SYS_MAX     /* Maximum value, must be last */
  /* 10-32 available, not yet assigned. */
}zb_cmd0_l;

typedef enum
{
  MT_CmdAppReport = 0x81,
  MT_CmdAppWarm   = 0x82,
  MT_CmdAppRespone= 0x83,

  MT_CmdAppSendAfData  = 0x05,
  MT_CmdAppZCLCommand = 0x06,
  MT_CmdAppSendRSSIPing = 0x07,

  MT_CmdAppSendCmdOff = 0x10,
  MT_CmdAppSendCmdOn = 0x11,
  MT_CmdAppSendCmdToggle = 0x12,

  MT_CmdAppSendCmdUpOpen = 0x13,
  MT_CmdAppSendCmdDonwClose = 0x14,
  MT_CmdAppSendCmdStop = 0x15,

  MT_CmdAppSendLevelControlMoveToLevel = 0x1A,
  MT_CmdAppSendLevelControlMove = 0x1B,
  MT_CmdAppSendLevelControlStep = 0x1C,
  MT_CmdAppSendLevelControlStop = 0x1D,
  MT_CmdAppSendLevelControlMoveToLevelWithOnOff = 0x1E,
  MT_CmdAppSendLevelControlMoveWithOnOff = 0x1F,
  MT_CmdAppSendLevelControlStepWithOnOff = 0x20,
  MT_CmdAppSendLevelControlStopWithOnOff = 0x21,
  MT_CmdAppSendAlarmResetAll            = 0x22,
  MT_CmdAppSendAlarmGet                 = 0x23,
  MT_CmdAppSendAlarmResetLog            = 0x24,

  MT_CmdAppSend_IAS_WD_StartWarningCmd  = 0x30,
  MT_CmdAppSend_IAS_WD_SquawkCmd        = 0x31,

  MT_CmdAppSendDoorLockLockDoor         = 0x40,
  MT_CmdAppSendDoorLockUnlockDoor       = 0x41,
  MT_CmdAppSendDoorLockToggleDoor       = 0x42,
  MT_CmdAppSendDoorLockGetPINCode       = 0x43,
  MT_CmdAppSendDoorLockClearPINCode     = 0x44,
  MT_CmdAppSendDoorLockClearAllPINCodes = 0x45,
  MT_CmdAppSendDoorLockGetUserStatus    = 0x46,
  MT_CmdAppSendDoorLockGetWeekDaySchedule = 0x47,
  MT_CmdAppSendDoorLockClearWeekDaySchedule = 0x48,
  MT_CmdAppSendDoorLockGetYearDaySchedule = 0x49,
  MT_CmdAppSendDoorLockClearYearDaySchedule = 0x4A,
  MT_CmdAppSendDoorLockGetHolidaySchedule = 0x4B,
  MT_CmdAppSendDoorLockClearHolidaySchedule = 0x4C,
  MT_CmdAppSendDoorLockGetUserType      = 0x4D,
  MT_CmdAppSendDoorLockGetRFIDCode      = 0x4E,
  MT_CmdAppSendDoorLockClearRFIDCode    = 0x4F,
  MT_CmdAppSendDoorLockClearAllRFIDCodes = 0x50,

  MT_CmdAppSendGoToLiftValue            = 0x51,
  MT_CmdAppSendGoToLiftPercentage       = 0x52,
  MT_CmdAppSendGoToTiltValue            = 0x53,
  MT_CmdAppSendGoToTiltPercentage       = 0x54,

  MT_CmdAppColorControl_Send_MoveToHueCmd=0x60,
  MT_CmdAppColorControl_Send_MoveHueCmd=0x61,
  MT_CmdAppColorControl_Send_StepHueCmd=0x62,
  MT_CmdAppColorControl_Send_MoveToSaturationCmd=0x63,
  MT_CmdAppColorControl_Send_MoveSaturationCmd=0x64,
  MT_CmdAppColorControl_Send_StepSaturationCmd=0x65,
  MT_CmdAppColorControl_Send_MoveToHueAndSaturationCmd=0x66,
  MT_CmdAppColorControl_Send_MoveToColorCmd=0x67,
  MT_CmdAppColorControl_Send_MoveColorCmd=0x68,
  MT_CmdAppColorControl_Send_StepColorCmd=0x69,
  MT_CmdAppColorControl_Send_MoveToColorTemperatureCmd=0x6A,
  MT_CmdAppColorControl_Send_EnhancedMoveToHueCmd=0x6B,
  MT_CmdAppColorControl_Send_EnhancedMoveHueCmd=0x6C,
  MT_CmdAppColorControl_Send_EnhancedStepHueCmd=0x6D,
  MT_CmdAppColorControl_Send_EnhancedMoveToHueAndSaturationCmd=0x6E,
  MT_CmdAppColorControl_Send_StopMoveStepCmd=0x6F,
}zb_zcl_cmd_t;

typedef void (*zb_uart_receive_cb_t)(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);

osState zb_uart_init(zb_uart_receive_cb_t cb);
osState zb_uart_write(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);
uint8 	zb_uart_read(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);

#endif /* APP_ZB_UART_H_ */
