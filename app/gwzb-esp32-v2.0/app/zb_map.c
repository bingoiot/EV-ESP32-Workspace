/*
 * zb_map.c
 *
 *  Created on: 2017��7��12��
 *      Author: lort
 */

#include "osCore.h"
#include "cJSON.h"
#include "zb_common.h"
#include "zb_zcl.h"
#include "zb_zcl_ha.h"
#include "zb_map.h"
#include "pluto.h"
#include "pluto_file_system.h"
#include "aID.h"

#define FILE_NAME_CLUSTE_ID_MAP		"ClusteID.map"
#define FILE_NAME_DEVICE_ID_MAP		"DeviceID.map"

static cJSON	*ClusteIDList = NULL;
static cJSON	*DeviceIDList = NULL;

static void create_clusteID_map(void);
static void create_deviceID_map(void);
static cJSON *load_clusteID_map(void);
static cJSON *load_deviceID_map(void);
static uint32  get_value_from_obj_list(cJSON *list, char *obj1, uint32 val1, char *obj2);

const uint32 clusteID_map[]={
 ZCL_CLUSTER_ID_GEN_POWER_CFG,
	 aID_Gen_Type_LowPowerAlarm,
 ZCL_CLUSTER_ID_GEN_ON_OFF,
	 aID_Gen_Type_Switch,
 ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,
	 aID_Gen_Type_White,
 ZCL_CLUSTER_ID_GEN_ALARMS,
 	 aID_Gen_Type_IAS_ZON_Alarm,
	// Closures Clusters
 ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,
	 aID_Gen_Type_Locker,
 ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,
	 aID_Gen_Type_WindowSwitch,
	// HVAC Clusters
 ZCL_CLUSTER_ID_HVAC_FAN_CONTROL,
	 aID_Gen_Type_FanSwitch,
 ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,
	 aID_Gen_Type_Color,
	// Measurement and Sensing Clusters
 ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT,
	 aID_Gen_Type_Illuminance_Measurement,
 ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,
	 aID_Gen_Type_TemperatureMeasurement,
 ZCL_CLUSTER_ID_MS_PRESSURE_MEASUREMENT,
	 aID_Gen_Type_PressureMeasurement,
 ZCL_CLUSTER_ID_MS_FLOW_MEASUREMENT,
	 aID_Gen_Type_FlowMeasurement,
 ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY,
	 aID_Gen_Type_HumidityMeasurement,
 ZCL_CLUSTER_ID_MS_OCCUPANCY_SENSING,
	 aID_Gen_Type_HumenIRAlarm,
 ZCL_CLUSTER_ID_SS_IAS_ZONE,
 	 aID_Gen_Type_IAS_ZON_Alarm,
 ZCL_CLUSTER_ID_SS_IAS_ACE,
 	 aID_Gen_Type_IAS_ZON_Alarm,
 ZCL_CLUSTER_ID_SS_IAS_WD,
 	 aID_Gen_Type_IAS_ZON_Alarm,
};
const uint32 deviceID_map[]={
 ZCL_HA_DEVICEID_ON_OFF_SWITCH,
	 Application_ID_Switch,
 ZCL_HA_DEVICEID_LEVEL_CONTROL_SWITCH,
	 Application_ID_DimmerLight,
 ZCL_HA_DEVICEID_ON_OFF_OUTPUT,
	 Application_ID_Switch,
 ZCL_HA_DEVICEID_LEVEL_CONTROLLABLE_OUTPUT,
	 ZCL_HA_DEVICEID_LEVEL_CONTROL_SWITCH,
 ZCL_HA_DEVICEID_MAINS_POWER_OUTLET,
	 Application_ID_MainPower_Outlet,
 ZCL_HA_DEVICEID_DOOR_LOCK,
	 Application_ID_Locker,
 ZCL_HA_DEVICEID_DOOR_LOCK_CONTROLLER,
	 Application_ID_Locker,
 ZCL_HA_DEVICEID_SIMPLE_SENSOR,
	 Application_ID_SimpleSensor,
 ZCL_HA_DEVICEID_HOME_GATEWAY,
	 Application_ID_Gateway,
 ZCL_HA_DEVICEID_SMART_PLUG,
	 Application_ID_86Socket,
// Lighting Device IDs
 ZCL_HA_DEVICEID_ON_OFF_LIGHT,
 	 Application_ID_Switch,//Application_ID_Light,
 ZCL_HA_DEVICEID_DIMMABLE_LIGHT,
	 Application_ID_DimmerLight,
 ZCL_HA_DEVICEID_COLORED_DIMMABLE_LIGHT,
	 Application_ID_ColorLight,
 ZCL_HA_DEVICEID_ON_OFF_LIGHT_SWITCH,
	 Application_ID_Light,
 ZCL_HA_DEVICEID_DIMMER_SWITCH,
	 Application_ID_DimmerLight,
 ZCL_HA_DEVICEID_COLOR_DIMMER_SWITCH,
	 Application_ID_ColorLight,

 ZCL_HA_DEVICEID_LIGHT_SENSOR,
 	 aID_Gen_Type_Illuminance_Measurement,
 ZCL_HA_DEVICEID_OCCUPANCY_SENSOR,
	 Application_ID_BodyInfraredAlarm,
// Closures Device IDs
 ZCL_HA_DEVICEID_SHADE,
	 Application_ID_Blind_WidowCovering,
 ZCL_HA_DEVICEID_SHADE_CONTROLLER,
	 Application_ID_Blind_WidowCovering,
 ZCL_HA_DEVICEID_WINDOW_COVERING_DEVICE,
	 Application_ID_WidowCovering,
 ZCL_HA_DEVICEID_WINDOW_COVERING_CONTROLLER,
	 Application_ID_WidowCovering,
// HVAC Device IDs
 ZCL_HA_DEVICEID_HEATING_COOLING_UNIT,
	 Application_ID_AirConditioning,
 ZCL_HA_DEVICEID_TEMPERATURE_SENSOR,
	 Application_ID_TemperatureSensor,
 ZCL_HA_DEVICEID_PRESSURE_SENSOR,
	 Application_ID_PressureSensor,
 ZCL_HA_DEVICEID_FLOW_SENSOR,
	 Application_ID_FlowSensor,
 ZCL_HA_DEVICEID_IAS_ZONE,
	 Application_ID_IAS_ZONE,
 ZCL_HA_DEVICEID_IAS_WARNING_DEVICE,
	 Application_ID_AlarmDevice,
};
static uint8 add_attr_to_list(uint32 *list, uint32 attr, uint8 num);
osState zb_create_port(uint8 *exaddr, uint16 addr, uint8 ep, uint16 device_id, uint16 *cid, uint8 cid_num)
{
	uint32 *attr;
	uint32	appID;
	uint32	temp;
	char	str_exaddr[8*2+1];
	uint8	i,cnt = 0;
	uint8	free_port;
	uint8	ret = osFailed;
	osLogI(DBG_MT_DEV,"zb_create_port:addr:%04x ,ep:%02x, devid:%04x\r\n",addr,ep,device_id);
	appID = zb_deviceID_to_applicationID(device_id);
	attr = (uint32*)osMalloc((cid_num+1)*sizeof(uint32));
	if(attr!=NULL)
	{
		osMemset(attr,0,(cid_num+1)*sizeof(uint32));
		for(i=0;i<cid_num;i++)
		{
			temp = zb_clusteID_to_attributeID(cid[i]);
			if(temp!=aID_Gen_Type_Unkown)
			{
				cnt = add_attr_to_list(attr,temp,cid_num);
			}
		}
		free_port = pdo_get_free_port();
		if(cnt==0)
		{
			attr[0] = aID_Gen_Type_Unkown;
			cnt++;
		}
		/*else//force add lqi attribute
		{
			attr[cnt] = aID_Gen_Type_LQI;
			cnt++;
		}*/
		osBtoHexStr(str_exaddr,'\0',exaddr,8);
		osLogI(DBG_MT_COMMON,"zb_create_port,device ID:%04x-->app ID:%08x ,exaddr:%s \r\n",device_id,appID,str_exaddr);
		osLogB(DBG_MT_COMMON,"zb_create_port: cluster ID:",(uint8*)cid,cid_num*sizeof(uint16));
		osLogB(DBG_MT_COMMON,"zb_create_port: attribute ID:",(uint8*)attr,cnt*sizeof(uint32));
		ret = pdo_register_port(free_port,appID,attr,cnt,osTrue);
		osFree(attr);
		pdo_port_set_value(free_port,"addr",addr);
		pdo_port_set_value(free_port,"ep",ep);
		pdo_port_set_string(free_port,"exaddr",str_exaddr);
	}
	return ret;
}
static uint8 add_attr_to_list(uint32 *list, uint32 attr, uint8 num)
{
	uint8 	i;
	uint8	same = 0x00;
	uint8	count = 0;
	for(i=0;i<num;i++)
	{
		if(list[i]!=0)
		{
			count++;
			if(list[i]==attr)
			{
				same=0x01;
			}
		}
	}
	if(same==0x00)
	{
		for(i=0;i<num;i++)
		{
			if(list[i]==0)
			{
				list[i]=attr;
				count++;
				break;
			}
		}
	}
	return count;
}
uint8 zb_check_addr_exist(uint16 addr)
{
	osLogI(DBG_MT_DEV,"zb_check_addr_exist:%04x\r\n",addr);
	if(pdo_count_port_by_value("addr",addr)>0)
		return osTrue;
	else
		return osFalse;
}
uint8 zb_check_exaddr_exist(uint8* exaddr)
{
	char	str_exaddr[8*2+1];
	osBtoHexStr(str_exaddr,'\0',exaddr,8);
	osLogI(DBG_MT_DEV,"zb_check_exaddr_exist:%s\r\n",str_exaddr);
	if(pdo_count_port_by_string("exaddr",str_exaddr)>0)
		return osTrue;
	else
		return osFalse;
}
uint8 zb_delete_device(uint16 addr)
{
	uint8 	i,num;
	uint8	port;
	uint8 	ret = osFailed;
	num = pdo_count_port_by_value("addr",addr);
	for(i=1;i<=num;i++)
	{
		port = pdo_get_port_by_value_index("addr",addr,num-i);
		pdo_delete_port(port);
	}
	return ret;
}
uint8 zb_delete_device_by_exaddr(uint8 *exaddr)
{
	uint8 	i,num;
	uint8	port;
	uint8 	ret = osFailed;
	char	str_exaddr[8*2+1];
	osBtoHexStr(str_exaddr,'\0',exaddr,8);
	num = pdo_count_port_by_string("exaddr",str_exaddr);
	for(i=1;i<=num;i++)
	{
		port = pdo_get_port_by_string_index("exaddr",str_exaddr,num-i);
		osLogI(DBG_MT_DEV,"zb_delete_device_by_exaddr:delete port:%d \r\n",port);
		pdo_delete_port(port);
	}
	return ret;
}

