/*
 * hal_factory.c
 *
 *  Created on: Feb 26, 2019
 *      Author: lort
 */

#include "osCore.h"
#include "soc/uart_struct.h"
#include "driver/uart.h"
#include "hal_factory.h"

#define DBG_MT_UART						0

#define HAL_UART_SOF_FLAG				0xFE
#define HAL_UART_SOF_FLAG1				0xFF
#define HAL_UART_RX_BUFFER_SIZE			(1024)
#define HAL_UART_MIN_PACKAGE_SIZE		(6)

#define UARTNUM							UART_NUM_0
#define UART_BUFFER_SIZE				(1024)

typedef struct
{
	uint8 SOF;
	uint8 LEN[2];
	uint8 CMD0;
	uint8 CMD1;
	uint8 DATA[0];
}uart_package_t;

typedef struct
{
	uint16 	timeout;
	uint16 	cnt;
	uint8	data[HAL_UART_RX_BUFFER_SIZE];
}uart_rx_t;

static uart_rx_t				uart_rx;
static factory_receive_cb_t		factory_receive_cb = NULL;

static void 					factory_thread(void *arg);

static int 						find_head(uint8 *s, uint16 len, uint16 index);
static int 						check_package(uart_package_t *pkg, int len);
static int 						read_package(uint8 *pbuf, int len);

static uart_package_t 			*package(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len);
static int 						package_length(uart_package_t *pkg);
static int 						package_data_length(uart_package_t *pkg);
static uint8 					get_xor(uint8 *pbuf, int len);


void hal_factory_init(factory_receive_cb_t cb)
{
	factory_receive_cb = cb;
	uart_driver_install(UARTNUM, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 0, NULL, 0);
	xTaskCreate(factory_thread, "factory_thread", 1024*4, NULL, (tskIDLE_PRIORITY + 1), NULL);
}

