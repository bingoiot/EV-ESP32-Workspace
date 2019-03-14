/*
 * zb_uart.c
 *
 *  Created on: 2017��7��10��
 *      Author: lort
 */

#include "osCore.h"
#include "zb_common.h"
#include "zb_uart.h"
#include "hal_uart.h"

#define ZB_SOF_FLAG				0xFE
#define ZB_SOF_FLAG1			0xFF
#define ZB_RX_BUFFER_SIZE		(512)
#define ZB_MIN_PACKAGE_SIZE		(sizeof(zb_uart_package_t)+1)

typedef struct
{
	uint8 SOF;
	uint8 LEN;
	uint8 CMD0;
	uint8 CMD1;
	uint8 DATA[0];
}zb_uart_package_t;

typedef struct
{
	uint16 	timeout;
	uint16 	cnt;
	uint8	data[ZB_RX_BUFFER_SIZE];
}zb_rx_t;

static zb_uart_receive_cb_t		zb_recieve_cb = NULL;
static zb_rx_t					zb_rx;
static uint8					zb_rx_lock = osFalse;

static void 					zb_uart_task(int sig, void *arg, int len);
static int 						find_head(uint8 *s, uint16 len, uint16 index);
static uint8 					check_package(zb_uart_package_t *pkg, uint16 len);
static uint8 					read_package(uint8 *pbuf, uint8 len);

static zb_uart_package_t 		*package(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len);
static uint8 					package_length(zb_uart_package_t *pkg);

osState zb_uart_init(zb_uart_receive_cb_t cb)
{
	static uint8 init_flag = 0x00;
	if(init_flag==0x00)
	{
		osStartReloadTimer(20,zb_uart_task,1,NULL);
		hal_uart_init();
		init_flag = 0x01;
	}
	osLogI(DBG_MT_UART,"init uart\r\n");
	zb_recieve_cb = cb;
	return osSucceed;
}

