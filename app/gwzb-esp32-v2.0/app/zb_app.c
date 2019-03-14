/*
 * zb_app.c
 *
 *  Created on: 2017��7��11��
 *      Author: lort
 */

#include "osCore.h"
#include "zb_common.h"
#include "zb_io_buffer.h"
#include "zb_common.h"
#include "zb_map.h"
#include "zb_zcl.h"
#include "zb_zcl_general.h"
#include "zb_zcl_ha.h"
#include "zcl_closures.h"
#include "zcl_lighting.h"
#include "zcl_ss.h"
#include "zb_zdo.h"
#include "zb_system.h"
#include "zb_color.h"
#include "pluto.h"
#include "aID.h"
#include "hal_lamp.h"

static void zb_app_task(int sig, void *arg, int len);
static void pluto_permit_join_cb(uint8 state,uint16 duration);
static void pluto_delete_port_cb(uint8 port);

static void app_req_read_onoff(uint8 ep, uint16 addr);
static void app_req_read_window_convering(uint8 ep, uint16 addr);
static void app_req_read_level_control(uint8 ep, uint16 addr);
static void app_req_read_color_lighting(uint8 ep, uint16 addr);

static void zb_report_command_process(uint16 addr,uint8 ep,uint8 seq, uint16 cID, uint8 *pdata,  uint8 len);
static void zb_default_respone_process(uint16 addr, uint8 ep,uint8 seq, uint16 cID, uint8 *pdata, uint8 len);
static void zb_read_respone_process(uint16 addr, uint8 ep,uint8 seq, uint16 cID, uint8 *pdata, uint8 len);
static void zb_specific_process(uint16 addr,uint8 ep,uint8 seq, uint16 cID,uint8 cmd, uint8 *pdata,  uint8 len);

