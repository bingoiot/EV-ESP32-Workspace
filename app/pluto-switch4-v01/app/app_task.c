/*
 * app_task.c
 *
 *  Created on: 2017��3��18��
 *      Author: lort
 */

#include "osCore.h"
#include "cJSON.h"
#include "pluto.h"
#include "pluto_entry.h"
#include "pluto_file_system.h"
#include "aID.h"
#include "hal_key.h"
#include "hal_led.h"
#include "factory_settings.h"
#include "app_task.h"
#include "aID.h"

static void app_login_state_cb (uint8 state);
static void factory_reset(int sig, void *arg, int len);
static void pluto_recieve_message_cb(address_t *src,uint8 seq, command_t *cmd, uint8 *pdata,uint32 len);

const uint32 device_attribute_id[] = {
	aID_PDO_Type_Device_Info,					 //
	aID_PDO_Type_Port_List,
	aID_PDO_Type_Port_Describe,
	aID_PDO_Type_Scene,
	aID_PDO_Type_File,
	aID_PDO_Type_White_Table,
	aID_PDO_Type_Alarm_Record,
	aID_PDO_Type_History_Record,
	aID_PDO_Type_Beacon,
	aID_PDO_Type_Device_Indication,
	aID_PDO_Type_Upgrade,
	aID_Gen_Type_LQI,
};
#define DEVICE_ATTRIBUTE_ID_NUM	(sizeof(device_attribute_id)/sizeof(device_attribute_id[0]))
const device_describe_t device_describe =
{
		.version = 1,
		.MFG = "2017-11-8 15:00:00",
		.bin_name = "ESP32.bin",
		.device_name = "switch",
		.device_type = dev_type_device,
		.guestLock= osFalse,
		.shareLock= osFalse,
		.localLock= osFalse,
		.remoteLock= osFalse,
		.zone = 8,
		.aIDNum = DEVICE_ATTRIBUTE_ID_NUM,
		.aID = (uint32*)device_attribute_id
};

const uint32 port_attribute_id[] = {aID_Gen_Type_Switch,aID_Gen_Type_LQI};
#define PORT_ATTRIBUTE_ID_NUM	(sizeof(port_attribute_id)/sizeof(port_attribute_id[0]))