uint8 zb_get_port(uint16 addr, uint8 ep)
{
	uint8 port;
	port = (uint8)pdo_get_port_by_value2("addr",addr,"ep",ep);
	return port;
}
uint16 zb_get_addr(uint8 port)
{
	uint16 addr;
	addr = (uint16)pdo_port_get_value(port,"addr");
	return addr;
}
uint8 zb_get_ep(uint8 port)
{
	uint8 ep;
	ep = (uint8)pdo_port_get_value(port,"ep");
	return ep;
}
osState zb_save_lqi(uint16 addr, uint8 lqi)
{
	uint8 	i,num;
	uint8	port;
	uint8 	ret = osFailed;
	num = pdo_count_port_by_value("addr",addr);
	for(i=0;i<num;i++)
	{
		port = pdo_get_port_by_value_index("addr",addr,i);
		ret = pdo_port_set_value(port,"lqi",lqi);
	}
	return ret;
}
uint8 zb_get_port_lqi(uint8 port)
{
	uint8 lqi;
	lqi = (uint8)pdo_port_get_value(port,"lqi");
	return lqi;
}
uint16 zb_attributeID_to_clusteID(uint32 aID)
{
	cJSON 	*list;
	uint32	temp,ID;
	ID = 0xFFFF;
	list = load_clusteID_map();
	if(list!=NULL)
	{
		temp = get_value_from_obj_list(list,"aID",aID,"cID");
		if(temp!=0xFFFFFFFF)
			ID = temp;
	}
	return (uint16)ID;
}
uint32 zb_clusteID_to_attributeID(uint16 cID)
{
	cJSON 	*list;
	uint32	temp,ID;
	ID = aID_Gen_Type_Unkown;
	list = load_clusteID_map();
	if(list!=NULL)
	{
		temp = get_value_from_obj_list(list,"cID",cID,"aID");
		if(temp!=0xFFFFFFFF)
			ID = temp;
	}
	return ID;
}
uint32 zb_deviceID_to_applicationID(uint16 device_id)
{
	cJSON 	*list;
	uint32	temp,ID;
	ID = Application_ID_Unkown;
	list = load_deviceID_map();
	if(list!=NULL)
	{
		temp = get_value_from_obj_list(list,"devID",device_id,"appID");
		if(temp!=0xFFFFFFFF)
			ID = temp;
	}
	return ID;
}
uint16 zb_applicationID_to_deviceID(uint32 appID)
{
	cJSON 	*list;
	uint32	temp,ID;
	ID = 0xFFFF;
	list = load_deviceID_map();
	if(list!=NULL)
	{
		temp = get_value_from_obj_list(list,"appID",appID,"devID");
		if(temp!=0xFFFFFFFF)
			ID = temp;
	}
	return (uint16)ID;
}
static uint32 get_value_from_obj_list(cJSON *list, char *obj1, uint32 val1, char *obj2)
{
	cJSON *map,*item;
	uint16 num;
	uint16 i;
	num = cJSON_GetArraySize(list);
	for(i=0;i<num;i++)
	{
		if((map = cJSON_GetArrayItem(list,i))!=NULL)
		{
			if(((item = cJSON_GetObjectItem(map,obj1))!=NULL)&&(item->valueint==val1))
			{
				if((item = cJSON_GetObjectItem(map,obj2))!=NULL)
				{
					return item->valueint;
				}
			}
		}
	}
	return 0xFFFFFFFF;
}
static cJSON *load_clusteID_map(void)
{
	cJSON 	*list = NULL;
	char	*str = NULL;
	if(ClusteIDList==NULL)
	{
		str = pluto_read_text_file(FILE_NAME_CLUSTE_ID_MAP);
		if(str==NULL)
		{
			create_clusteID_map();
			str = pluto_read_text_file(FILE_NAME_CLUSTE_ID_MAP);
		}
		if(str!=NULL)
		{
			osLogI(DBG_MT_DEV,"load_clusteID_map:%s\r\n",str);
			list = cJSON_Parse(str);
		}
		ClusteIDList = list;
	}
	return ClusteIDList;
}
static cJSON *load_deviceID_map(void)
{
	cJSON 	*list = NULL;
	char	*str;
	if(DeviceIDList==NULL)
	{
		str = pluto_read_text_file(FILE_NAME_DEVICE_ID_MAP);
		if(str==NULL)
		{
			create_deviceID_map();
			str = pluto_read_text_file(FILE_NAME_DEVICE_ID_MAP);
		}
		if(str!=NULL)
		{
			osLogI(DBG_MT_DEV,"load_deviceID_map:%s\r\n",str);
			list = cJSON_Parse(str);
		}
		DeviceIDList = list;
	}
	return DeviceIDList;
}
static void create_clusteID_map(void)
{
	cJSON	*list,*map;
	char	*str;
	uint16	i;
	uint16	index;
	uint16	num;
	num = sizeof(clusteID_map)/sizeof(uint32);
	num /= 2;
	list = cJSON_CreateArray();
	if(list!=NULL)
	{
		for(i=0;i<num;i++)
		{
			map = cJSON_CreateObject();
			if(map!=NULL)
			{
				index = i*2;
				cJSON_AddNumberToObject(map,"cID",clusteID_map[index]);
				cJSON_AddNumberToObject(map,"aID",clusteID_map[index+1]);
				cJSON_AddItemToArray(list,map);
			}
		}
		str = cJSON_Print(list);
		if(str!=NULL)
		{
			osLogI(DBG_MT_DEV,"create_clusteID_map:%s\r\n",str);
			pluto_write_text_file(FILE_NAME_CLUSTE_ID_MAP,str);
			osFree(str);
		}
		cJSON_Delete(list);
	}
}
static void create_deviceID_map(void)
{
	cJSON	*list,*map;
	char	*str;
	uint16	i;
	uint16	index;
	uint16	num;
	num = sizeof(deviceID_map)/sizeof(uint32);
	num /= 2;
	list = cJSON_CreateArray();
	if(list!=NULL)
	{
		for(i=0;i<num;i++)
		{
			map = cJSON_CreateObject();
			if(map!=NULL)
			{
				index = i*2;
				cJSON_AddNumberToObject(map,"devID",deviceID_map[index]);
				cJSON_AddNumberToObject(map,"appID",deviceID_map[index+1]);
				cJSON_AddItemToArray(list,map);
			}
		}
		str = cJSON_Print(list);
		if(str!=NULL)
		{
			osLogI(DBG_MT_DEV,"create_deviceID_map:%s\r\n",str);
			pluto_write_text_file(FILE_NAME_DEVICE_ID_MAP,str);
			osFree(str);
		}
		cJSON_Delete(list);
	}
}