osState zb_app_init(void)
{
	uint8 ret;
	pdo_set_permit_join_listener(pluto_permit_join_cb);
	pdo_set_delete_port_listener(pluto_delete_port_cb);
	ret = zb_io_init();
	osLogI(DBG_MT_APP,"zb_app_init ret:%d \r\n",ret);
	osStartTimer(5000,zb_app_task,1,NULL);
	return ret;
}
static void zb_app_task(int sig, void *arg, int len)
{
	zb_read_network_info();
}
static void pluto_permit_join_cb(uint8 state,uint16 duration)
{
	if(state==osTrue)
		zb_permite_join(0xFFFC,200,1);
	else
		zb_permite_join(0xFFFC,0,1);
	osLogI(DBG_MT_APP,"zb_permit_join_cb state:%02x, duration:%04x \r\n",state,duration);
}
static void pluto_delete_port_cb(uint8 port)
{

}
void pluto_send_status_cb(address_t *src,uint8 seq, command_t *cmd, uint8 state)
{
	osLogI(DBG_MT_APP,"zb_send_status_cb state:%d \r\n",state);
}
static void FUC_ATTR set_on_off(uint8 port, uint8 state)
{
	switch(port)
	{
	case 0x01:
		hal_lamp_set(LAMP0,state);
		break;
	case 0x02:
		hal_lamp_set(LAMP1,state);
		break;
	case 0x03:
		hal_lamp_set(LAMP2,state);
		break;
	case 0x04:
		hal_lamp_set(LAMP3,state);
		break;
	default:
		return ;
	}
}
static uint8 FUC_ATTR read_on_off(uint8 port)
{
	uint8 state = 0x00;
	switch(port)
	{
	case 0x01:
		state = hal_lamp_get(LAMP0);
		break;
	case 0x02:
		state = hal_lamp_get(LAMP1);
		break;
	case 0x03:
		state = hal_lamp_get(LAMP2);
		break;
	case 0x04:
		state = hal_lamp_get(LAMP3);
		break;
	}
	return state;
}
void pluto_recieve_message_cb(address_t *src,uint8 seq, command_t *cmd, uint8 *pdata,uint32 len)
{
	color_xy_t 	cxy;
	color_rgb_t rgb;
	uint32		temp;
	uint16		level;
	uint16 		addr;
	uint16  	cID;
	uint8		ep;
	if(src->port<4)
	{
		uint8 state;
		uint8 cmd_id = cmd->cmd_id;
		if(cmd->aID==aID_Gen_Type_Switch)
		{
			cmd->cmd_id |= cmd_return;
			if(cmd_id==cmd_write)
			{
				set_on_off(src->port,pdata[0]);
				aps_req_report_command(src->port,seq,cmd,pdata,1,option_default);
			}
			else if(cmd_id==cmd_read)
			{
				state = read_on_off(src->port);
				aps_req_send_command(src,seq,cmd,&state,1,option_default);
			}
		}
		else if(cmd->aID==aID_Gen_Type_LQI)
		{
			cmd->cmd_id |= cmd_return;
			if(cmd_id==cmd_read)
			{
				state = pluto_get_device_lqi();
				aps_req_send_command(src,seq,cmd,&state,1,option_default);
			}
		}
	}
	else
	{
		addr = zb_get_addr(src->port);
		ep = zb_get_ep(src->port);
		cID = zb_attributeID_to_clusteID(cmd->aID);
		osLogI(1,"zb_recieve_message_cb port:%d--> addr:%04x,  ep:%02x cID:%04x aID:%08x cmd_id:%02x,\r\n",src->port,addr,ep,cID,cmd->aID,cmd->cmd_id);
		osLogI(DBG_MT_APP,"test_mem: free heap: %d\r\n",esp_get_free_heap_size());
		switch(cmd->aID)
		{
			case aID_Gen_Type_Switch://switch
				if(cmd->cmd_id==cmd_write)
				{
					zclGeneral_SendOnOff(addr,ep,osFalse,zb_get_seq(),pdata[0]);
					pdo_port_set_value(src->port,"value",pdata[0]);
				}
				else if(cmd->cmd_id==cmd_read)
				{
					app_req_read_onoff(ep,addr);
				}
				break;
			case aID_Gen_Type_Level://dimer light
			case aID_Gen_Type_White:
				if(cmd->cmd_id==cmd_write)
				{
					zclGeneral_SendLevelControlMoveToLevelWithOnOff(addr,ep,pdata[0],0,osFalse,zb_get_seq());
					pdo_port_set_value(src->port,"value",pdata[0]);
				}
				else if(cmd->cmd_id==cmd_read)
				{
					app_req_read_level_control(ep,addr);
				}
				if(pdata[0]==0)
					zclGeneral_SendOnOff(addr,ep,osFalse,zb_get_seq(),COMMAND_OFF);
				break;
			case aID_Gen_Type_Red://RGB light
			case aID_Gen_Type_Green:
			case aID_Gen_Type_Blue:
			case aID_Gen_Type_Yellow:
				break;
			case aID_Gen_Type_Color:
				if(cmd->cmd_id==cmd_write)
				{
					rgb.R = pdata[0];
					rgb.G = pdata[1];
					rgb.B = pdata[2];
					temp = osBtoU32(pdata);
					pdo_port_set_value(src->port,"value",temp);
					color_rgb_to_xy(&cxy,&rgb);
					level = (uint8)zclLighting_ColorControl_Send_MoveToColorCmd(ep,addr,
							(uint16)(cxy.x*0xFEFF), (uint16)(cxy.y*0xFEFF),
							0,osFalse,zb_get_seq());
					level += pdata[3];
					level /= 2;
					if(level)
						zclGeneral_SendLevelControlMoveToLevelWithOnOff(addr,ep,level,0,osFalse,zb_get_seq());
					else
						zclGeneral_SendOnOff(addr,ep,osFalse,zb_get_seq(),COMMAND_OFF);
				}
				else if(cmd->cmd_id==cmd_read)
				{
					app_req_read_color_lighting(ep,addr);
				}
				break;
			case aID_Gen_Type_WindowSwitch:
				if(cmd->cmd_id==cmd_write)
				{
					pdo_port_set_value(src->port,"value",pdata[0]);
					switch(pdata[0])
					{
					case 0x01://open
						zclClosures_SendUpOpen(ep,addr,osFalse,zb_get_seq());
						break;
					case 0x02://close
						zclClosures_SendDownClose(ep,addr,osFalse,zb_get_seq());
						break;
					case 0x00://stop
						zclClosures_SendStop(ep,addr,osFalse,zb_get_seq());
						break;
					}
				}
				else if(cmd->cmd_id==cmd_read)
				{
					app_req_read_window_convering(ep,addr);
				}
				break;
			case aID_Gen_Type_WindowPercent:
				pdo_port_set_value(src->port,"value",pdata[0]);
				zclClosures_SendGoToLiftPercentage(ep,addr,pdata[0],osFalse,zb_get_seq());
				break;
			case aID_Gen_Type_Locker:
				break;
			default:
				break;
		}//switch
	}
}

