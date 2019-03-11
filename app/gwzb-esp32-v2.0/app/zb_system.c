/*
 * zb_system.c
 *
 *  Created on: 2017��7��10��
 *      Author: lort
 */

#include "osCore.h"
#include "zb_common.h"
#include "zb_uart.h"
#include "zb_system.h"

#define byte uint8
#define Z_EXTADDR_LEN	(8)
#define SEC_KEY_LEN		(16)

// OSAL NV item IDs
#define ZCD_NV_EXTADDR                    0x0001
#define ZCD_NV_BOOTCOUNTER                0x0002
#define ZCD_NV_STARTUP_OPTION             0x0003
#define ZCD_NV_START_DELAY                0x0004

#define ZCD_NV_NIB                        0x0021
#define ZCD_NV_DEVICE_LIST                0x0022
#define ZCD_NV_ADDRMGR                    0x0023

#define ZCD_NV_BINDING_TABLE              0x0041
#define ZCD_NV_GROUP_TABLE                0x0042

#define ZCD_NV_NWK_PARENT_INFO            0x0051
#define ZCD_NV_NWKMGR_ADDR                0x0089

#define ZCD_NV_SCENE_TABLE                0x0091

typedef enum
{
  NWK_INIT,
  NWK_JOINING_ORPHAN,
  NWK_DISC,
  NWK_JOINING,
  NWK_ENDDEVICE,
  PAN_CHNL_SELECTION,
  PAN_CHNL_VERIFY,
  PAN_STARTING,
  NWK_ROUTER,
  NWK_REJOINING
} nwk_states_t;

typedef struct
{
  byte  nwkDevAddress[2];//20,21
  byte  nwkLogicalChannel;//22
  byte  nwkCoordAddress[2];//23,24
  byte  nwkCoordExtAddress[Z_EXTADDR_LEN];
  byte  nwkPanId[2];
}NetworkInfo_t;

typedef struct
{
  uint8 keySeqNum;
  uint8 key[SEC_KEY_LEN];
} NwkKeyDescribe_t;

static osState 	zb_req_read_nv(uint16 nvID, uint8 offset);
static uint8 	zb_req_read_nv_length(uint16 nv_id);
static osState 	zb_req_delete_nv(uint16 nvID, uint8 len);
static osState 	zb_req_write_nv(uint16 nvID, uint8 offset, uint8 *pdata, uint16 len);

