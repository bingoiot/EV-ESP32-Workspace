/*
 * zcl_closures.c
 *
 *  Created on: Sep 14, 2017
 *      Author: lort
 */
#include "osCore.h"
#include "zb_zcl.h"
#include "zcl_closures.h"
#include "zb_common.h"

#ifdef ZCL_DOORLOCK
/*********************************************************************
 * @fn      zclClosures_SendDoorLockRequest
 *
 * @brief   Call to send out a Door Lock Lock/Unlock/Toggle Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - COMMAND_CLOSURES_LOCK_DOOR, COMMAND_CLOSURES_UNLOCK_DOOR, COMMAND_CLOSURES_TOGGLE_DOOR
 * @param   pPayload:
 *           aPinRfidCode - PIN/RFID code in ZCL Octet String Format
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */

osState zclClosures_SendDoorLockRequest(uint8 srcEP,  uint16 dstAddr, uint8 cmd,
                                           zclDoorLock_t *pPayload,
                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 calculatedBufSize;
  osState status;

  // first octet of PIN/RFID Code variable string identifies its length
  calculatedBufSize = pPayload->pPinRfidCode[0] + 1;  // add first byte of string

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  for ( i = 0; i < calculatedBufSize; i++ )
  {
    pBuf[i] = pPayload->pPinRfidCode[i];
  }

  status = zcl_SendCommand( dstAddr,srcEP, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockUnlockTimeoutRequest
 *
 * @brief   Call to send out a Unlock with Timeout Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           timeout - The timeout in seconds
 *           aPinRfidCode - PIN/RFID code in ZCL Octet String Format
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockUnlockTimeoutRequest( uint8 srcEP, uint16 dstAddr,
                                                        zclDoorLockUnlockTimeout_t *pPayload,
                                                        uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint8 calculatedBufSize;
  osState status;

  // first octet of PIN/RFID Code variable string identifies its length
  calculatedArrayLen = pPayload->pPinRfidCode[0] + 1;  // add first byte of string

  // determine total size of buffer
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_UNLOCK_TIMEOUT;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  zb_u16tob(&pBuf[0],pPayload->timeout);
  offset = 2;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pPinRfidCode[i];
  }

  status = zcl_SendCommand( dstAddr,srcEP, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_UNLOCK_WITH_TIMEOUT, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetLogRecordRequest
 *
 * @brief   Call to send out a Get Log Record Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   logIndex - Log number between 1 - [max log attribute]
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetLogRecordRequest( uint8 srcEP, uint16 dstAddr,
                                                       uint16 logIndex, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_GET_LOG_RECORD];
  zb_u16tob(&payload[0],logIndex);
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_GET_LOG_RECORD, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_GET_LOG_RECORD, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockSetPINCodeRequest
 *
 * @brief   Call to send out a Set PIN Code Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           userID - User ID is between 0 - [# PINs User supported attribute]
 *           userStatus - Used to indicate what the status is for a specific User ID
 *           userType - Used to indicate what the type is for a specific User ID
 *           pPIN - A ZigBee string indicating the PIN code used to create the event on the door lock
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockSetPINCodeRequest( uint8 srcEP, uint16 dstAddr,
                                                     zclDoorLockSetPINCode_t *pPayload,
                                                     uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint8 calculatedBufSize;
  osState status;

  // first octet of PIN/RFID Code variable string identifies its length
  calculatedArrayLen = pPayload->pPIN[0] + 1;  // add first byte of string

  // determine the total buffer size
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_SET_PIN_CODE;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  zb_u16tob(&pBuf[0],pPayload->userID);
  pBuf[2] = pPayload->userStatus;
  pBuf[3] = pPayload->userType;
  offset = 4;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pPIN[i];
  }


  status = zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_SET_PIN_CODE, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockUserIDRequest
 *
 * @brief   Call to send out a User ID Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - COMMAND_CLOSURES_GET_PIN_CODE, COMMAND_CLOSURES_CLEAR_PIN_CODE,
 *                COMMAND_CLOSURES_GET_USER_STATUS, COMMAND_CLOSURES_GET_USER_TYPE,
 *                COMMAND_CLOSURES_GET_RFID_CODE, COMMAND_CLOSURES_CLEAR_RFID_CODE
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockUserIDRequest( uint8 srcEP, uint16 dstAddr, uint8 cmd,
                                                 uint16 userID, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_USER_ID];
  zb_u16tob(&payload[0],userID);
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_USER_ID, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockClearAllCodesRequest
 *
 * @brief   Call to send out a Clear All Codes Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - COMMAND_CLOSURES_CLEAR_ALL_PIN_CODES, COMMAND_CLOSURES_CLEAR_ALL_RFID_CODES
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockClearAllCodesRequest( uint8 srcEP, uint16 dstAddr, uint8 cmd,
                                                        uint8 disableDefaultRsp, uint8 seqNum )
{
  // no payload

  return zcl_SendCommand(  dstAddr, srcEP,ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 0, NULL );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockSetUserStatusRequest
 *
 * @brief   Call to send out a Set User Status Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   userStatus - Used to indicate what the status is for a specific User ID
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockSetUserStatusRequest( uint8 srcEP, uint16 dstAddr,
                                                        uint16 userID, uint8 userStatus,
                                                        uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_SET_USER_STATUS];
  zb_u16tob(payload,userID);
  payload[2] = userStatus;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_SET_USER_STATUS, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_SET_USER_STATUS, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockSetWeekDayScheduleRequest
 *
 * @brief   Call to send out a Set Week Day Schedule Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   scheduleID - The Schedule ID # is between 0 - [# Schedule IDs per user attribute]
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   daysMask - Bitmask of the effective days in the order XSFTWTMS
 * @param   startHour - The start hour of the Week Day Schedule: 0-23
 * @param   startMinute - The start minute of the Week Day Schedule: 0-59
 * @param   endHour - The end hour of the Week Day Schedule: 0-23
 * @param   endMinute - The end minute of the Week Day Schedule: 0-59
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockSetWeekDayScheduleRequest( uint8 srcEP, uint16 dstAddr,
                                                             uint8 scheduleID, uint16 userID,
                                                             uint8 daysMask, uint8 startHour,
                                                             uint8 startMinute, uint8 endHour,
                                                             uint8 endMinute, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_SET_WEEK_DAY_SCHEDULE];

  payload[0] = scheduleID;
  zb_u16tob(&payload[1],userID);
  payload[3] = daysMask;
  payload[4] = startHour;
  payload[5] = startMinute;
  payload[6] = endHour;
  payload[7] = endMinute;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_SET_WEEK_DAY_SCHEDULE, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_SET_WEEK_DAY_SCHEDULE, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockScheduleRequest
 *
 * @brief   Call to send out a Schedule Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - COMMAND_CLOSURES_GET_WEEK_DAY_SCHEDULE, COMMAND_CLOSURES_CLEAR_WEEK_DAY_SCHEDULE,
 *                COMMAND_CLOSURES_GET_YEAR_DAY_SCHEDULE, COMMAND_CLOSURES_CLEAR_YEAR_DAY_SCHEDULE
 * @param   scheduleID - The Schedule ID # is between 0 - [# Schedule IDs per user attribute]
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockScheduleRequest( uint8 srcEP, uint16 dstAddr, uint8 cmd,
                                                   uint8 scheduleID, uint16 userID,
                                                   uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_SCHEDULE];

  payload[0] = scheduleID;
  zb_u16tob(&payload[1],userID);
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_SCHEDULE, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockSetYearDayScheduleRequest
 *
 * @brief   Call to send out a Set Year Day Schedule Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   scheduleID - The Schedule ID # is between 0 - [# Schedule IDs per user attribute]
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   zigBeeLocalStartTime - Start time of the Year Day Schedule representing by ZigBeeLocalTime
 * @param   zigBeeLocalEndTime - End time of the Year Day Schedule representing by ZigBeeLocalTime
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockSetYearDayScheduleRequest( uint8 srcEP, uint16 dstAddr,
                                                             uint8 scheduleID, uint16 userID,
                                                             uint32 zigBeeLocalStartTime,
                                                             uint32 zigBeeLocalEndTime,
                                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_SET_YEAR_DAY_SCHEDULE];

  payload[0] = scheduleID;
  zb_u16tob(&payload[1],userID);
  zb_u32tob(&payload[3],zigBeeLocalStartTime);
  zb_u32tob(&payload[7],zigBeeLocalEndTime);
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_SET_YEAR_DAY_SCHEDULE, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_SET_YEAR_DAY_SCHEDULE, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockSetHolidayScheduleRequest
 *
 * @brief   Call to send out a Set Holiday Schedule Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   holidayScheduleID - A unique ID for given Holiday Schedule (0 to 254)
 * @param   zigBeeLocalStartTime - Start time of the Year Day Schedule representing by ZigBeeLocalTime
 * @param   zigBeeLocalEndTime - End time of the Year Day Schedule representing by ZigBeeLocalTime
 * @param   operatingModeDuringHoliday - A valid enumeration value as listed in operating mode attribute
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockSetHolidayScheduleRequest( uint8 srcEP, uint16 dstAddr,
                                                             uint8 holidayScheduleID,
                                                             uint32 zigBeeLocalStartTime,
                                                             uint32 zigBeeLocalEndTime,
                                                             uint8 operatingModeDuringHoliday,
                                                             uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_SET_HOLIDAY_SCHEDULE];

  payload[0] = holidayScheduleID;
  zb_u32tob(&payload[1],zigBeeLocalStartTime);
  zb_u32tob(&payload[5],zigBeeLocalEndTime);
  payload[9] = operatingModeDuringHoliday;
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_SET_HOLIDAY_SCHEDULE, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_SET_HOLIDAY_SCHEDULE, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockHolidayScheduleRequest
 *
 * @brief   Call to send out a Holiday Schedule Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - COMMAND_CLOSURES_GET_HOLIDAY_SCHEDULE, COMMAND_CLOSURES_CLEAR_HOLIDAY_SCHEDULE
 * @param   holidayScheduleID - A unique ID for given Holiday Schedule (0 to 254)
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockHolidayScheduleRequest( uint8 srcEP, uint16 dstAddr, uint8 cmd,
                                                          uint8 holidayScheduleID,
                                                          uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_HOLIDAY_SCHEDULE];

  payload[0] = holidayScheduleID;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_HOLIDAY_SCHEDULE, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockSetUserTypeRequest
 *
 * @brief   Call to send out a Set User Type Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   userType - Used to indicate what the type is for a specific User ID
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockSetUserTypeRequest( uint8 srcEP, uint16 dstAddr,
                                                      uint16 userID, uint8 userType,
                                                      uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_SET_USER_TYPE];
  zb_u16tob(&payload[0],userID);
  payload[2] = userType;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_SET_USER_TYPE, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_SET_USER_TYPE, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockSetRFIDCodeRequest
 *
 * @brief   Call to send out a Set RFID Code Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           userID - User ID is between 0 - [# PINs User supported attribute]
 *           userStatus - Used to indicate what the status is for a specific User ID
 *           userType - Used to indicate what the type is for a specific User ID
 *           aRfidCode - A ZigBee string indicating the RFID code used to create the event
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockSetRFIDCodeRequest( uint8 srcEP, uint16 dstAddr,
                                                      zclDoorLockSetRFIDCode_t *pPayload,
                                                      uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint8 calculatedBufSize;
  osState status;

  // first octet of PIN/RFID Code variable string identifies its length
  calculatedArrayLen = pPayload->pRfidCode[0] + 1;   // add first byte of string

  // determine the total buffer size
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_SET_RFID_CODE;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  zb_u16tob(&pBuf[0],pPayload->userID);
  pBuf[2] = pPayload->userStatus;
  pBuf[3] = pPayload->userType;
  offset = 4;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pRfidCode[i];
  }

  status = zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_SET_RFID_CODE, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockStatusResponse
 *
 * @brief   Call to send out a Status Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - COMMAND_CLOSURES_LOCK_DOOR_RSP, COMMAND_CLOSURES_UNLOCK_DOOR_RSP
 *                COMMAND_CLOSURES_TOGGLE_RSP, COMMAND_CLOSURES_UNLOCK_WITH_TIMEOUT_RSP,
 *                COMMAND_CLOSURES_SET_PIN_CODE_RSP, COMMAND_CLOSURES_CLEAR_PIN_CODE_RSP,
 *                COMMAND_CLOSURES_CLEAR_ALL_PIN_CODES_RSP, COMMAND_CLOSURES_SET_USER_STATUS_RSP,
 *                COMMAND_CLOSURES_SET_WEEK_DAY_SCHEDULE_RSP, COMMAND_CLOSURES_CLEAR_WEEK_DAY_SCHEDULE_RSP,
 *                COMMAND_CLOSURES_SET_YEAR_DAY_SCHEDULE_RSP, COMMAND_CLOSURES_CLEAR_YEAR_DAY_SCHEDULE_RSP,
 *                COMMAND_CLOSURES_SET_HOLIDAY_SCHEDULE_RSP, COMMAND_CLOSURES_CLEAR_HOLIDAY_SCHEDULE_RSP,
 *                COMMAND_CLOSURES_SET_USER_TYPE_RSP, COMMAND_CLOSURES_SET_RFID_CODE_RSP,
 *                COMMAND_CLOSURES_CLEAR_RFID_CODE_RSP, COMMAND_CLOSURES_CLEAR_ALL_RFID_CODES_RSP
 * @param   status - Returns the state due to the requesting command
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockStatusResponse( uint8 srcEP, uint16 dstAddr,uint8 cmd,
                                                  uint8 status, uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[1];   // 1 byte payload

  payload[0] = status;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          cmd, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                          disableDefaultRsp, 0, seqNum, sizeof( payload ), payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetLogRecordResponse
 *
 * @brief   Call to send out a Get Log Record Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           logEntryID - The index into the log table where this log entry is stored
 *           timestamp - A ZigBeeLocalTime used to timestamp all events and alarms on the door lock
 *           eventType - Indicates the type of event that took place on the door lock
 *           source - A source value of available sources
 *           eventIDAlarmCode - A one byte value indicating the type of event that took place on the door lock
 *           userID - User ID is between 0 - [# PINs User supported attribute]
 *           aPIN - A ZigBee string indicating the PIN code used to create the event on the door lock
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetLogRecordResponse( uint8 srcEP, uint16 dstAddr,
                                                        zclDoorLockGetLogRecordRsp_t *pPayload,
                                                        uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint8 calculatedBufSize;
  osState status;

  // first octet of PIN/RFID Code variable string identifies its length
  calculatedArrayLen = pPayload->pPIN[0] + 1;   // add first byte of string

  // determine the total buffer size
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_GET_LOG_RECORD_RSP;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  zb_u16tob(&pBuf[0],pPayload->logEntryID);
  zb_u32tob(&pBuf[2],pPayload->timestamp);
  pBuf[6] = pPayload->eventType;
  pBuf[7] = pPayload->source;
  pBuf[8] = pPayload->eventIDAlarmCode;
  zb_u16tob(&pBuf[9],pPayload->userID);
  offset = 11;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pPIN[i];
  }

  status = zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_GET_LOG_RECORD_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetPINCodeResponse
 *
 * @brief   Call to send out a Get PIN Code Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           userID - User ID is between 0 - [# PINs User supported attribute]
 *           userStatus - Used to indicate what the status is for a specific User ID
 *           userType - Used to indicate what the type is for a specific User ID
 *           aCode - Returned PIN number
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetPINCodeResponse( uint8 srcEP, uint16 dstAddr,
                                                      zclDoorLockGetPINCodeRsp_t *pPayload,
                                                      uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint8 calculatedBufSize;
  osState status;

  // first octet of PIN/RFID Code variable string identifies its length
  calculatedArrayLen = pPayload->pCode[0] + 1;   // add first byte of string

  // determine the total buffer size
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_GET_PIN_CODE_RSP;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  zb_u16tob(&pBuf[0],pPayload->userID);
  pBuf[2] = pPayload->userStatus;
  pBuf[3] = pPayload->userType;
  offset = 4;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pCode[i];
  }

  status = zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_GET_PIN_CODE_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetUserStatusResponse
 *
 * @brief   Call to send out a Get User Status Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   userStatus - Used to indicate what the status is for a specific User ID
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetUserStatusResponse( uint8 srcEP, uint16 dstAddr,
                                                         uint16 userID, uint8 userStatus,
                                                         uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_GET_USER_STATUS_RSP];
  zb_u16tob(&payload[0],userID);
  payload[2] = userStatus;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_GET_USER_STATUS_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_GET_USER_STATUS_RSP, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetWeekDayScheduleResponse
 *
 * @brief   Call to send out a Get Week Day Schedule Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pCmd:
 *               scheduleID - The Schedule ID # is between 0 - [# Schedule IDs per user attribute]
 *               userID - User ID is between 0 - [# PINs User supported attribute]
 *               status - Returns the state due to the requesting command
 *               daysMask - Bitmask of the effective days in the order XSFTWTMS
 *               startHour - The start hour of the Week Day Schedule: 0-23
 *               startMinute - The start minute of the Week Day Schedule: 0-59
 *               endHour - The end hour of the Week Day Schedule: 0-23
 *               endMinute - The end minute of the Week Day Schedule: 0-59
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetWeekDayScheduleResponse( uint8 srcEP, uint16 dstAddr,
                                                              zclDoorLockGetWeekDayScheduleRsp_t *pCmd,
                                                              uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_GET_WEEK_DAY_SCHEDULE_RSP];
  uint8 payloadLen = sizeof( payload );

  payload[0] = pCmd->scheduleID;
  zb_u16tob(&payload[1],pCmd->userID);
  payload[3] = pCmd->status;

  if ( pCmd->status == ZCL_STATUS_SUCCESS )
  {
    payload[4] = pCmd->daysMask;
    payload[5] = pCmd->startHour;
    payload[6] = pCmd->startMinute;
    payload[7] = pCmd->endHour;
    payload[8] = pCmd->endMinute;
  }
  else
  {
    payloadLen = 4;
  }

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_GET_WEEK_DAY_SCHEDULE_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                          disableDefaultRsp, 0, seqNum, payloadLen, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetYearDayScheduleResponse
 *
 * @brief   Call to send out a Get Year Day Schedule Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pCmd:
 *             scheduleID - The Schedule ID # is between 0 - [# Schedule IDs per user attribute]
 *             userID - User ID is between 0 - [# PINs User supported attribute]
 *             status - Returns the state due to the requesting command
 *             zigBeeLocalStartTime - Start time of the Year Day Schedule representing by ZigBeeLocalTime
 *             zigBeeLocalEndTime - End time of the Year Day Schedule representing by ZigBeeLocalTime
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetYearDayScheduleResponse( uint8 srcEP, uint16 dstAddr,
                                                              zclDoorLockGetYearDayScheduleRsp_t *pCmd,
                                                              uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_GET_YEAR_DAY_SCHEDULE_RSP];
  uint8 payloadLen = sizeof( payload );

  payload[0] = pCmd->scheduleID;
  zb_u16tob(&payload[1],pCmd->userID);
  payload[3] = pCmd->status;

  if ( pCmd->status == ZCL_STATUS_SUCCESS )
  {
    zb_u32tob(&payload[4],pCmd->zigBeeLocalStartTime);
    zb_u32tob(&payload[8],pCmd->zigBeeLocalEndTime);
  }
  else
  {
    payloadLen = 4;
  }

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_GET_YEAR_DAY_SCHEDULE_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                          disableDefaultRsp, 0, seqNum, payloadLen, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetHolidayScheduleResponse
 *
 * @brief   Call to send out a Get Holiday Schedule Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pCmd:
 *             holidayScheduleID - A unique ID for given Holiday Schedule (0 to 254)
 *             status - Returns the state due to the requesting command
 *             zigBeeLocalStartTime - Start time of the Year Day Schedule representing by ZigBeeLocalTime
 *             zigBeeLocalEndTime - End time of the Year Day Schedule representing by ZigBeeLocalTime
 *             operatingModeDuringHoliday - A valid enumeration value as listed in operating mode attribute
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetHolidayScheduleResponse( uint8 srcEP, uint16 dstAddr,
                                                              zclDoorLockGetHolidayScheduleRsp_t *pCmd,
                                                              uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_GET_HOLIDAY_SCHEDULE_RSP];
  uint8 payloadLen = sizeof( payload );

  payload[0] = pCmd->holidayScheduleID;
  payload[1] = pCmd->status;

  if ( pCmd->status == ZCL_STATUS_SUCCESS )
  {
    zb_u32tob(&payload[2],pCmd->zigBeeLocalStartTime);
    zb_u32tob(&payload[6],pCmd->zigBeeLocalEndTime);
    payload[10] = pCmd->operatingModeDuringHoliday;
  }
  else
  {
    payloadLen = 2;
  }

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_GET_HOLIDAY_SCHEDULE_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                          disableDefaultRsp, 0, seqNum, payloadLen, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetUserTypeResponse
 *
 * @brief   Call to send out a Get User Type Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   userID - User ID is between 0 - [# PINs User supported attribute]
 * @param   userType - Used to indicate what the type is for a specific User ID
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetUserTypeResponse( uint8 srcEP, uint16 dstAddr,
                                                       uint16 userID, uint8 userType,
                                                       uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 payload[PAYLOAD_LEN_GET_USER_TYPE_RSP];
  zb_u16tob(&payload[0],userID);
  payload[2] = userType;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                          COMMAND_CLOSURES_GET_USER_TYPE_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                          disableDefaultRsp, 0, seqNum, PAYLOAD_LEN_GET_USER_TYPE_RSP, payload );
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockGetRFIDCodeResponse
 *
 * @brief   Call to send out a Get RFID Code Response
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           userID - User ID is between 0 - [# PINs User supported attribute]
 *           userStatus - Used to indicate what the status is for a specific User ID
 *           userType - Used to indicate what the type is for a specific User ID
 *           aRfidCode - A ZigBee string indicating the RFID code used to create the event
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockGetRFIDCodeResponse( uint8 srcEP, uint16 dstAddr,
                                                       zclDoorLockGetRFIDCodeRsp_t *pPayload,
                                                       uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint8 calculatedBufSize;
  osState status;

  // first octet of PIN/RFID Code variable string identifies its length
  calculatedArrayLen = pPayload->pRfidCode[0] + 1;   // add first byte of string

  // determine total size of buffer
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_GET_RFID_CODE_RSP;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  zb_u16tob(&pBuf[0],pPayload->userID);
  pBuf[2] = pPayload->userStatus;
  pBuf[3] = pPayload->userType;
  offset = 4;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pRfidCode[i];
  }

  status = zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_GET_RFID_CODE_RSP, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockOperationEventNotification
 *
 * @brief   Call to send out a Operation Event Notification
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           operationEventSource - Indicates where the event was triggered from
 *           operationEventCode - (Optional) a notification whenever there is a significant operation event on the lock
 *           userID - User ID is between 0 - [# PINs User supported attribute]
 *           pin - The PIN that is associated with the User ID who performed the event
 *           zigBeeLocalTime - Indicates when the event is triggered
 *           aData - Used to pass data associated with a particular event
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockOperationEventNotification( uint8 srcEP, uint16 dstAddr,
                                                              zclDoorLockOperationEventNotification_t *pPayload,
                                                              uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint16 calculatedBufSize;
  osState status;

  // set variable length if data is available
  calculatedArrayLen = pPayload->pData[0] + 1; // add first byte of string

  // determine total size of buffer
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_OPERATION_EVENT_NOTIFICATION;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  pBuf[0] = pPayload->operationEventSource;
  pBuf[1] = pPayload->operationEventCode;
  zb_u16tob(&pBuf[2],pPayload->userID);
  pBuf[4] = pPayload->pin;
  zb_u32tob(&pBuf[5],pPayload->zigBeeLocalTime);
  offset = 9;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pData[i];
  }

  status = zcl_SendCommand( dstAddr, srcEP, ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_OPERATION_EVENT_NOTIFICATION, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

/*********************************************************************
 * @fn      zclClosures_SendDoorLockProgrammingEventNotification
 *
 * @brief   Call to send out a Programming Event Notification
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   pPayload:
 *           programEventSource - Indicates where the event was triggered from
 *           programEventCode - (Optional) a notification whenever there is a significant programming event on the lock
 *           userID - User ID is between 0 - [# PINs User supported attribute]
 *           pin - The PIN that is associated with the User ID who performed the event
 *           userType - Used to indicate what the type is for a specific User ID
 *           userStatus - Used to indicate what the status is for a specific User ID
 *           zigBeeLocalTime - Indicates when the event is triggered
 *           dataLen - Manufacture specific, describes length of aData
 *           aData - Used to pass data associated with a particular event
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_SendDoorLockProgrammingEventNotification( uint8 srcEP, uint16 dstAddr,
                                                                zclDoorLockProgrammingEventNotification_t *pPayload,
                                                                uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 i;
  uint8 *pBuf;  // variable length payload
  uint8 offset;
  uint8 calculatedArrayLen;
  uint16 calculatedBufSize;
  osState status;

  // set variable length if data is available
  calculatedArrayLen = pPayload->pData[0] + 1;  // add first byte of string

  // determine total size of buffer
  calculatedBufSize = calculatedArrayLen + PAYLOAD_LEN_PROGRAMMING_EVENT_NOTIFICATION;

  pBuf = zcl_mem_alloc( calculatedBufSize );
  if ( !pBuf )
  {
    return ( osFailed );  // no memory
  }

  // over-the-air is always little endian. Break into a byte stream.
  pBuf[0] = pPayload->programEventSource;
  pBuf[1] = pPayload->programEventCode;
  zb_u16tob(&pBuf[2],pPayload->userID);
  pBuf[4] = pPayload->pin;
  pBuf[5] = pPayload->userType;
  pBuf[6] = pPayload->userStatus;
  zb_u32tob(&pBuf[7],pPayload->zigBeeLocalTime);
  offset = 11;
  for ( i = 0; i < calculatedArrayLen; i++ )
  {
    pBuf[offset++] = pPayload->pData[i];
  }

  status = zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
                            COMMAND_CLOSURES_PROGRAMMING_EVENT_NOTIFICATION, osTrue, ZCL_FRAME_SERVER_CLIENT_DIR,
                            disableDefaultRsp, 0, seqNum, calculatedBufSize, pBuf );
  zcl_mem_free( pBuf );
  return status;
}

#endif //ZCL_DOORLOCK

#ifdef ZCL_WINDOWCOVERING
/*********************************************************************
 * @fn      zclClosures_WindowCoveringSimpleReq
 *
 * @brief   Call to send out a Window Covering command with no payload
 *          as Up/Open, Down/Close or Stop
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_WindowCoveringSimpleReq( uint8 srcEP, uint16 dstAddr,
                                               uint8 cmd, uint8 disableDefaultRsp, uint8 seqNum )
{
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 0, NULL );
}

/*********************************************************************
 * @fn      zclClosures_WindowCoveringSendGoToValueReq
 *
 * @brief   Call to send out a Go to Value Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID for COMMAND_CLOSURES_GO_TO_LIFT_VALUE
 * @param   liftValue - payload
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_WindowCoveringSendGoToValueReq( uint8 srcEP, uint16 dstAddr,
                                                      uint8 cmd, uint16 Value,
                                                      uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[ZCL_WC_GOTOVALUEREQ_PAYLOADLEN];
  zb_u16tob(&buf[0],Value);
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum,
                          ZCL_WC_GOTOVALUEREQ_PAYLOADLEN, buf );
}

/*********************************************************************
 * @fn      zclClosures_WindowCoveringSendGoToPercentageReq
 *
 * @brief   Call to send out a Go to Percentage Request Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   cmd - Command ID e.g. COMMAND_CLOSURES_GO_TO_LIFT_PERCENTAGE
 * @param   percentageLiftValue - payload
 * @param   disableDefaultRsp - decides default response is necessary or not
 * @param   seqNum - sequence number of the command packet
 *
 * @return  osState
 */
osState zclClosures_WindowCoveringSendGoToPercentageReq( uint8 srcEP, uint16 dstAddr,
                                                           uint8 cmd, uint8 percentageValue,
                                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[ZCL_WC_GOTOPERCENTAGEREQ_PAYLOADLEN];

  buf[0] = percentageValue;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum,
                          ZCL_WC_GOTOPERCENTAGEREQ_PAYLOADLEN, buf );
}
#endif //ZCL_WINDOWCOVERING
