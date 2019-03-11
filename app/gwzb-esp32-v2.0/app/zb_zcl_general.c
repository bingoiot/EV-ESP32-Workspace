/*
 * zb_zcl_general.c
 *
 *  Created on: 2017��7��11��
 *      Author: lort
 */
#include "osCore.h"
#include "hal_uart.h"
#include "zb_common.h"
#include "zb_io_buffer.h"
#include "zb_uart.h"
#include "zb_zcl.h"
#include "zb_zcl_ha.h"
#include "zb_zcl_general.h"

osState zclGeneral_SendOnOff(uint16 dstAddr, uint8 ep, uint8 disableDefaultRsp, uint8 seqNum ,uint8 cmd)
{
	return zcl_SendCommand(dstAddr,ep, ZCL_CLUSTER_ID_GEN_ON_OFF, cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR, disableDefaultRsp, 0, seqNum, 0, NULL );
}

osState zclGeneral_SendLevelControlMoveToLevel( uint16 dstAddr,uint8 ep, uint8 level, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum )
{
	return zcl_SendLevelControlMoveToLevelRequest( dstAddr, ep, COMMAND_LEVEL_MOVE_TO_LEVEL, level, transTime, disableDefaultRsp ,seqNum );
}


osState zclGeneral_SendLevelControlMoveRequest( uint16 dstAddr,uint8 ep, uint8 moveMode, uint8 rate, uint8 disableDefaultRsp, uint8 seqNum )
{
	return zcl_SendLevelControlMoveToLevelRequest( dstAddr,ep, COMMAND_LEVEL_MOVE, (moveMode), (rate), (disableDefaultRsp), (seqNum) );
}

osState zclGeneral_SendLevelControlStep( uint16 dstAddr,uint8 ep, uint8 stepMode, uint8 stepSize, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum )
{
	return zcl_SendLevelControlStepRequest(dstAddr,ep, COMMAND_LEVEL_STEP, stepMode, stepSize, transTime, disableDefaultRsp, seqNum );
}

osState zclGeneral_SendLevelControlStop(uint16 dstAddr,uint8 ep, uint8 disableDefaultRsp, uint8 seqNum )
{
	return zcl_SendCommand( dstAddr, ep,ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL, COMMAND_LEVEL_STOP, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR, (disableDefaultRsp), 0, (seqNum), 0, NULL );
}

osState zclGeneral_SendLevelControlMoveToLevelWithOnOff( uint16 dstAddr,uint8 ep, uint8 level, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum )
{
	return zcl_SendLevelControlMoveToLevelRequest( dstAddr, ep, COMMAND_LEVEL_MOVE_TO_LEVEL_WITH_ON_OFF, level, transTime, disableDefaultRsp, seqNum );
}

osState zclGeneral_SendLevelControlMoveWithOnOff( uint16 dstAddr,uint8 ep,  uint8 moveMode, uint8 rate, uint8 disableDefaultRsp, uint8 seqNum )
{
	return zcl_SendLevelControlMoveToLevelRequest( dstAddr, ep,  COMMAND_LEVEL_MOVE_WITH_ON_OFF,moveMode,  rate,  disableDefaultRsp,  seqNum  );
}

osState zclGeneral_SendLevelControlStepWithOnOff( uint16 dstAddr,uint8 ep,  uint8 stepMode, uint8 stepSize, uint16 transTime, uint8 disableDefaultRsp, uint8 seqNum )
{
	return zcl_SendLevelControlStepRequest( dstAddr, ep, COMMAND_LEVEL_STEP_WITH_ON_OFF, stepMode,  stepSize,  transTime,  disableDefaultRsp,  seqNum );
}

osState zclGeneral_SendLevelControlStopWithOnOff( uint16 dstAddr,uint8 ep,  uint8 disableDefaultRsp, uint8 seqNum )
{
	return  zcl_SendCommand(  dstAddr, ep, ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL, COMMAND_LEVEL_STOP_WITH_ON_OFF, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR, (disableDefaultRsp), 0, (seqNum), 0, NULL );
}
