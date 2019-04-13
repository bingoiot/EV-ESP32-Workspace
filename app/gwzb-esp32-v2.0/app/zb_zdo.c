/*
 * zb_zdo.c
 *
 *  Created on: 2017��7��11��
 *      Author: lort
 */
#include "osCore.h"
#include "hal_uart.h"
#include "zb_common.h"
#include "zb_uart.h"
#include "zb_map.h"
#include "zb_zdo.h"
#include "zb_io_buffer.h"

typedef struct
{
	void 	*next;
	uint8	exAddr[8];
	uint16 	addr;
	uint8	stry;
	uint8	point_num;
	uint8	finish_count;
	uint8	lqi;
}Device_t;

static Device_t	*pNewDevice = NULL;
static osState req_read_active_ep(uint16 addr);
static osState req_ep_describe(uint16 addr, uint8 point);
static void zdo_task(int sig, void *arg, int len);
static void remove_new_device(uint16 addr);
static uint8 *get_exaddr_by_addr(uint16 addr);

static void device_indication_process(uint8 *pdata, uint8 len);
static void associate_process(uint8 *pdata, uint16 len);
static void device_leave_process(uint8 *pdata, uint8 len);
static void active_ep_process(uint8 *pdata, uint8 len);
static void ep_describe_process(uint8 *pdata, uint16 len);
static void set_point_num(uint16 addr, uint8 point_num);
static void set_point_count(uint16 addr);