osState zb_factory_reset(void)
{
	osState ret;
	uint8 slen;
	/*if((slen = zb_req_read_nv_length(ZCD_NV_BOOTCOUNTER))>0)
		ret = zb_req_delete_nv(ZCD_NV_BOOTCOUNTER,slen);
	if((slen = zb_req_read_nv_length(ZCD_NV_STARTUP_OPTION))>0)
		ret = zb_req_delete_nv(ZCD_NV_STARTUP_OPTION,slen);
	if((slen = zb_req_read_nv_length(ZCD_NV_NIB))>0)
		ret = zb_req_delete_nv(ZCD_NV_NIB,slen);
	if((slen = zb_req_read_nv_length(ZCD_NV_DEVICE_LIST))>0)
		ret = zb_req_delete_nv(ZCD_NV_DEVICE_LIST,slen);
	if((slen = zb_req_read_nv_length(ZCD_NV_ADDRMGR))>0)
		ret = zb_req_delete_nv(ZCD_NV_ADDRMGR,slen);
	if((slen = zb_req_read_nv_length(ZCD_NV_BINDING_TABLE))>0)
		ret = zb_req_delete_nv(ZCD_NV_BINDING_TABLE,slen);
	if((slen = zb_req_read_nv_length(ZCD_NV_GROUP_TABLE))>0)
		ret = zb_req_delete_nv(ZCD_NV_GROUP_TABLE,slen);
	if((slen = zb_req_read_nv_length(ZCD_NV_SCENE_TABLE))>0)
		ret = zb_req_delete_nv(ZCD_NV_SCENE_TABLE,slen);
	ret = zb_reset();*/
	ret = zb_uart_write(MT_RPC_CMD_AREQ|MT_RPC_SYS_APP,0x07,NULL,0);
	return ret;
}
osState zb_reset(void)
{
	osState ret = osFailed;
	uint8 	slen;
	uint8 state = 0x01;//soft reset
	zb_uart_write(MT_RPC_CMD_AREQ|MT_RPC_SYS_SYS,MT_SYS_RESET_REQ,&state,1);
	if((slen=zb_uart_read(MT_RPC_CMD_AREQ|MT_RPC_SYS_SYS,MT_SYS_RESET_IND,&state,1))>0)
	{
		osLogI(DBG_MT_SYS,"reset ok, reason is :%02x \r\n",state);
		ret = osSucceed;
	}
	return ret;
}
osState zb_read_network_info(void)
{
	NetworkInfo_t *pInfo;
	uint8 	buf[sizeof(NetworkInfo_t)+2];
	uint8	slen;
	osState ret;
	ret = zb_req_read_nv(ZCD_NV_NIB,20);
	if((slen=zb_uart_read(MT_RPC_CMD_SRSP|MT_RPC_SYS_SYS,MT_SYS_OSAL_NV_READ,buf,sizeof(buf)))>0)
	{
		osLogB(DBG_MT_SYS,"zb_read_network_info:",buf,slen);
		pInfo = (NetworkInfo_t*)&buf[2];
		osLogI(1,"zb_read_network_info: channel:%d panID:%02x%02x \r\n",pInfo->nwkLogicalChannel,pInfo->nwkPanId[1],pInfo->nwkPanId[0]);
	}
	return ret;
}
osState zb_set_network(uint8 channel, uint16 panID, uint16 devAddr, uint8 *exAddr)
{
	osState ret = osFailed;
	NetworkInfo_t NwkInfo;
	if((channel>=11)&&(channel)&&(panID!=0x0000)&&(panID!=0xFFFF))
	{
		osMemset(&NwkInfo,0,sizeof(NwkInfo));
		zb_u16tob((uint8*)&NwkInfo.nwkDevAddress,devAddr);
		NwkInfo.nwkLogicalChannel = channel;
		zb_u16tob((uint8*)&NwkInfo.nwkCoordAddress,0x0000);
		osMemcpy(&NwkInfo.nwkCoordExtAddress,exAddr,8);
		zb_u16tob((uint8*)&NwkInfo.nwkPanId,panID);
		osLogB(1,"MTSYS_ReqSetNetwork: ",(uint8*)(&NwkInfo),sizeof(NetworkInfo_t));
		ret = zb_req_write_nv(ZCD_NV_NIB,20,(uint8*)(&NwkInfo),sizeof(NetworkInfo_t));
		ret = zb_reset();
	}
	return ret;
}


static osState zb_req_read_nv(uint16 nvID, uint8 offset)
{
	uint8 buf[3];
	zb_u16tob(buf,nvID);
	buf[2] = offset;
	return zb_uart_write(MT_RPC_CMD_SREQ|MT_RPC_SYS_SYS,MT_SYS_OSAL_NV_READ,buf,3);
}
static uint8 zb_req_read_nv_length(uint16 nv_id)
{
	uint16 temp = 0;
	uint8 buf[2];
	uint8 slen;
	zb_u16tob(buf,nv_id);
	zb_uart_write(MT_RPC_CMD_SREQ|MT_RPC_SYS_SYS,MT_SYS_OSAL_NV_LENGTH,buf,2);
	if((slen=zb_uart_read(MT_RPC_CMD_SRSP|MT_RPC_SYS_SYS,MT_SYS_OSAL_NV_LENGTH,buf,2))>0)
	{
		temp = zb_btou16(buf);
	}
	osLogI(DBG_MT_SYS,"zb_req_read_nv_length :%04x \r\n",temp);
	return temp;
}
static osState zb_req_delete_nv(uint16 nvID, uint8 len)
{
	uint8 buf[4];
	zb_u16tob(buf,nvID);
	zb_u16tob(&buf[2],len);
	return zb_uart_write(MT_RPC_CMD_SREQ|MT_RPC_SYS_SYS,MT_SYS_OSAL_NV_DELETE,buf,4);
}

