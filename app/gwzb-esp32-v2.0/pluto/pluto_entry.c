/*
 * pluto_service.c
 *
 *  Created on: 2017��8��31��
 *      Author: lort
 */

#include "osCore.h"
#include "pluto.h"
#include "tcp_socket.h"
#include "udp_socket.h"
#include "pluto_entry.h"

const static nwk_adapter_t myadapter={
		.udp_open = udp_socket_init,
		.tcp_open = tcp_socket_init,
		.udp_close = udp_socket_deinit,
		.tcp_close = tcp_socket_deinit,
		.udp_output = udp_socket_send,
		.tcp_output = tcp_socket_send
};

static led_blink_cb_t blink_led=NULL;
static void pluto_task(int sig, void *arg,int len);
void FUC_ATTR pluto_init(void)
{
	socket_init((nwk_adapter_t*)&myadapter);
	pluto_pre_init();
	osStartReloadTimer(100,pluto_task,1,NULL);
}

static void FUC_ATTR pluto_task(int sig, void *arg,int len)
{
	tcpip_adapter_ip_info_t 	ipconfig;
	osLogD(0,"pluto_task\r\n");
    tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ipconfig);
	if ((tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ipconfig)==ESP_OK)&& ipconfig.ip.addr != 0)
	{
		pluto_start_server(ipconfig.ip.addr);
		osDeleteTimer(pluto_task,1);
	}
}
void FUC_ATTR pluto_registe_led(led_blink_cb_t cb)
{
	blink_led = cb;
}
void FUC_ATTR pluto_led_blink(int num, int htime, int ltime)
{
	if(blink_led!=NULL)
	{
		blink_led(num,htime,ltime);
	}
}
