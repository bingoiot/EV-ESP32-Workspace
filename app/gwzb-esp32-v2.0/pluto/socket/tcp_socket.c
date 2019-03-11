/*
 * tcp_socket.c
 *
 *  Created on: 2017��5��4��
 *      Author: lort
 */

#include "osCore.h"
#include "tcp_socket.h"
#include "external_include.h"
#include "pluto.h"
#include "pluto_entry.h"

struct tcp_send_nwk_t
{
	uint32	srcIP;
	uint16	srcPort;
	int		len;
	uint8 	pdata[0];
};

static uint8	tcp_task_init = osFalse;
static int 		tcp_socket_fd = -1;
static struct 	sockaddr_in mysocket;
static uint8 	socket_buf[defTcpSocketBufferSize];//receive buffer

static void FUC_ATTR tcp_create_socket(void);
static void tcp_socket_thread_rec(void *arg);
struct udp_send_nwk_t
{
	uint32	srcIP;
	uint16	srcPort;
	int		len;
	uint8 	pdata[0];
};

int FUC_ATTR tcp_socket_init(uint32 ip, uint16 port)
{
	osLogI(DBG_TCP_SOC,"tcp_socket_init: port:%d \r\n",os_ntohs(port));
	osMemset(&mysocket,0,sizeof(struct sockaddr_in));
	mysocket.sin_family = AF_INET;
	mysocket.sin_addr.s_addr = ip;
	mysocket.sin_port = port;
	//udp_socket.sin_len = sizeof(udp_socket);
	if(tcp_task_init==osFalse)
	{
		xTaskCreate(tcp_socket_thread_rec, "tcp_socket_thread_rec", 4096, NULL, (tskIDLE_PRIORITY + 3), NULL);
		tcp_task_init = osTrue;
	}
	tcp_create_socket();
	return 0;
}
osState FUC_ATTR tcp_socket_deinit(void)
{
	if(tcp_socket_fd>0)
	{
		shutdown(tcp_socket_fd,SHUT_RDWR);
		close(tcp_socket_fd);
		tcp_socket_fd = -1;
	}
	return osSucceed;
}
static void FUC_ATTR tcp_create_socket(void)
{
	int ret = -1;
	osLogI(DBG_TCP_SOC,"tcp_create_socket :got IP \r\n");
	tcp_socket_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(tcp_socket_fd<0)
	{
		osLogI(DBG_TCP_SOC,"tcp_create_socket :create socket fd error :%d\r\n",tcp_socket_fd);
		pluto_reset_system(3000);
		return;
	}
	osLogI(DBG_TCP_SOC,"tcp_create_socket :create socket fd :%d \r\n",tcp_socket_fd);
	ret = connect(tcp_socket_fd,(struct sockaddr *)&(mysocket),sizeof(mysocket));
	if(ret<0)
	{
		osLogI(DBG_TCP_SOC,"tcp_create_socket tcp connect error fd:%d \r\n",tcp_socket_fd);
		tcp_socket_deinit();
	}
}
static void FUC_ATTR tcp_socket_thread_rec(void *arg)
{
	int ret;
	while(1)
	{
		//ret = recvfrom(udp_socket_fd,pudp_socket_buf,defTcpSocketBufferSize,0,(struct sockaddr *)&fromSocket,&fromSlen);
		if(tcp_socket_fd>=0)
		{
			ret = recv(tcp_socket_fd,socket_buf,defTcpSocketBufferSize,0);
			if(ret>0)
			{
				if(login_get_state()==login_state_online)
					pluto_led_blink(1,10,0);
				osLogB(DBG_TCP_SOC,NULL,socket_buf,ret);
				socket_tcp_input(socket_buf,ret);
			}
			else
			{
				osLogI(DBG_ERROR,"tcp_socket_thread_rec: error occur !\r\n ");
				tcp_socket_deinit();
				tcp_create_socket();
			}
		}
		else
		{
			osSleep(100);
		}
		//osLogI("udp_socket_thread_rec",NULL,0);
	}
}
int FUC_ATTR tcp_socket_send(uint8 *pdata, uint16 len)
{
	osLogI(DBG_TCP_SOC,"tcp_socketSend to :  at timestamp: %d \r\n",osGetmsTimestamp());
	osLogB(1,NULL,pdata,len);
	if(tcp_socket_fd>=0)
	{
		if(login_get_state()==login_state_online)
			pluto_led_blink(1,10,0);
		if(send(tcp_socket_fd,pdata,len,0)==0)
			return 0;
	}
	else
	{
		osLogI(DBG_ERROR,"tcp_socketSend: socket do not connect !\r\n ");
	}
	return -1;
}