static osState zb_req_write_nv(uint16 nvID, uint8 offset, uint8 *pdata, uint16 len)
{
	osState ret = osFailed;
	uint8 *pbuf;
	uint8 slen;
	slen = 2+1+1+len;
	pbuf = (uint8*)osMalloc(slen);
	if(pbuf!=NULL)
	{
		zb_u16tob(pbuf,nvID);
		pbuf[2] = offset;
		pbuf[3] = (uint8)(len);
		osMemcpy(&pbuf[4],pdata,len);
		ret = zb_uart_write(MT_RPC_CMD_SREQ|MT_RPC_SYS_SYS,MT_SYS_OSAL_NV_WRITE,pbuf,slen);
		osFree(pbuf);
	}
	return ret;
}


/*
static void dumpNwkInfo(void)
{
	nwkIB_t *pnwk = &nwk;
	jf_printf(1,"SequenceNum:%02x \r\n",pnwk->SequenceNum);
	jf_printf(1,"PassiveAckTimeout:%02x \r\n",pnwk->PassiveAckTimeout);
	jf_printf(1,"MaxBroadcastRetries:%02x \r\n",pnwk->MaxBroadcastRetries);
	jf_printf(1,"MaxChildren:%02x \r\n",pnwk->MaxChildren);
	jf_printf(1,"MaxDepth:%02x \r\n",pnwk->MaxDepth);
	jf_printf(1,"MaxRouters:%02x \r\n",pnwk->MaxRouters);
	jf_printf(1,"dummyNeighborTable:%02x \r\n",pnwk->dummyNeighborTable);
	jf_printf(1,"BroadcastDeliveryTime:%02x \r\n",pnwk->BroadcastDeliveryTime);
	jf_printf(1,"ReportConstantCost:%02x \r\n",pnwk->ReportConstantCost);
	jf_printf(1,"RouteDiscRetries:%02x \r\n",pnwk->RouteDiscRetries);
	jf_printf(1,"dummyRoutingTable:%02x \r\n",pnwk->dummyRoutingTable);
	jf_printf(1,"SecureAllFrames:%02x \r\n",pnwk->SecureAllFrames);
	jf_printf(1,"SecurityLevel:%02x \r\n",pnwk->SecurityLevel);

	jf_printf(1,"SymLink:%02x \r\n",pnwk->SymLink);
	jf_printf(1,"CapabilityFlags:%02x \r\n",pnwk->CapabilityFlags);
	jf_printb(1,"TransactionPersistenceTime:",pnwk->TransactionPersistenceTime,2);
	jf_printf(1,"nwkProtocolVersion:%02x \r\n",pnwk->nwkProtocolVersion);
	jf_printf(1,"RouteDiscoveryTime:%02x \r\n",pnwk->RouteDiscoveryTime);
	jf_printf(1,"RouteExpiryTime:%02x \r\n",pnwk->RouteExpiryTime);
	jf_printb(1,"nwkDevAddress:",pnwk->nwkDevAddress,2);
	jf_printf(1,"nwkLogicalChannel:%02x \r\n",pnwk->nwkLogicalChannel);
	jf_printb(1,"nwkCoordAddress:",pnwk->nwkCoordAddress,2);
	jf_printb(1,"nwkCoordExtAddress:",pnwk->nwkCoordExtAddress,Z_EXTADDR_LEN);
	jf_printb(1,"nwkPanId:",pnwk->nwkPanId,2);
}
*/