osState zb_uart_write(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
	zb_uart_package_t 	*pkg;
	uint8				ret = osFailed;
	uint8				slen;
	osLogI(DBG_MT_UART,"zb_uart_write:cmd0:%02x cmd1:%02x ",cmd0,cmd1);
	osLogB(DBG_MT_UART,"data:",pdata,len);
	pkg = package(cmd0,cmd1,pdata,len);
	if(pkg!=NULL)
	{
		slen = package_length(pkg);
		osLogB(DBG_MT_COMMON,"zb_uart_write:",(uint8*)pkg,slen);
		if((hal_uart_write((uint8 *)pkg,slen))>0)
		{
			ret = osSucceed;
		}
		osFree(pkg);
	}
	return ret;
}
uint8 zb_uart_read(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
	uint8 buf[255];
	zb_uart_package_t 	*pkg = (zb_uart_package_t*)buf;
	uint8				slen = 0;
	uint8 stry = 60;
	osLogI(1,"zb_uart_read at :cmd0:%02x cmd1:%02x \r\n",cmd0,cmd1);
	zb_rx_lock = osTrue;
	while(stry>0)
	{
		if((slen = read_package(buf,255))>0)
		{
			if((pkg->CMD0==cmd0)&&(pkg->CMD1==cmd1))
			{
				if(len<pkg->LEN)
					slen = len;
				else
					slen = pkg->LEN;
				osMemcpy(pdata,pkg->DATA,slen);
				break;
			}
			else
			{
				if(zb_recieve_cb!=NULL)
				{
					zb_recieve_cb(pkg->CMD0,pkg->CMD1,pkg->DATA,pkg->LEN);
				}
			}
		}
		else
		{
			osSleep(10);
		}
		stry--;
	}
	zb_rx_lock = osFalse;
	return slen;
}
static void zb_uart_task(int sig, void *arg, int len)
{
	uint8 buf[255];
	zb_uart_package_t 	*pkg = (zb_uart_package_t*)buf;
	uint8	slen;
	if(zb_rx_lock==osFalse)
	{
		osLogI(0,"zb_uart_task:cmd0: try to read package now\r\n");
		if((slen = read_package(buf,255))>0)
		{
			osLogI(DBG_MT_UART,"zb_uart_thread read package :cmd0:%02x cmd1:%02x ",pkg->CMD0,pkg->CMD1);
			osLogB(DBG_MT_UART,"data:",pkg->DATA,pkg->LEN);
			if(zb_recieve_cb!=NULL)
			{
				zb_recieve_cb(pkg->CMD0,pkg->CMD1,pkg->DATA,pkg->LEN);
			}
		}
	}
}
static int find_head(uint8 *s, uint16 len, uint16 index)
{
	uint16 i;
	uint16 cnt=0;
	for(i=0;i<len;i++)
	{
		if((s[i]==ZB_SOF_FLAG1)||(s[i]==ZB_SOF_FLAG))
		{
			if(cnt>=index)
			{
				return i;
			}
			cnt++;
		}
	}
	return -1;
}
static uint8 read_package(uint8 *pbuf, uint8 len)
{
	zb_uart_package_t	*pkg = (zb_uart_package_t*)pbuf;
	int 				headID = -1;
	uint16				slen;
	uint16 				rlen;
	uint8				itry;
	uint8				buf[128];
	//DBG_vPrintf(TRUE, "hal_uartRead4: \r\n ");
	while((slen = hal_uart_read(buf,128,0))>0)
	{
		//osEntercritical();
		if(ZB_RX_BUFFER_SIZE<(zb_rx.cnt+slen))
			zb_rx.cnt = 0;
		rlen = ZB_RX_BUFFER_SIZE - zb_rx.cnt;//ʣ��ռ�
		if(rlen>0)
		{
			if(rlen>=ZB_RX_BUFFER_SIZE)//error occur
			{
				rlen = ZB_RX_BUFFER_SIZE;
				zb_rx.cnt = 0;
			}
			osMemcpy(&zb_rx.data[zb_rx.cnt],buf,slen);
			zb_rx.cnt += slen;
			zb_rx.timeout = 3;
		}
		//osExitcritical();
		osLogB(0,"read_package:",zb_rx.data,zb_rx.cnt);
	}
	//osEntercritical();
	if(zb_rx.timeout)
	{
		zb_rx.timeout--;
	}
	else if(zb_rx.cnt)
	{
		osLogB(DBG_MT_ERROR, "message timeout:",zb_rx.data,zb_rx.cnt);
		zb_rx.cnt = 0;
	}
	slen = 0;
	if(zb_rx.cnt>0)
	{
		itry=3;
		while(itry--)
		{
			headID = find_head(zb_rx.data,zb_rx.cnt,0);//find first head index
			if(headID>0)
			{
				zb_rx.cnt -= headID;
				osMemcpy(zb_rx.data,&zb_rx.data[headID],zb_rx.cnt);
			}
			else
				break;
		}
		if(headID==0)
		{
			if((slen = check_package((void*)zb_rx.data,zb_rx.cnt))>0)
			{
				osLogB(DBG_MT_COMMON,"read_package:",zb_rx.data,slen);
				slen = (slen>len)?len:slen;
				osMemcpy(pkg,zb_rx.data,slen);
				zb_rx.cnt-=slen;
				osMemcpy(zb_rx.data,&zb_rx.data[slen],zb_rx.cnt);
			}
		}
	}
	//osExitcritical();
	return slen;
}
static uint8 check_package(zb_uart_package_t *pkg, uint16 len)
{
	uint16 slen = 0;
	uint8 sum;
	osLogB(0,"check_package: ",(void*)pkg,len);
	if(len>ZB_MIN_PACKAGE_SIZE)
	{
		slen = package_length(pkg);
		if(slen<=len)
		{
			osLogI(0,"check_package: slen:%d len:%d \r\n",slen,len);
			if(pkg->SOF==ZB_SOF_FLAG)
			{
				sum = osGetXOR(&pkg->LEN,(slen-2));
				if(sum==pkg->DATA[pkg->LEN])
					return (uint8)slen;
				else
					osLogI(DBG_MT_ERROR,"check pacakge: sum error msg sum:%02x cal:%02x \r\n",pkg->DATA[pkg->LEN],sum);
			}
			else if(pkg->SOF==ZB_SOF_FLAG1)
			{
				return (uint8)slen;
			}
		}

	}
	return 0;
}

static zb_uart_package_t *package(uint8 cmd0, uint8 cmd1, uint8 *pdata, uint8 len)
{
	zb_uart_package_t 	*pkg;
	uint16 	slen;
	slen = len + ZB_MIN_PACKAGE_SIZE;
	pkg = (zb_uart_package_t*)osMalloc(slen);
	if(pkg!=NULL)
	{
		pkg->SOF = ZB_SOF_FLAG;
		pkg->CMD0 = cmd0;
		pkg->CMD1 = cmd1;
		pkg->LEN = len;
		if((pdata!=NULL)&&(len!=0x00))
			osMemcpy(pkg->DATA,pdata,len);
		pkg->DATA[len] = osGetXOR(&pkg->LEN,(slen-2));
		return pkg;
	}
	return NULL;
}
static uint8 package_length(zb_uart_package_t *pkg)
{
	uint8 len;
	len = pkg->LEN+ZB_MIN_PACKAGE_SIZE;
	return len;
}