static void factory_thread(void *arg)
{
	int 	slen;
	uint8 	buf[128];
	uart_rx.cnt = 0;
	while(1)
	{
		if((slen = uart_read_bytes(UARTNUM, buf, 128, 1))>0)
		{
			if((uart_rx.cnt+slen)<UART_BUFFER_SIZE)
			{
				osMemcpy(&uart_rx.data[uart_rx.cnt],buf,slen);
				uart_rx.cnt += slen;
				uart_rx.timeout = 3;
			}
			else
			{
				uart_rx.cnt = 0;
			}
			osLogB(1,"debug uart:receive:",uart_rx.data,uart_rx.cnt);
		}
		else
		{
			osSleep(10);
		}
	}
}
static uint8 FactoryBuffer[1024];
void FUC_ATTR hal_factory_poll(void)
{
	uart_package_t 	*pkg;
	int	slen,dlen;
	pkg = (uart_package_t*)&FactoryBuffer;
	if(pkg!=NULL)
	{
		osLogI(0,"zb_uart_task:cmd0: try to read package now\r\n");
		if((slen = read_package((uint8*)pkg,1024))>0)
		{
			dlen = package_data_length(pkg);
			osLogI(DBG_MT_UART,"zb_uart_thread read package :cmd0:%02x cmd1:%02x ",pkg->CMD0,pkg->CMD1);
			osLogB(DBG_MT_UART,"data:",pkg->DATA,dlen);
			if(factory_receive_cb!=NULL)
			{
				factory_receive_cb(pkg->CMD0,pkg->CMD1,pkg->DATA,dlen);
			}
		}
	}
}
osState FUC_ATTR hal_factory_write(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	uart_package_t 	*pkg;
	uint8			ret = osFailed;
	int				slen;
	osLogI(1,"zb_uart_write:cmd0:%02x cmd1:%02x len:%d \r\n",cmd0,cmd1,len);
	osLogB(DBG_MT_UART,"data:",pdata,len);
	pkg = package(cmd0,cmd1,pdata,len);
	if(pkg!=NULL)
	{
		slen = package_length(pkg);
		osLogB(DBG_MT_UART,"zb_uart_write:",(uint8*)pkg,slen);
		if((uart_write_bytes(UARTNUM,(const char *)pkg,slen))==0)
		{
			ret = osSucceed;
		}
		osFree(pkg);
	}
	return ret;
}
static int FUC_ATTR find_head(uint8 *s, uint16 len, uint16 index)
{
	uint16 i;
	uint16 cnt=0;
	for(i=0;i<len;i++)
	{
		if((s[i]==HAL_UART_SOF_FLAG1)||(s[i]==HAL_UART_SOF_FLAG))
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
static int FUC_ATTR read_package(uint8 *pbuf, int len)
{
	int 				headID = -1;
	int					slen;
	uint8				itry;
	//osEntercritical();
	if(uart_rx.timeout)
	{
		uart_rx.timeout--;
	}
	else if(uart_rx.cnt)
	{
		osLogB(DBG_MT_UART, "message timeout:",uart_rx.data,uart_rx.cnt);
		uart_rx.cnt = 0;
	}
	slen = 0;
	if(uart_rx.cnt>0)
	{
		itry=3;
		while(itry--)
		{
			headID = find_head(uart_rx.data,uart_rx.cnt,0);//find first head index
			if(headID>0)
			{
				uart_rx.cnt -= headID;
				osMemcpy(uart_rx.data,&uart_rx.data[headID],uart_rx.cnt);
			}
			else
				break;
		}
		if(headID==0)
		{
			if((slen = check_package((void*)uart_rx.data,uart_rx.cnt))>0)
			{
				osLogB(DBG_MT_UART,"read_package:",uart_rx.data,slen);
				slen = (slen>len)?len:slen;
				osMemcpy(pbuf,uart_rx.data,slen);
				uart_rx.cnt-=slen;
				osMemcpy(uart_rx.data,&uart_rx.data[slen],uart_rx.cnt);
				osLogI(DBG_MT_UART,"read_package check_package OK: slen:%d len:%d\r\n",slen,len);
			}
		}
	}
	//osExitcritical();
	return slen;
}
static int FUC_ATTR check_package(uart_package_t *pkg, int len)
{
	int slen = 0,dlen;
	uint8 sum;
	osLogB(0,"check_package: ",(void*)pkg,len);
	if(len>=HAL_UART_MIN_PACKAGE_SIZE)
	{
		slen = package_length(pkg);
		if(slen<=len)
		{
			osLogI(DBG_MT_UART,"check_package: slen:%d len:%d \r\n",slen,len);
			if(pkg->SOF==HAL_UART_SOF_FLAG)
			{
				dlen = package_data_length(pkg);
				sum = get_xor(&pkg->LEN[0],(slen-2));
				if(sum==pkg->DATA[dlen])
					return (int)slen;
				else
				{
					osLogI(DBG_MT_UART,"check pacakge: sum error msg sum:%02x cal:%02x \r\n",pkg->DATA[dlen],sum);
				}
			}
			else if(pkg->SOF==HAL_UART_SOF_FLAG1)
			{
				return (int)slen;
			}
		}

	}
	return 0;
}

static uart_package_t FUC_ATTR *package(uint8 cmd0, uint8 cmd1, uint8 *pdata, int len)
{
	uart_package_t 	*pkg;
	int 	slen;
	slen = len + HAL_UART_MIN_PACKAGE_SIZE;
	pkg = (uart_package_t*)osMalloc(slen);
	if(pkg!=NULL)
	{
		pkg->SOF = HAL_UART_SOF_FLAG;
		pkg->CMD0 = cmd0;
		pkg->CMD1 = cmd1;
		pkg->LEN[0] = (uint8)(len>>8);
		pkg->LEN[1] = (uint8)len;
		if((pdata!=NULL)&&(len!=0))
			osMemcpy(pkg->DATA,pdata,len);
		pkg->DATA[len] = get_xor(&pkg->LEN[0],(slen-2));
		return pkg;
	}
	return NULL;
}
static int FUC_ATTR package_length(uart_package_t *pkg)
{
	int 	len;
	uint16 	temp;
	temp = pkg->LEN[0];
	temp <<= 8;
	temp |= pkg->LEN[1];
	len = temp+HAL_UART_MIN_PACKAGE_SIZE;
	return len;
}
static int FUC_ATTR package_data_length(uart_package_t *pkg)
{
	uint16 	temp;
	temp = pkg->LEN[0];
	temp <<= 8;
	temp |= pkg->LEN[1];
	return (int)temp;
}
static uint8 FUC_ATTR get_xor(uint8 *pbuf, int len)
{
	int i;
	uint8 sum;
	sum = 0;
	for(i=0;i<len;i++)
	{
		sum ^= pbuf[i];
	}
	return sum;
}
