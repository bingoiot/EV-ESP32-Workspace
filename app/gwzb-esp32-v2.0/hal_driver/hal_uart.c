/*
 * hal_uart.c
 *
 *  Created on: 2017��7��24��
 *      Author: lort
 */

#include "osCore.h"
#include "external_include.h"
#include "hal_conf.h"
#include "hal_uart.h"
#include "soc/uart_struct.h"
#include "driver/uart.h"


#define ECHO_TEST_TXD  		(22)//4
#define ECHO_TEST_RXD  		(21)//5
#define UARTNUM				UART_NUM_1
#define UART_BUFFER_SIZE	(1024)

static portMUX_TYPE 	uart_mutex;
static void  uart_open(void);
static void  uart_thread(void *arg);
static uint8	RxBuffer[UART_BUFFER_SIZE];
static int		RxCnt = 0;

void hal_uart_init(void)
{
	vPortCPUInitializeMutex(&uart_mutex);
	xTaskCreate(uart_thread, "jf_thread", 1024*4, NULL, (tskIDLE_PRIORITY + 1), NULL);
}
static void  uart_open(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 64,
    };
    //Configure UART1 parameters
    uart_param_config(UARTNUM, &uart_config);
    //Set UART1 pins(TX: IO4, RX: I05, RTS: IO18, CTS: IO19)
    uart_set_pin(UARTNUM, ECHO_TEST_TXD, ECHO_TEST_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    //Install UART driver (we don't need an event queue here)
    //In this example we don't even use a buffer for sending data.
    uart_driver_install(UARTNUM, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 0, NULL, 0);
}
void hal_uart_set_baud(uint32 b)
{
	osLogI(1,"hal_uartSetBaud :%d \r\n",b);
    uart_config_t uart_config = {
        .baud_rate = b,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 64,
    };
    //Configure UART1 parameters
    uart_param_config(UARTNUM, &uart_config);
}
static void uart_thread(void *arg)
{
	int 	slen;
	uint8 	buf[128];
	uart_open();
	hal_uart_set_baud(115200);
	RxCnt = 0;
	while(1)
	{
		if((slen = uart_read_bytes(UARTNUM, buf, 128, 1))>0)
		{
			portENTER_CRITICAL(&uart_mutex);
			if((RxCnt+slen)<UART_BUFFER_SIZE)
			{
				osMemcpy(&RxBuffer[RxCnt],buf,slen);
				RxCnt += slen;
			}
			else
			{
				RxCnt = 0;
			}
			portEXIT_CRITICAL(&uart_mutex);
			osLogB(0,"uart_thread:receive:",RxBuffer,RxCnt);
		}
		osSleep(20);
	}
}
uint16 	hal_uart_read(uint8 *pbuf, uint16 len, uint16 wait)
{
	int slen;
	slen = 0;
	portENTER_CRITICAL(&uart_mutex);
	if(RxCnt>0)
	{
		slen = (RxCnt>len)?len:RxCnt;
		osMemcpy(pbuf,RxBuffer,slen);
		RxCnt -= slen;
		osMemcpy(RxBuffer,&RxBuffer[slen],RxCnt);
	}
	portEXIT_CRITICAL(&uart_mutex);
	if(slen)
		osLogB(0,"hal_uart_read:",pbuf,slen);
	return slen;
}
uint16 	hal_uart_write(uint8 *pdata, uint16 len)
{
	int ret;
	osLogB(0,"hal_uart_write:",pdata,len);
	ret = uart_write_bytes(UARTNUM,(const char*)pdata,len);
	return ret;
}


/*
osState  hal_uartSend(uint8 *pdata, int len)
{
	//osLogI(DBG_HAL,"hal_uartSend:at:%d \r\n",osGetmsTimestamp());
	//osLogB(1,"hal_uartSend:",(uint8*)pdata,len);
	uart_write_bytes(UARTNUM, (const char*) pdata, len);
	return osSucceed;
}

uint16 hal_uartSendPkg(uint8 type, uint8 cmd, uint8 *pdata, uint16 len)
{
	UartMsg_t 	*pmsg;
	uint16 		slen;
	slen = sizeof(UartMsg_t)+1;
	slen += len;
	pmsg = (UartMsg_t*)osMalloc(slen);
	if(pmsg!=NULL)
	{
		pmsg->head = 0xFE;
		pmsg->dlen = len;
		pmsg->type = type;
		pmsg->cmd = cmd;
		osMemcpy(pmsg->pdata,pdata,len);
		pmsg->pdata[len] = osGetXOR(&pmsg->dlen,slen-2);
		osLogB(DBG_MT_UART,"MT_SerialSend:",(uint8*)pmsg,slen);
		uart_write_bytes(UARTNUM,(const char*)pmsg,slen);
		osFree(pmsg);
		return osSucceed;
	}
	return osFailed;
}


*/