osState zb_permite_join(uint16 addr,uint8 duration, uint8 significance)
{
	osState ret = osFailed;
	uint8	slen;
	uint8 	buf[5];
	osLogI(DBG_MT_ZDO,"zb_permite_join duration:%d ",duration);
	buf[0] = 0x02;//16 bit addr mode
	zb_u16tob(&buf[1],addr);
	buf[3] = duration;
	buf[4] = significance;
	ret = zb_uart_write(MT_RPC_CMD_SREQ|MT_RPC_SYS_ZDO,MT_ZDO_MGMT_PERMIT_JOIN_REQ,buf,5);
	if((slen=zb_uart_read(MT_RPC_CMD_SRSP|MT_RPC_SYS_ZDO,MT_ZDO_MGMT_PERMIT_JOIN_REQ,buf,1))>0)
	{
		if(buf[0]==0x00)
			ret = osSucceed;
	}
	return ret;
}
osState zb_req_read_associate(uint16 addr,uint8 startID)
{
	osState ret;
	uint8 buf[3];
	osLogI(DBG_MT_ZDO,"zb_req_read_associate addr:%04x ",addr);
	zb_u16tob(buf,addr);
	buf[2] = startID;
	ret = zb_io_req_send(MT_RPC_CMD_SREQ|MT_RPC_SYS_ZDO,MT_ZDO_MGMT_LQI_REQ,addr,zb_get_seq(),buf,3,osFalse);
	return ret;
}
static void associate_process(uint8 *pdata, uint16 len)
{
	uint8	*p;
	uint16 	srcAddr,devAddr;
	uint8	count;
	uint8	total;
	uint8	startID;
	if(pdata[2]==osSucceed)
	{
		srcAddr = zb_btou16(pdata);
		total = pdata[3];
		startID = pdata[4];
		count = pdata[5];
		osLogI(DBG_MT_ZDO,"MTZDO_RspLqi:src:%04x, total:%d, startID:%d, count:%d \r\n",srcAddr,total,startID,count);
		startID += count;
		if(startID<total)
		{
			zb_req_read_associate(srcAddr,startID);
		}
		p = &pdata[6];
		for(startID=0;startID<count;startID++)
		{
			devAddr = zb_btou16(&p[16]);
			zb_save_lqi(devAddr,p[21]);
			zb_req_read_associate(devAddr,0x00);
			p+=22;
		}
	}
}
osState zb_req_remove_device(uint16 addr, uint8 *exAddr)
{
	osState ret;
	uint8 buf[11];
	osLogI(DBG_MT_ZDO,"zb_req_remove_device addr:%04x ",addr);
	zb_u16tob(buf,addr);
	osMemcpy(&buf[2],exAddr,8);
	buf[10] = 0x00;
	ret = zb_io_req_send(MT_RPC_CMD_SREQ|MT_RPC_SYS_ZDO,MT_ZDO_MGMT_LEAVE_REQ,addr,zb_get_seq(),buf,3,osFalse);
	return ret;
}
void zb_zdo_message_process(uint8 cmd1,uint8 *pdata, uint8 len)
{
	switch(cmd1)
	{
	case MT_ZDO_STATE_CHANGE_IND://              0xC0
		break;
	case MT_ZDO_END_DEVICE_ANNCE_IND://          0xC1
		device_indication_process(pdata,len);
		break;
	case MT_ZDO_TC_DEVICE_IND:
		break;
	case MT_ZDO_MATCH_DESC_RSP_SENT://           0xC2
		break;
	case MT_ZDO_STATUS_ERROR_RSP://              0xC3
		break;
	case MT_ZDO_SRC_RTG_IND://                   0xC4
		break;
	case MT_ZDO_BEACON_NOTIFY_IND://             0xC5
		break;
	case MT_ZDO_JOIN_CNF://                      0xC6
		break;
	case MT_ZDO_NWK_DISCOVERY_CNF://             0xC7
		break;
	case MT_ZDO_CONCENTRATOR_IND_CB://           0xC8
		break;
	case MT_ZDO_LEAVE_IND://                     0xC9
		device_leave_process(pdata,len);
		break;
	case MT_ZDO_MSG_CB_INCOMING://               0xFF
		break;
	case MT_ZDO_MGMT_LQI_RSP://
		associate_process(pdata,len);
		break;
	case MT_ZDO_SIMPLE_DESC_RSP://0x84
		ep_describe_process(pdata,len);
		break;
	case MT_ZDO_ACTIVE_EP_RSP:// 0x85
		active_ep_process(pdata,len);
		break;

	}
}
static void device_leave_process(uint8 *pdata, uint8 len)
{
	uint16 srcAddr;
	uint16 devAddr;
	uint8 *pmac;
	if(len>=(2+2+8))
	{
		srcAddr = zb_btou16(pdata);
		devAddr = zb_btou16(&pdata[2]);
		pmac = &pdata[4];
		zb_delete_device(devAddr);
		osLogI(DBG_MT_ZDO,"device leave: src:%04x, dev:%04x mac:",srcAddr,devAddr);
		osLogB(DBG_MT_ZDO,NULL,pmac,8);
	}
}
static void device_indication_process(uint8 *pdata, uint8 len)
{
	uint16 	srcAddr;
	uint16 	devAddr;
	uint8 	*pmac;
	uint8	cap;
	if(len>=(2+2+8+1))
	{
		srcAddr = zb_btou16(pdata);
		devAddr = zb_btou16(&pdata[2]);
		pmac = &pdata[4];
		cap = pdata[4+8];
		osLogI(DBG_MT_ZDO,"new device indication: src:%04x, dev:%04x cap:%02x mac:",srcAddr,devAddr,cap);
		osLogB(DBG_MT_ZDO,NULL,pmac,8);
		osLogI((DBG_MT_ZDO&&(cap&0x01)),"coord,");
		osLogI((DBG_MT_ZDO&&(cap&0x02)),"router,");
		osLogI((DBG_MT_ZDO&&(!(cap&0x02))),"end device,");
		osLogI((DBG_MT_ZDO&&(!(cap&0x04))),"main power,");
		osLogI((DBG_MT_ZDO&&(!(cap&0x40))),"encrypt\r\n");
		zdo_add_new_device(devAddr,pmac);
	}
}
static osState req_read_active_ep(uint16 addr)
{
	osState ret;
	uint8 buf[4];
	zb_u16tob(buf,addr);
	zb_u16tob(&buf[2],addr);
	osLogI(DBG_MT_ZDO,"MTZDO_ReqActivePoint: addr:%04x \r\n",addr);
	ret = zb_io_req_send(MT_RPC_CMD_SREQ|MT_RPC_SYS_ZDO,MT_ZDO_ACTIVE_EP_REQ,addr,zb_get_seq(),buf,4,osFalse);
	return ret;
}
static void active_ep_process(uint8 *pdata, uint8 len)
{
	uint8	*ppoint;
	uint16 	srcAddr;
	uint16 	devAddr;
	uint8	pointNum;
	uint8 	i;
	if(pdata[2]==osSucceed)
	{
		srcAddr = zb_btou16(pdata);
		devAddr = zb_btou16(&pdata[3]);
		pointNum = pdata[5];
		ppoint = &pdata[6];
		set_point_num(srcAddr,pointNum);
		osLogI(DBG_MT_ZDO,"active ep: src:%04x, dev:%04x point list:",srcAddr,devAddr);
		osLogB(DBG_MT_ZDO,NULL,ppoint,pointNum);
		for(i=0;i<pointNum;i++)
		{
			req_ep_describe(devAddr,ppoint[i]);
		}
	}
}
static osState req_ep_describe(uint16 addr, uint8 point)
{
	osState ret;
	uint8 buf[5];
	osLogI(DBG_MT_ZDO,"req_ep_describe: addr:%04x point:%d \r\n",addr,point);
	zb_u16tob(buf,addr);
	zb_u16tob(&buf[2],addr);
	buf[4] = point;
	ret = zb_io_req_send(MT_RPC_CMD_SREQ|MT_RPC_SYS_ZDO,MT_ZDO_SIMPLE_DESC_REQ,addr,zb_get_seq(),buf,5,osFalse);
	return ret;
}