const uint8 addr[]={0x01,0x56,0x05,0x00,0x01,0x00,0x00,0x07};
const uint8 sn[]={0xA3,0xD8,0x72,0x3B,0xAD,0x88,0x52,0xBE,0x2D,0x4C,0x90,0x91,0xC0,0xF6,0xB3,0xDC};
const pdo_server_t SnInfo= {
		.product_ID=1,//uint32	product_ID;//��Ʒ������룬HA���Ʒ����Ϊ1
		.vendor_ID = 1,//uint32  vendor_ID;
		.product_date="2018-07-06",//char	*product_date;
		.password="123456",//char	*password;
		.sn = (uint8*)sn,//uint8 	*sn;
		.addr = (uint8*)addr,//uint8	*addr;
		.url = "www.glalaxy.com",//char	*url;
		.server_ipv4="119.23.8.181",//char	*server_ipv4;
		.server_udp_port=16729,//uint16	server_udp_port;
		.server_tcp_port = 16728,//uint16	server_tcp_port;
		.local_udp_port = 17729,//uint16	local_udp_port;
		.version = 1 //uint16	version;//��Ʒ��ʼ�汾
};
void FUC_ATTR app_task_init(uint8 is_connect)
{
	aps_listener_t	mylisten;
	hal_key_init();
	hal_led_init();
	factory_init();
	pluto_init();
	if(is_connect==osFalse)
	{
    	hal_led_blink(0x7FFFFFFF,100,100);
    	pluto_registe_led(NULL);
	}
	else
	{
		hal_led_blink(0x7FFFFFFF,500,500);
		pluto_registe_led(hal_led_blink);
	}
	mylisten.recieve_msg = pluto_recieve_message_cb;
	mylisten.send_status = NULL;
	aps_set_listener(&mylisten);
    pdo_set_device_describe((device_describe_t*)&device_describe);
    pdo_register_port(1,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,osFalse);
    pdo_register_port(2,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,osFalse);
    pdo_register_port(3,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,osFalse);
    pdo_register_port(4,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,osFalse);
    login_set_state_change_cb(app_login_state_cb);
	osStartReloadTimer(20,app_task,app_sig_keyscan,NULL);
	osStartReloadTimer(100,app_task,app_sig_test_send,NULL);
    //pdo_write_server((pdo_server_t*)&SnInfo);
}
#define RESET_WIFI_CONFIG	1
#define RESET_ALL_CONFIG	2
#include "hal_uart.h"
uint8 bb[]={0xfe, 0x03, 0x21, 0x08, 0x21, 0x00, 0x14, 0x1f};
uint8* FUC_ATTR nwk_get_local_addr(void);
void FUC_ATTR app_task(int sig,void *arg,int len)
{
	command_t cmd;
	address_t addr;
	static uint8 state = 0;
	static uint8 flag = 0;
	static uint8 start_flag = 0;
	uint32 time;
	uint8 	key,shift;
	cmd.aID 	= aID_Gen_Type_Switch;
	cmd.cmd_id 	= cmd_return;
	cmd.option = 0x00;
	switch(sig)
	{
	case app_sig_test_send:
		if(start_flag)
		{
			osMemcpy(addr.addr,nwk_get_local_addr(),8);
			addr.keyID = keyID_default;
			addr.port = 04;
			aps_req_send_command(&addr,pluto_get_seq(),&cmd,&state,1,option_default);
			addr.port = 03;
			aps_req_send_command(&addr,pluto_get_seq(),&cmd,&state,1,option_default);
			addr.port = 02;
			aps_req_send_command(&addr,pluto_get_seq(),&cmd,&state,1,option_default);
		}
		break;
	case app_sig_keyscan:
		shift = hal_get_key(&key,20);
		if(shift)
		{
			if(shift==HAL_KEY_DOWN)
			{
				start_flag = !start_flag;
				flag = 0x00;
				state= !state;
				osLogI(1,"app_sig_keyscan: key:%02x shift:%02x  free:%d \r\n ",key,shift,esp_get_free_heap_size());
				flag = 0;
				if(key==0x08)
				{
					aps_req_report_command(0x04,pluto_get_seq(),&cmd,&state,1,option_default);
				}
				else if(key==0x04)
				{
					aps_req_report_command(0x03,pluto_get_seq(),&cmd,&state,1,option_default);
				}
				else if(key==0x02)
				{
					aps_req_report_command(0x02,pluto_get_seq(),&cmd,&state,1,option_default);
				}
				else if(key==0x01)
				{
					aps_req_report_command(0x01,pluto_get_seq(),&cmd,&state,1,option_default);
				}
			}
			else if(shift==HAL_KEY_KEEP)
			{
				time = hal_key_keeptime();
				osLogI(0,"app_task: key time:%d flag:%d \r\n !",time,flag);
				if(time>8000)
				{
					if(flag!=2)
					{
						flag = 2;
						osStartTimer(100,factory_reset,RESET_ALL_CONFIG,NULL);
						osDeleteTimer(factory_reset,RESET_WIFI_CONFIG);
					}
				}
				else if(time>4000)
				{
					if(flag!=1)
					{
						flag = 1;
						osLogI(1,"app_task: restore zigbee config \r\n !");
						hal_led_blink(4,200,200);
						osStartTimer(5000,factory_reset,RESET_WIFI_CONFIG,NULL);
					}
				}
			}
		}
	break;
	}
}
static void pluto_recieve_message_cb(address_t *src,uint8 seq, command_t *cmd, uint8 *pdata,uint32 len)
{
	osLogI(1,"zb_recieve_message_cb port:%d, aID:%08x cmd_id:%02x,\r\n",src->port,cmd->aID,cmd->cmd_id);
	osLogI(1,"test_mem: free heap: %d\r\n",esp_get_free_heap_size());
}
static void FUC_ATTR factory_reset(int sig, void *arg, int len)
{
	wifi_config_t sta_conf;
	osLogI(1,"factory_reset: sig:%d \r\n !",sig);
	osMemset(&sta_conf,0,sizeof(wifi_config_t));
	esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_conf);
	switch(sig)
	{
	case RESET_WIFI_CONFIG:
		break;
	case RESET_ALL_CONFIG:
		pluto_disk_format();
		break;
	}
	pluto_reset_system(1000);
}
static void FUC_ATTR app_login_state_cb (uint8 state)
{
	osLogI(1,"app_login_state_cb: state:%d \r\n !",state);
	switch(state)
	{
	case login_state_online:
		pluto_registe_led(hal_led_blink);
		hal_led_blink(1,10,0);
		hal_led_set(LED_STATUS_OFF);
		break;
	case login_state_offline:
		pluto_registe_led(NULL);
		hal_led_blink(0x0FFFFFFF,1000,1000);
		break;
	case login_state_start:
	case login_state_stop:
	case login_state_login_failed:
	case login_state_logout_failed:
		pluto_registe_led(NULL);
		hal_led_blink(0x0FFFFFFF,1000,1000);
		break;
	}
}