void zb_app_receive_message(uint16 addr, uint8  ep,uint8 seq, uint16 cID, uint8 cmd, uint8 specific, uint8 *pdata, uint8 len)
{
	osLogI(DBG_MT_APP,"zb_zcl_receive_message: addr:%04x,ep:%02x,cID:%04x,cmd:%02x,specific:%02x ",addr,ep,cID,cmd,specific);
	osLogB(DBG_MT_APP,"data:",pdata,len);
	if(specific==0x00)//process normal message
	{
		switch(cmd)
		{
		case ZCL_CMD_READ:
			break;
		case ZCL_CMD_READ_RSP:
			zb_read_respone_process(addr,ep,seq,cID,pdata,len);
			break;
		case ZCL_CMD_WRITE:
		case ZCL_CMD_WRITE_UNDIVIDED:
		case ZCL_CMD_WRITE_RSP:
		case ZCL_CMD_WRITE_NO_RSP:
		case ZCL_CMD_CONFIG_REPORT:
		case ZCL_CMD_CONFIG_REPORT_RSP:
		case ZCL_CMD_READ_REPORT_CFG:
		case ZCL_CMD_READ_REPORT_CFG_RSP:
			break;
		case ZCL_CMD_REPORT:
			zb_report_command_process(addr,ep,seq,cID,pdata,len);
			break;
		case ZCL_CMD_DEFAULT_RSP:
			zb_default_respone_process(addr,ep,seq,cID,pdata,len);
			break;
		case ZCL_CMD_DISCOVER_ATTRS:
		case ZCL_CMD_DISCOVER_ATTRS_RSP:
		case ZCL_CMD_DISCOVER_CMDS_RECEIVED:
		case ZCL_CMD_DISCOVER_CMDS_RECEIVED_RSP:
		case ZCL_CMD_DISCOVER_CMDS_GEN:
		case ZCL_CMD_DISCOVER_CMDS_GEN_RSP:
		case ZCL_CMD_DISCOVER_ATTRS_EXT:
		case ZCL_CMD_DISCOVER_ATTRS_EXT_RSP:
		default:
			osLogI(DBG_ERROR,"zb_zcl_receive_message: Unknown command :%02x \r\n",cmd);
			break;
		}
	}
	else//specific command basic cluster id
	{
		zb_specific_process(addr,ep,seq,cID,cmd,pdata,len);
	}
}
static void zb_specific_process(uint16 addr,uint8 ep,uint8 seq, uint16 cID,uint8 cmd, uint8 *pdata,  uint8 len)
{
	command_t 	scmd;
	uint16		value16;
	uint8 		port;
	uint8		buf[4];
	port = zb_get_port(addr,ep);
	osLogI(DBG_MT_APP,"zb_specific_process: addr:%04x,ep:%02x,cID:%04x,cmd:%02x,port:%02x ",addr,ep,cID,cmd,port);
	osLogB(DBG_MT_APP,"data:",pdata,len);
	switch(cID)
	{
	case ZCL_CLUSTER_ID_SS_IAS_ZONE:
		if(cmd==COMMAND_SS_IAS_ZONE_STATUS_CHANGE_NOTIFICATION)
		{
			value16 = zb_btou16(pdata);
			pdo_port_set_value(port,"value",value16);
			scmd.option = aID_Gen_Option;
			scmd.cmd_id = cmd_alarm;
			scmd.aID = zb_clusteID_to_attributeID(cID);
			osLogI(DBG_MT_APP,"zb_report_command_process:report aID:%08x \r\n",scmd.aID);
			value16 = pdo_port_get_value(port,"value");
			if(value16)
				buf[0] = 0x01;
			else
				buf[0] = 0x00;
			aps_req_report_command(port,pluto_get_seq(),&scmd,buf,1,option_default);
		}
		break;
	}
}
static void zb_report_command_process(uint16 addr,uint8 ep,uint8 seq, uint16 cID, uint8 *pdata,  uint8 len)
{
	command_t 	cmd;
	uint8 		port;
	uint8		value8;
	osLogI(DBG_MT_APP,"zb_report_command_process: addr:%04x,ep:%02x,seq:%d,cID:%04x ",addr,ep,seq ,cID);
	osLogB(DBG_MT_APP,"data:",pdata,len);
	port = zb_get_port(addr,ep);
	if(port!=0)
	{
		switch(cID)
		{
		case ZCL_CLUSTER_ID_GEN_ON_OFF:
		case ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL:
		case ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING:
			value8 = pdata[3];
			if(cID==ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING)
			{
				switch(value8)
				{
				case COMMAND_CLOSURES_UP_OPEN://open
					value8 = 0x01;
					break;
				case COMMAND_CLOSURES_DOWN_CLOSE://close
					value8 = 0x02;
					break;
				case COMMAND_CLOSURES_STOP://stop
					value8 = 0x00;
					break;
				default:
					value8 = 0x00;
				}
			}
			pdo_port_set_value(port,"value",value8);
			cmd.option = aID_Gen_Option;
			cmd.cmd_id = cmd_return;
			cmd.aID = zb_clusteID_to_attributeID(cID);
			osLogI(DBG_MT_APP,"zb_report_command_process:report aID:%08x \r\n",cmd.aID);
			value8 = pdo_port_get_value(port,"value");
			aps_req_report_command(port,pluto_get_seq(),&cmd,&value8,1,option_default);
			break;
		case ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL:
			break;
		case ZCL_CLUSTER_ID_SS_IAS_ZONE:
			break;

		}
	}
}
static void zb_default_respone_process(uint16 addr, uint8 ep,uint8 seq, uint16 cID, uint8 *pdata, uint8 len)
{
	command_t 	cmd;
	uint8 		port;
	uint8		value8;
	uint32		value32;
	uint8		buf[4];
	port = zb_get_port(addr,ep);
	osLogI(DBG_MT_APP,"zb_default_respone_process: addr:%04x,ep:%02x,seq:%d,cID:%04x,port:%d ",addr,ep,seq ,cID,port);
	osLogB(DBG_MT_APP,"data:",pdata,len);
	if(port!=0)
	{
		switch(cID)
		{
		case ZCL_CLUSTER_ID_GEN_ON_OFF:
		case ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL:
		case ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING:
			if(pdata[1]==op_succeed)
			{
				cmd.option = aID_Gen_Option;
				cmd.cmd_id = cmd_write|cmd_return;
				cmd.aID = zb_clusteID_to_attributeID(cID);
				value8 = pdo_port_get_value(port,"value");
				aps_req_report_command(port,pluto_get_seq(),&cmd,&value8,1,option_default);
			}
			break;
		case ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL:
			if(pdata[1]==op_succeed)
			{
				cmd.option = aID_Gen_Option;
				cmd.cmd_id = cmd_write|cmd_return;
				cmd.aID = zb_clusteID_to_attributeID(cID);
				value32 = pdo_port_get_value(port,"value");
				osU32toB(buf,value32);
				aps_req_report_command(port,pluto_get_seq(),&cmd,buf,4,option_default);
			}
			break;
		case ZCL_CLUSTER_ID_SS_IAS_ZONE:
			break;

		}
	}
}
static void zb_read_respone_process(uint16 addr, uint8 ep,uint8 seq, uint16 cID, uint8 *pdata, uint8 len)
{
	command_t 	cmd;
	uint8 		port;
	uint8		value8;
	uint32		value32;
	uint8		buf[4];
	port = zb_get_port(addr,ep);
	osLogI(DBG_MT_APP,"zb_read_respone_process: addr:%04x,ep:%02x,seq:%d,cID:%04x,port:%d ",addr,ep,seq ,cID,port);
	osLogB(DBG_MT_APP,"data:",pdata,len);
	if(port!=0)
	{
		switch(cID)
		{
		case ZCL_CLUSTER_ID_GEN_ON_OFF:
		case ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL:
		case ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING:
			if((pdata[2]==op_succeed))
			{
				value8 = pdata[4];
				if(cID==ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING)
				{
					switch(value8)
					{
					case COMMAND_CLOSURES_UP_OPEN://open
						value8 = 0x01;
						break;
					case COMMAND_CLOSURES_DOWN_CLOSE://close
						value8 = 0x02;
						break;
					case COMMAND_CLOSURES_STOP://stop
						value8 = 0x00;
						break;
					default:
						value8 = 0x00;
					}
				}
				pdo_port_set_value(port,"value",value8);
				cmd.option = aID_Gen_Option;
				cmd.cmd_id = cmd_read|cmd_return;
				cmd.aID = zb_clusteID_to_attributeID(cID);
				value8 = pdo_port_get_value(port,"value");
				aps_req_report_command(port,pluto_get_seq(),&cmd,&value8,1,option_default);
			}
			break;
		case ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL:
			if(pdata[2]==op_succeed)
			{
				cmd.option = aID_Gen_Option;
				cmd.cmd_id = cmd_read|cmd_return;
				cmd.aID = zb_clusteID_to_attributeID(cID);
				value32 = pdo_port_get_value(port,"value");
				osU32toB(buf,value32);
				aps_req_report_command(port,pluto_get_seq(),&cmd,buf,4,option_default);
			}
			break;
		case ZCL_CLUSTER_ID_SS_IAS_ZONE:
			break;

		}
	}
}
//osState zcl_SendRead(uint16 dstAddr,uint8 ep,uint16 clusterID, zclReadCmd_t *readCmd,
//                        uint8 direction, uint8 disableDefaultRsp, uint8 seqNum);
static void app_req_read_onoff(uint8 ep, uint16 addr)
{
	zclReadCmd_t *pcmd;
	osLogI(DBG_MT_APP,"app_req_read_onoff: addr:%04x,ep:%02x\r\n",addr,ep);
	pcmd = (zclReadCmd_t*)osMalloc(sizeof(zclReadCmd_t));
	if(pcmd!=NULL)
	{
		pcmd->numAttr = 0x01;
		pcmd->attrID[0] = ATTRID_ON_OFF;
		zcl_SendRead(addr,ep,ZCL_CLUSTER_ID_GEN_ON_OFF,pcmd,ZCL_FRAME_CLIENT_SERVER_DIR,osFalse,zb_get_seq());
		osFree(pcmd);
	}
}
static void app_req_read_window_convering(uint8 ep, uint16 addr)
{
	zclReadCmd_t *pcmd;
	osLogI(DBG_MT_APP,"app_req_read_window_convering: addr:%04x,ep:%02x\r\n",addr,ep);
	pcmd = (zclReadCmd_t*)osMalloc(sizeof(zclReadCmd_t));
	if(pcmd!=NULL)
	{
		pcmd->numAttr = 0x01;
		pcmd->attrID[0] = ATTRID_CLOSURES_WINDOW_COVERING_TYPE;
		zcl_SendRead(addr,ep,ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,pcmd,ZCL_FRAME_CLIENT_SERVER_DIR,osFalse,zb_get_seq());
		osFree(pcmd);
	}
}
static void app_req_read_level_control(uint8 ep, uint16 addr)
{
	zclReadCmd_t *pcmd;
	osLogI(DBG_MT_APP,"app_req_read_level_control: addr:%04x,ep:%02x\r\n",addr,ep);
	pcmd = (zclReadCmd_t*)osMalloc(sizeof(zclReadCmd_t));
	if(pcmd!=NULL)
	{
		pcmd->numAttr = 0x01;
		pcmd->attrID[0] = ATTRID_LEVEL_CURRENT_LEVEL;
		zcl_SendRead(addr,ep,ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,pcmd,ZCL_FRAME_CLIENT_SERVER_DIR,osFalse,zb_get_seq());
		osFree(pcmd);
	}
}
static void app_req_read_color_lighting(uint8 ep, uint16 addr)
{
	zclReadCmd_t *pcmd;
	osLogI(DBG_MT_APP,"app_req_read_color_lighting: addr:%04x,ep:%02x\r\n",addr,ep);
	pcmd = (zclReadCmd_t*)osMalloc(sizeof(zclReadCmd_t));
	if(pcmd!=NULL)
	{
		pcmd->numAttr = 0x02;
		pcmd->attrID[0] = ATTRID_LIGHTING_COLOR_CONTROL_CURRENT_X;
		pcmd->attrID[1] = ATTRID_LIGHTING_COLOR_CONTROL_CURRENT_X;
		zcl_SendRead(addr,ep,ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,pcmd,ZCL_FRAME_CLIENT_SERVER_DIR,osFalse,zb_get_seq());
		osFree(pcmd);
	}
}

