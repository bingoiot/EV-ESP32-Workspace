/*
 * udp_socket.c
 *
 *  Created on: Jul 22, 2016
 *      Author: lort
 */

#include "osCore.h"
#include "udp_socket.h"
#include "external_include.h"
#include "pluto.h"
#include "pluto_entry.h"

static uint8	udp_task_init = osFalse;
static int 		udp_socket_fd = -1;
static struct 	sockaddr_in udp_socket;
static uint8 	pudp_socket_buf[defUdpSocketBufferSize];//receive buffer

static void  udp_create_socket(void);
void udp_socket_thread_rec(void *arg);
struct udp_send_nwk_t
{
	uint32	srcIP;
	uint16	srcPort;
	int		len;
	uint8 	pdata[0];
};

int  udp_socket_init(uint16 port)
{
	osLogI(DBG_UDP_SOC,"udp_socket_init: port:%d \r\n",os_ntohs(port));
	osMemset(&udp_socket,0,sizeof(struct sockaddr_in));
	udp_socket.sin_family = AF_INET;
	udp_socket.sin_addr.s_addr = INADDR_ANY;
	udp_socket.sin_port = port;
	//udp_socket.sin_len = sizeof(udp_socket);
	if(udp_task_init==osFalse)
	{
		xTaskCreate(udp_socket_thread_rec, "udp_socket_thread_rec", 4096, NULL, (tskIDLE_PRIORITY + 3), NULL);
		udp_task_init = osTrue;
	}
	udp_create_socket();
	return 0;
}
int udp_socket_deinit(void)
{
	/*if(udp_socket_fd>0)
	{
		shutdown(udp_socket_fd,SHUT_RDWR);
		close(udp_socket_fd);
		udp_socket_fd = -1;
	}*/
	return osSucceed;
}
static void  udp_create_socket(void)
{
	int ret;
	int so_br = 1;
	osLogI(DBG_UDP_SOC,"udp_socket_create_socket :got IP \r\n");
	if(udp_socket_fd<0)
	{
		udp_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
		if(udp_socket_fd<0)
		{
			osLogI(DBG_UDP_SOC,"udp_socket_create_socket creat socket_fd error \r\n");
			pluto_reset_system(3000);
			return;
		}
		else
			setsockopt(udp_socket_fd,SOL_SOCKET,SO_BROADCAST,&so_br,sizeof(so_br));
		osLogI(DBG_UDP_SOC,"udp_socket_create_socket :create socket fd \r\n");
		ret = bind(udp_socket_fd,(struct sockaddr *)&(udp_socket),sizeof(udp_socket));
		if(ret==-1)
		{
			osLogI(DBG_UDP_SOC,"udp_socket_create_socket bind socket_fd error \r\n");
			udp_socket_deinit();
		}
	}
	osLogI(DBG_UDP_SOC,"udp_socket_create_socket :binded socket finished\r\n");
}
void  udp_socket_thread_rec(void *arg)
{
	struct sockaddr_in fromSocket;
	socklen_t 	fromSlen = sizeof(fromSocket);
	uint32 ip;
	char *pip;
	uint16 port;
	int ret;
	while(1)
	{
		//ret = recvfrom(udp_socket_fd,pudp_socket_buf,defUdpSocketBufferSize,0,(struct sockaddr *)&fromSocket,&fromSlen);
		if(udp_socket_fd>=0)
		{
			ret = recvfrom(udp_socket_fd,pudp_socket_buf,defUdpSocketBufferSize,0,(struct sockaddr *)&fromSocket,&fromSlen);
			if(ret>0)
			{
				if(login_get_state()==login_state_online)
					pluto_led_blink(1,10,0);
				port = (uint16)fromSocket.sin_port;//ntohs(fromSocket.sin_port);
				ip = (uint32)fromSocket.sin_addr.s_addr;//ntohl(fromSocket.sin_addr);
				pip = (char*)inet_ntoa(fromSocket.sin_addr);
				osLogI(DBG_UDP_SOC,"udp_socket_thread_rec from :%s:%d  at timestamp: %d \r\n",pip,ntohs(port),osGetmsTimestamp());
				osLogB(DBG_UDP_SOC,NULL,pudp_socket_buf,ret);
				socket_udp_input(ip,port,pudp_socket_buf,ret);
			}
			else
			{
				osLogI(DBG_ERROR,"udp_socket_thread_rec: error occur !\r\n ");
				udp_socket_deinit();
				udp_create_socket();
			}
		}
		else
			osSleep(100);
		//osLogI("udp_socket_thread_rec",NULL,0);
	}
}
int  udp_socket_send(uint32 ip, uint16 port,uint8 *pdata, uint16 len)
{
	struct sockaddr_in mysocket;
	int addrlen = sizeof(mysocket);
	char *pip;
	//jf_entercriticalSection();
	if(udp_socket_fd>=0)
	{
		if(login_get_state()==login_state_online)
			pluto_led_blink(1,10,0);
		osMemset(&mysocket,0,sizeof(struct sockaddr_in));
		mysocket.sin_family = AF_INET;
		mysocket.sin_addr.s_addr = ip;//htonl(ip);//;
		mysocket.sin_port = port;//htons(port);
		pip = (char*)inet_ntoa(mysocket.sin_addr);
		osLogI(DBG_UDP_SOC,"udp_socket_send to :%s:%d  at timestamp: %d \r\n",pip,ntohs(port),osGetmsTimestamp());
		osLogB(DBG_UDP_SOC,NULL,pdata,len);
		if(sendto(udp_socket_fd,pdata,len,0,(struct sockaddr *)&mysocket,addrlen)==0)
		{
			//jf_exitcriticalSection();
			return 0;
		}
	}
	//jf_exitcriticalSection();
	return -1;
}







