/*********************************************************************
 * INCLUDES
 */
#include "osCore.h"
#include "zb_common.h"
#include "zb_zcl.h"
#include "zb_zcl_general.h"
#include "zcl_lighting.h"

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToHueCmd
 *
 * @brief   Call to send out a Move To Hue Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   hue - target hue value
 * @param   direction - direction of hue change
 * @param   transitionTime - tame taken to move to the target hue in 1/10 sec increments
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveToHueCmd( uint8 srcEP, uint16  dstAddr,
                                                      uint8 hue, uint8 direction, uint16 transitionTime,
                                                      uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];

  buf[0] = hue;
  buf[1] = direction;
  zb_u16tob(&buf[2],transitionTime);
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_TO_HUE, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 4, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveHueCmd
 *
 * @brief   Call to send out a Move To Hue Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   moveMode - LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP,
 *                     LIGHTING_MOVE_HUE_DOWN
 * @param   rate - the movement in steps per second, where step is
 *                 a change in the device's hue of one unit
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveHueCmd( uint8 srcEP, uint16  dstAddr,
                                                    uint8 moveMode, uint8 rate,
                                                    uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];

  buf[0] = moveMode;
  buf[1] = rate;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_HUE, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_StepHueCmd
 *
 * @brief   Call to send out a Step Hue Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   stepMode -	LIGHTING_STEP_HUE_UP, LIGHTING_STEP_HUE_DOWN
 * @param   stepSize -  change to the current value of the device's hue
 * @param   transitionTime - the movement in steps per 1/10 second
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_StepHueCmd( uint8 srcEP, uint16  dstAddr,
                                  uint8 stepMode, uint8 stepSize, uint8 transitionTime,
                                  uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[3];

  buf[0] = stepMode;
  buf[1] = stepSize;
  buf[2] = transitionTime;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_STEP_HUE, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 3, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToSaturationCmd
 *
 * @brief   Call to send out a Move To Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   saturation - target saturation value
 * @param   transitionTime - time taken move to the target saturation,
 *                           in 1/10 second units
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveToSaturationCmd( uint8 srcEP, uint16  dstAddr,
                                         uint8 saturation, uint16 transitionTime,
                                         uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[3];

  buf[0] = saturation;
  zb_u16tob(&buf[1],transitionTime);
  return zcl_SendCommand( dstAddr, srcEP, ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_TO_SATURATION, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 3, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveSaturationCmd
 *
 * @brief   Call to send out a Move Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   moveMode - LIGHTING_MOVE_SATURATION_STOP, LIGHTING_MOVE_SATURATION_UP,
 *                     LIGHTING_MOVE_SATURATION_DOWN
 * @param   rate - rate of movement in step/sec; step is the device's saturation of one unit
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveSaturationCmd( uint8 srcEP, uint16  dstAddr,
                                                           uint8 moveMode, uint8 rate,
                                                           uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[2];

  buf[0] = moveMode;
  buf[1] = rate;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_SATURATION, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 2, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_StepSaturationCmd
 *
 * @brief   Call to send out a Step Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   stepMode -  LIGHTING_STEP_SATURATION_UP, LIGHTING_STEP_SATURATION_DOWN
 * @param   stepSize -  change to the current value of the device's hue
 * @param   transitionTime - time to perform a single step in 1/10 of second
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_StepSaturationCmd( uint8 srcEP, uint16  dstAddr,
                                         uint8 stepMode, uint8 stepSize, uint8 transitionTime,
                                         uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[3];

  buf[0] = stepMode;
  buf[1] = stepSize;
  buf[2] = transitionTime;

  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_STEP_SATURATION, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 3, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToHueAndSaturationCmd
 *
 * @brief   Call to send out a Move To Hue And Saturation Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   hue - a target hue
 * @param   saturation - a target saturation
 * @param   transitionTime -  time to move, equal of the value of the field in 1/10 seconds
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveToHueAndSaturationCmd( uint8 srcEP, uint16  dstAddr,
                                                   uint8 hue, uint8 saturation, uint16 transitionTime,
                                                   uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];

  buf[0] = hue;
  buf[1] = saturation;
  zb_u16tob(&buf[2],transitionTime);
  return zcl_SendCommand( dstAddr,srcEP,  ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_TO_HUE_AND_SATURATION, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 4, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToColorCmd
 *
 * @brief   Call to send out a Move To Color Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   colorX - a target color X
 * @param   colorY - a target color Y
 * @param   transitionTime -  time to move, equal of the value of the field in 1/10 seconds
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveToColorCmd( uint8 srcEP, uint16  dstAddr,
                                       uint16 colorX, uint16 colorY, uint16 transitionTime,
                                       uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[6];
  zb_u16tob(&buf[0],colorX);
  zb_u16tob(&buf[2],colorY);
  zb_u16tob(&buf[4],transitionTime);
  return zcl_SendCommand( dstAddr, srcEP, ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_TO_COLOR, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 6, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveColorCmd
 *
 * @brief   Call to send out a Move Color Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   rateX - rate of movement in steps per second. A step is a change
 *                  in the device's CurrentX attribute of one unit.
 * @param   rateY - rate of movement in steps per second. A step is a change
 *                  in the device's CurrentY attribute of one unit.
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveColorCmd( uint8 srcEP, uint16  dstAddr,
                                                      int16 rateX, int16 rateY,
                                                      uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];
  zb_u16tob(&buf[0],rateX);
  zb_u16tob(&buf[2],rateY);

  return zcl_SendCommand( dstAddr, srcEP, ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_COLOR, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 4, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_StepColorCmd
 *
 * @brief   Call to send out a Step Color Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   stepX - change to be added to the device's CurrentX attribute
 * @param   stepY - change to be added to the device's CurrentY attribute
 * @param   transitionTime -  time to perform the color change, equal of
 *                            the value of the field in 1/10 seconds
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_StepColorCmd( uint8 srcEP, uint16  dstAddr,
                                         int16 stepX, int16 stepY, uint16 transitionTime,
                                         uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[6];
  zb_u16tob(&buf[0],stepX);
  zb_u16tob(&buf[2],stepY);
  zb_u16tob(&buf[4],transitionTime);

  return zcl_SendCommand( dstAddr, srcEP, ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_STEP_COLOR, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 6, buf );
}

/*********************************************************************
 * @fn      zclLighting_ColorControl_Send_MoveToColorTemperatureCmd
 *
 * @brief   Call to send out a Move To Color Temperature Command
 *
 * @param   srcEP - Sending application's endpoint
 * @param   dstAddr - where you want the message to go
 * @param   colorTemperature - a target color temperature
 * @param   transitionTime -  time to perform the color change, equal of
 *                            the value of the field in 1/10 seconds
 * @param   disableDefaultRsp - whether to disable the Default Response command
 * @param   seqNum - sequence number
 *
 * @return  osState
 */
osState zclLighting_ColorControl_Send_MoveToColorTemperatureCmd( uint8 srcEP, uint16  dstAddr,
                                                       uint16 colorTemperature, uint16 transitionTime,
                                                       uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];
  zb_u16tob(&buf[0],colorTemperature);
  zb_u16tob(&buf[2],transitionTime);

  return zcl_SendCommand(  dstAddr, srcEP,ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
                          COMMAND_LIGHTING_MOVE_TO_COLOR_TEMPERATURE, osTrue,
                          ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 4, buf );
}

/****************************************************************************
****************************************************************************/