static void ep_describe_process(uint8 *pdata, uint16 len)
{
	uint8	*p;
	uint16	*pIncID,*pOutcID;
	uint16 	srcAddr,devAddr;
	uint16 	profileID,devID;
	uint8	devVer;
	uint8	cLen,i,InNum,OutNum;
	uint8	point;
	if(pdata[2]==osSucceed)
	{
		srcAddr = zb_btou16(pdata);
		devAddr = zb_btou16(&pdata[3]);
		cLen = pdata[5];
		point = pdata[6];
		profileID = zb_btou16(&pdata[7]);
		devID = zb_btou16(&pdata[9]);
		devVer = pdata[11];
		p = &pdata[12];
		InNum = *p++;
		pIncID = (uint16*)osMalloc(InNum*2);
		if(pIncID!=NULL)
		{
			for(i=0;i<InNum;i++)
			{
				pIncID[i] = zb_btou16(p);p+=2;
				osLogI(DBG_MT_ZDO,"IncID:%04x \r\n",pIncID[i]);
			}
		}
		OutNum = *p++;
		pOutcID = (uint16*)osMalloc(OutNum*2);
		if(pOutcID!=NULL)
		{
			for(i=0;i<OutNum;i++)
			{
				pOutcID[i] = zb_btou16(p);p+=2;
				osLogI(DBG_MT_ZDO,"outcID:%04x \r\n",pOutcID[i]);
			}
		}
		osLogI(DBG_MT_COMMON,"MTZDO_RspSimpeDesc: src:%04x, dev:%04x, cLen:%d, point:%02x, profileID:%04x, devID:%04x, version:%d, InNum:%02x,OutNum,:%02x \r\n",
				srcAddr,devAddr,cLen,point,profileID,devID,devVer,InNum,OutNum);
		osLogB(DBG_MT_ZDO,"InCluster:",(uint8*)pIncID,InNum*2);
		osLogB(DBG_MT_ZDO,"OutCluster:",(uint8*)pOutcID,OutNum*2);
		p = get_exaddr_by_addr(devAddr);
		if(p!=NULL)
		{
			if(InNum>=OutNum)
			{
				zb_create_port(p,devAddr,point,devID,pIncID,InNum);
			}
			else
			{
				zb_create_port(p,devAddr,point,devID,pOutcID,OutNum);
			}
		}
		else
		{
			osLogI(DBG_MT_ERROR,"create port error, exaddr not found !\r\n");
		}
		set_point_count(devAddr);
		osFree(pIncID);
		osFree(pOutcID);
		osLogI(DBG_MT_ZDO,"create port finished !\r\n");
	}
}
static void zdo_task(int sig, void *arg, int len)
{
	Device_t *pfree;
	switch(sig)
	{
	case 1://new device list poll
		if(pNewDevice!=NULL)
		{
			osLogI(DBG_MT_ZDO,"MTZDO_Task: req dev active point: addr:%04x stry:%d \r\n",pNewDevice->addr,pNewDevice->stry);
			if((pNewDevice->stry==0)||(pNewDevice->point_num==pNewDevice->finish_count))
			{
				pfree = pNewDevice;
				pNewDevice = pNewDevice->next;
				osFree(pfree);
				osSendMessage(zdo_task,1,NULL,0);
			}
			else
			{
				req_read_active_ep(pNewDevice->addr);
				pNewDevice->stry--;
			}
		}
		else
		{
			osDeleteTimer(zdo_task,1);
		}
		break;
	}
}
void zdo_add_new_device(uint16 addr, uint8 *exAddr)
{
	Device_t *pnew,*pcur;
	osLogI(DBG_MT_ZDO,"zdo_add_new_device addr:%04x\r\n",addr);
	if(zb_check_exaddr_exist(exAddr)==osTrue)
	{
		osLogI(DBG_MT_ERROR,"zdo_add_new_device device already exist and delete it !\r\n");
		zb_delete_device_by_exaddr(exAddr);
	}
	pnew = (Device_t*)osMalloc(sizeof(Device_t));
	if(pnew!=NULL)
	{
		pnew->addr = addr;
		if(exAddr!=NULL)
			osMemcpy(pnew->exAddr,exAddr,8);
		pnew->stry = 5;
		pnew->next = NULL;
		pnew->point_num = 0xFF;
		pnew->finish_count = 0;
		if(pNewDevice==NULL)
		{
			pNewDevice = pnew;
			osStartReloadTimer(5000,zdo_task,1,NULL);
			osSendMessage(zdo_task,1,NULL,0);
		}
		else
		{
			pcur = pNewDevice;
			while(pcur->next!=NULL)
				pcur = pcur->next;
			pcur->next = pnew;
		}
	}
}
static void remove_new_device(uint16 addr)
{
	Device_t *pcur;
	pcur = pNewDevice;
	while(pcur!=NULL)
	{
		if(pcur->addr==addr)
		{
			osStartReloadTimer(5000,zdo_task,1,NULL);
			pcur->stry = 0;
		}
		pcur = pcur->next;
	}
	osLogI(DBG_MT_ZDO,"remove_new_device: finished \r\n");
}
static void set_point_num(uint16 addr, uint8 point_num)
{
	Device_t *pcur;
	pcur = pNewDevice;
	while(pcur!=NULL)
	{
		if(pcur->addr==addr)
		{
			osLogI(DBG_MT_ZDO,"set_point_num: addr:%04x \r\n",pcur->addr);
			pcur->point_num = point_num;
			break;
		}
		pcur = pcur->next;
	}
}
static void set_point_count(uint16 addr)
{
	Device_t *pcur;
	pcur = pNewDevice;
	while(pcur!=NULL)
	{
		if(pcur->addr==addr)
		{
			osLogI(DBG_MT_ZDO,"set_point_num: addr:%04x \r\n",pcur->addr);
			pcur->finish_count++;
			break;
		}
		pcur = pcur->next;
	}
}
static uint8 *get_exaddr_by_addr(uint16 addr)
{
	Device_t *pcur;
	pcur = pNewDevice;
	while(pcur!=NULL)
	{
		if(pcur->addr==addr)
		{
			osLogI(DBG_MT_ZDO,"get_exaddr_by_addr: addr:%04x \r\n",pcur->addr);
			return pcur->exAddr;
		}
		pcur = pcur->next;
	}
	osLogI(DBG_MT_ZDO,"get_exaddr_by_addr: finished \r\n");
	return NULL;
}




