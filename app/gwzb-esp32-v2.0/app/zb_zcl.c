/*
 * zb_zcl.c
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
#include "zb_zdo.h"
#include "zb_map.h"
#include "zb_app.h"

typedef struct
{
	uint8 addr[2];
	uint8 ep;
	uint8 seq;
	uint8 disable_respone;
	uint8 cID[2];
	uint8 len;
	struct
	{
		uint8 cmd;
		uint8 specific;
		uint8 direction;
		uint8 manucode[2];
		uint8 dlen;
		uint8 data[0];
	}payload;
}zcl_pkg_t;


void zb_zcl_message_process(uint8 cmd1, uint8 *pdata, uint8 len)
{
	zcl_pkg_t	*zcl_pkg;
	uint16		addr;
	uint16 		cID;
	osLogI(DBG_MT_APP,"zb_zcl_message_process, cmd1:%02x, len:%d ",cmd1,len);
	osLogB(DBG_MT_APP,"data:",pdata,len);
	switch(cmd1)
	{
	case MT_CmdAppZCLCommand:
		zcl_pkg = (zcl_pkg_t*)pdata;
		addr = zb_btou16(zcl_pkg->addr);
		cID = zb_btou16(zcl_pkg->cID);
		if(zb_check_addr_exist(addr)==osTrue)
		{
			zb_io_remove_send(addr,zcl_pkg->seq,osFalse);
			zb_app_receive_message(addr,zcl_pkg->ep,zcl_pkg->seq,cID,zcl_pkg->payload.cmd,zcl_pkg->payload.specific,zcl_pkg->payload.data,zcl_pkg->payload.dlen);
		}
		else
		{
			osLogI(DBG_MT_ERROR,"zb_zcl_message_process, unregistered device:%04x \r\n",addr);
		}
		break;
	}
}

osState zcl_SendRead(uint16 dstAddr,uint8 ep,uint16 clusterID, zclReadCmd_t *readCmd,
                        uint8 direction, uint8 disableDefaultRsp, uint8 seqNum)
{
  uint16 dataLen;
  uint8 *buf;
  uint8 *pBuf;
  osState status;
  dataLen = readCmd->numAttr * 2; // Attribute ID
  buf = osMalloc( dataLen );
  if ( buf != NULL )
  {
    uint8 i;
    // Load the buffer - serially
    pBuf = buf;
    for (i = 0; i < readCmd->numAttr; i++)
    {
    	zb_u16tob(pBuf,readCmd->attrID[i]);
    	pBuf+=2;
    }
    status = zcl_SendCommand( dstAddr,ep, clusterID, ZCL_CMD_READ, osFalse,
                              direction, disableDefaultRsp, 0, seqNum, dataLen, buf );
    osFree( buf );
  }
  else
  {
    status = osFailed;
  }
  return ( status );
}

osState zcl_SendCommand(uint16 dst,uint8 ep,uint16 clusterID, uint8 cmd,
						uint8 specific, uint8 direction,uint8 disableDefaultRsp,
						uint16 manuCode, uint8 seqNum,
                        uint16 cmdFormatLen, uint8 *cmdFormat )
{
	char buf[255];
	zcl_pkg_t *pkg;
	osLogI(0,"zcl_SendCommand, addr :%04x, ep:%02x, seq:%02x \r\n",dst,ep,seqNum);
	uint8 ret = osFailed;
	uint8 slen = sizeof(zcl_pkg_t)+cmdFormatLen;
	pkg = (zcl_pkg_t*)buf;
	if(pkg!=NULL)
	{
		zb_u16tob(pkg->addr,dst);
		pkg->ep = ep;
		zb_u16tob(pkg->cID,clusterID);
		pkg->disable_respone = disableDefaultRsp;
		pkg->seq = zb_get_seq();
		pkg->payload.cmd = cmd;
		pkg->payload.direction = direction;
		pkg->payload.specific = specific;
		zb_u16tob(pkg->payload.manucode,manuCode);
		pkg->payload.dlen = cmdFormatLen;
		if((cmdFormatLen>0)&&(cmdFormat!=NULL))
		{
			osMemcpy(pkg->payload.data,cmdFormat,cmdFormatLen);
		}
		ret = zb_io_req_send((MT_RPC_CMD_SREQ|MT_RPC_SYS_APP),MT_CmdAppZCLCommand,dst,pkg->seq,(uint8*)pkg,slen,osTrue);
	}
	return ret;
}
osState zcl_SendLevelControlMoveToLevelRequest( uint16 dstAddr,uint8 ep,
                                                         uint8 cmd, uint8 level, uint16 transTime,
                                                         uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[3];

  buf[0] = level;
  zb_u16tob(&buf[1],transTime);
  return zcl_SendCommand( dstAddr,ep, ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 3, buf );
}
osState zcl_SendLevelControlStepRequest( uint16 dstAddr,uint8 ep,
                                                  uint8 cmd, uint8 stepMode, uint8 stepSize, uint16 transTime,
                                                  uint8 disableDefaultRsp, uint8 seqNum )
{
  uint8 buf[4];

  buf[0] = stepMode;
  buf[1] = stepSize;
  zb_u16tob(&buf[2],transTime);

  return zcl_SendCommand( dstAddr,ep,  ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
                          cmd, osTrue, ZCL_FRAME_CLIENT_SERVER_DIR,
                          disableDefaultRsp, 0, seqNum, 4, buf );
}

