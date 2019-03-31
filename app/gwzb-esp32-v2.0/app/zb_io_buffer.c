/*
 * zb_io_buffer.c
 *
 *  Created on: 2017��7��11��
 *      Author: lort
 */
#include "osCore.h"
#include "zb_common.h"
#include "zb_uart.h"
#include "zb_zdo.h"
#include "zb_zcl.h"
#include "zb_io_buffer.h"

typedef struct
{
	void 	*next;
	uint8	stry;
	uint8	ack;
	uint8 	cmd0;
	uint8 	cmd1;
	uint16 	addr;
	uint8	seq;
	uint8 	len;
	uint8 	pdata[0];
}output_t;

static output_t	*pSend = NULL;
static uint8	active_task = osFalse;
static void add_to_send_list(output_t *psnd);
static void io_task(int sig, void *arg, int len);
static void receive_message_cb(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);

osState zb_io_init(void)
{
	osLogI(DBG_MT_APP,"zb_io_init\r\n");
	return zb_uart_init(receive_message_cb);
}
osState zb_io_req_send(uint8  cmd0, uint8 cmd1,uint16 addr, uint8 seq, uint8 *pdata, uint8 len, uint8 req_ack)
{
	output_t *pnew;
	osLogI(0,"zb_io_req_send, addr :%04x, seq:%02x \r\n",addr,seq);
	pnew = (output_t*)osMalloc(sizeof(output_t)+len);
	if(pnew!=NULL)
	{
		pnew->addr = addr;
		if(req_ack)
			pnew->stry = IO_SEND_TRY;
		else
			pnew->stry = 1;
		pnew->ack = req_ack;
		pnew->cmd0 = cmd0;
		pnew->cmd1 = cmd1;
		pnew->len = len;
		pnew->next = NULL;
		pnew->seq = seq;
		if((len>0)&&(pdata!=NULL))
		{
			osMemcpy(pnew->pdata,pdata,len);
		}
		add_to_send_list(pnew);
		return osSucceed;
	}
	return osFailed;
}
osState zb_io_remove_send(uint16 addr, uint8 seq, uint8 same_addr)
{
	output_t *pcur,*pre;
	uint8 ret = osFailed;
	osLogI(DBG_MT_APP,"zb_io_remove_send addr :%04x, seq:%02x \r\n",addr,seq);
	pre = pcur = pSend;
	while(pcur!=NULL)
	{
		if(pcur->addr==addr)
		{
			if((same_addr==osTrue)||(seq==pcur->seq))
			{
				osLogI(DBG_MT_APP,"zb_io_remove_send addr :%04x, seq:%02x  try:%d \r\n",addr,pcur->seq,pcur->stry);
				ret = osSucceed;
				if(pcur==pre)
				{
					pSend = pSend->next;
					osFree(pcur);
					pre = pcur = pSend;
					continue;
				}
				else
				{
					pre->next = pcur->next;
					osFree(pcur);
					pcur = pre->next;
					continue;
				}
			}
		}
		pre = pcur;
		pcur = pcur->next;
	}
	if(ret==osSucceed)
	{
		osStartReloadTimer(IO_DELAY_SEND,io_task,1,NULL);
	}
	return ret;
}
static void receive_message_cb(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
	osLogI(DBG_MT_APP,"receive_message_cb cmd0:%02x, cmd1:%02x, len:%d \r\n",cmd0,cmd1,len);
	if((cmd0&MT_RPC_SUBSYSTEM_MASK)==MT_RPC_SYS_SYS)
	{

	}
	else if(cmd0==(MT_RPC_SYS_ZDO|MT_RPC_CMD_AREQ))
	{
		zb_zdo_message_process(cmd1,pdata,len);
	}
	else if(cmd0==(MT_RPC_SYS_APP|MT_RPC_CMD_AREQ))
	{
		zb_zcl_message_process(cmd1,pdata,len);
	}
	else if(cmd0==(MT_RPC_SYS_APP|MT_RPC_CMD_SRSP))
	{
		//app response
	}
	else if(cmd0==(MT_RPC_SYS_ZDO|MT_RPC_CMD_SRSP))
	{
		//zdo response
	}
	else
	{
		osLogE(DBG_MT_ERROR,"unkonw zb command cmd0:%02x, cmd1:%02x, len:%d \r\n",cmd0,cmd1,len);
	}
}
static void io_task(int sig, void *arg, int len)
{
	output_t *pcur;
	osLogI(DBG_MT_APP,"io_task: \r\n");
	pcur = pSend;
	if(pcur!=NULL)
	{
		if(pcur->stry>0)
		{
			osLogI(DBG_MT_APP,"io_task: send try:%d at:%d \r\n",pcur->stry,osGetmsTimestamp());
			osLogB(DBG_MT_APP,NULL,pcur->pdata,pcur->len);
			pcur->stry--;
			zb_uart_write(pcur->cmd0,pcur->cmd1,pcur->pdata,pcur->len);
		}
		else if(pcur->stry==0)
		{
			osLogI(DBG_MT_APP,"io_task: send try:%d free \r\n",pcur->stry);
			if(pcur->ack==osTrue)//if req ack message send failed and remove all target message anyway
				zb_io_remove_send(pcur->addr,pcur->seq,osTrue);
			else
				zb_io_remove_send(pcur->addr,pcur->seq,osFalse);
		}
		osStartReloadTimer(IO_SEND_TIMEOUT,io_task,1,NULL);
	}
	else
	{
		osLogI(DBG_MT_APP,"io_task: end task \r\n");
		osDeleteTimer(io_task,1);
		active_task = osFalse;
	}
	osLogI(DBG_MT_APP,"io_task: end \r\n");
}
static void add_to_send_list(output_t *msg)
{
	output_t *pcur;
	osLogI(DBG_MT_APP,"add_to_send_list, addr :%04x, seq:%02x \r\n",msg->addr,msg->seq);
	pcur = pSend;
	if(pcur==NULL)
	{
		pSend = msg;
	}
	else
	{
		while(pcur->next!=NULL)
			pcur = pcur->next;
		pcur->next = msg;
	}
	if(active_task == osFalse)
	{
		osStartReloadTimer(30,io_task,1,NULL);
		active_task = osTrue;
	}
}



