/*
 * app_task.c
 *
 *  Created on: Apr 15, 2019
 *      Author: lort
 */
#include "osCore.h"
#include "pluto.h"
#include "pluto_entry.h"
#include "aID.h"
#include "app_task.h"

static void pluto_recieve_message_cb(address_t *src,uint8 seq, command_t *cmd, uint8 *pdata,uint32 len);
static void app_login_state_cb (uint8 state);

const uint32 device_attribute_id[] = {
	aID_PDO_Type_Device_Info,//
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

void app_task_init(void)
{
	aps_listener_t	mylisten;
	pluto_init();
	mylisten.recieve_msg = pluto_recieve_message_cb;
	mylisten.send_status = NULL;
	aps_set_listener(&mylisten);
    pdo_set_device_describe((device_describe_t*)&device_describe);
    pdo_register_port(1,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,osFalse);
    pdo_register_port(2,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,osFalse);
    pdo_register_port(3,Application_ID_Switch,(uint32*)port_attribute_id,PORT_ATTRIBUTE_ID_NUM,osFalse);
    login_set_state_change_cb(app_login_state_cb);
}
static void pluto_recieve_message_cb(address_t *src,uint8 seq, command_t *cmd, uint8 *pdata,uint32 len)
{
	osLogI(1,"zb_recieve_message_cb port:%d, aID:%08x cmd_id:%02x,\r\n",src->port,cmd->aID,cmd->cmd_id);
	osLogI(1,"test_mem: free heap: %d\r\n",esp_get_free_heap_size());
}
static void app_login_state_cb (uint8 state)
{
	osLogI(1,"app_login_state_cb: state:%d \r\n !",state);
	switch(state)
	{
	case login_state_online:
		break;
	case login_state_offline:
		break;
	case login_state_start:
	case login_state_stop:
	case login_state_login_failed:
	case login_state_logout_failed:
		break;
	}
}
