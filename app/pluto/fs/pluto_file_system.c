/*
 * jf_ini.c
 *
 *  Created on: 2017��3��29��
 *      Author: lort
 */

#include "osCore.h"
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "dirent.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "unistd.h"
#include "pluto_file_system.h"
#include "pluto_nvs.h"

const char *base_path = "/spiffs";

#define FLASH_SN_USE_FLAG		(0x5A5AA5A5)
#define FLASH_SN_PSW_LENGTH		32

const char *bk_sn0="SN0";
const char *bk_sn1="SN1";
const char *bk_sn2="SN2";

typedef struct
{
	uint32 	useflag;
	uint16	crc;
	uint16	len;
	uint8	sn[0];
}sn_table_t;

#define 		MAX_FILENAME_LENGTH			(128)
static osState  init_file_system(void);
static void		gen_file_name(char *path, char *dir, char *file);
static uint32 	get_file_size(char *fname);


char  *pluto_read_text_file(char *fname)
{
	FILE 			*pfile;
	char 			*pstr = NULL;
	char			path[MAX_FILENAME_LENGTH];
	uint32			slen = 0;
	uint32			flen;
	init_file_system();
	osLogI(DBG_FS,"pluto_read_text_file: \r\n");
	gen_file_name(path,(char*)base_path,(char*)fname);
	flen = get_file_size((char*)path);
	osLogI(DBG_FS,"pluto_read_text_file: %s, len:%d \r\n",path,flen);
	if(flen>0)
	{
		pfile = fopen(path, "rb");
		if(pfile!=NULL)
		{
			pstr = (char *)osMalloc(flen+1);
			if(pstr!=NULL)
			{
				slen = fread(pstr,sizeof(char),flen,pfile);
				if(slen<=0)
				{
					osFree(pstr);
					pstr = NULL;
				}
				pstr[flen] = 0x00;
				osLogI(DBG_FS_LEVEL1,"pluto_read_text_file: %s \r\n",pstr);
			}
			fclose(pfile);
		}
		else
		{
			osLogI(DBG_ERROR,"pluto_read_text_file: open file :%s error \r\n",path);
		}
	}
	return pstr;
}

osState  pluto_write_text_file(char *fname,char *str)
{
	FILE 			*pfile;
	char			path[MAX_FILENAME_LENGTH];
	uint32			slen;
	init_file_system();
	gen_file_name(path,(char*)base_path,(char*)fname);
	osLogI(DBG_FS,"pluto_write_text_file: %s\r\n",path);
	pfile = fopen(path, "wb");
	if(pfile!=NULL)
	{
		slen = osStrlen(str)+1;
		if(fwrite(str,sizeof(char),slen,pfile)>0)
		{
			fclose(pfile);
			osLogI(DBG_FS_LEVEL1,"pluto_write_text_file: len:%d ,\r\n%s \r\n",slen,str);
			return osSucceed;
		}
		else
			osLogI(DBG_ERROR,"pluto_write_text_file: fs_write file :%s error \r\n",path);
		fclose(pfile);
	}
	else
		osLogI(DBG_ERROR,"pluto_write_text_file: open file :%s error \r\n",path);
	return osFailed;
}

uint32  pluto_get_file_length(char *fname)
{
	char path[MAX_FILENAME_LENGTH];
	uint32	slen = 0;
	init_file_system();
	gen_file_name(path,(char*)base_path,(char*)fname);
	slen = get_file_size(path);
	return slen;
}
uint32  pluto_read_file(char *fname, uint8 *pdata, uint32 len)
{
	FILE 		*pfile;
	char 		path[MAX_FILENAME_LENGTH];
	uint32 		slen = 0;
	init_file_system();
	gen_file_name(path,(char*)base_path,(char*)fname);
	slen = get_file_size(path);
	osLogI(DBG_FS,"pluto_read_file: %s, len:%d \r\n",path,slen);
	if(slen>0)
	{
		osLogI(DBG_FS,"pluto_read_file: \r\n");
		pfile = fopen(path, "rb");
		if(pfile!=NULL)
		{
			slen = (slen>len)?len:slen;
			slen = fread(pdata,sizeof(char),slen,pfile);
			if(slen>0)
			{
				fclose(pfile);
				osLogB(DBG_FS_LEVEL1,"pluto_read_file:",pdata,slen);
				return slen;
			}
			else
				osLogI(DBG_ERROR,"pluto_read_file: fs_read file :%s error \r\n",path);
			fclose(pfile);
		}
		else
			osLogI(DBG_ERROR,"pluto_read_file: open file :%s error \r\n",path);
	}
	return slen;
}
osState  pluto_write_file(char *fname, uint8 *pdata, uint32 len)
{
	FILE 	*pfile;
	char 	path[MAX_FILENAME_LENGTH];
	int 	slen = -1;
	init_file_system();
	gen_file_name(path,(char*)base_path,(char*)fname);
	pfile = fopen(path, "wb");
	if(pfile!=NULL)
	{
		slen = fwrite(pdata,sizeof(char),len,pfile);
		if(slen>=0)
		{
			fclose(pfile);
			osLogB(DBG_FS_LEVEL1,"pluto_write_file:",pdata,slen);
			return osSucceed;
		}
		else
			osLogI(DBG_ERROR,"pluto_write_file: fs_write file :%s error \r\n",path);
		fclose(pfile);
	}
	else
		osLogI(DBG_ERROR,"pluto_write_file: open file :%s error \r\n",path);
	return osFailed;
}
char *pluto_get_file_name(char *filter, uint32 id)
{
	char	path[MAX_FILENAME_LENGTH];
	char	*fname = NULL;
	char	*p = NULL;
	DIR 	*dir;
	struct 	dirent *ptr;
	uint32	cnt = 0;
	uint8 	match = 0x00;
	uint8	match_flag;
	init_file_system();
	osLogI(DBG_FS,"pluto_get_file_name : \r\n");
	gen_file_name(path,(char*)base_path,NULL);
	osLogI(DBG_FS,"pluto_get_file_name: Open dir:%s\r\n",path);
	if ((dir=opendir(path)) != NULL)
	{
		if((p = osStrchr(filter,'*'))!=NULL)
		{
			if(p[1]=='\0')//match all file
			{
				match = 0xFF;
			}
			else
			{
				match =0x01;
				p++;
			}
		}
		osLogI(DBG_FS,"try read dir id:%d \r\n",cnt);
		while ((ptr=readdir(dir)) != NULL)
		{
			match_flag = osFalse;
			osLogI(DBG_FS,"read dir succeed type:%d,name:%s  filter:%s \r\n",ptr->d_type,ptr->d_name,filter);
			switch(match)
			{
			case 0x00://match name
				if(osStrcmp(ptr->d_name,(char*)filter)==0)
				{
					match_flag = osTrue;
				}
				break;
			case 0x01://match type
				if(osCaseStrstr(ptr->d_name,(char*)p)!=NULL)
				{
					match_flag = osTrue;
				}
				break;
			case 0xFF://match all file
				match_flag = osTrue;
				break;
			}
			if(match_flag == osTrue)
			{
				osLogI(DBG_FS,"try read dir filter pass, id = %d name:%s \r\n",id,ptr->d_name);
				if(cnt==id)
				{
					fname = (char*)osMalloc(osStrlen(ptr->d_name)+1);
					if(fname!=NULL)
					{
						osStrcpy(fname,ptr->d_name);
					}
					break;
				}
				cnt++;
			}
		}
		closedir(dir);
	}
	else
	{
		osLogI(DBG_FS,"Open dir error...\r\n");
	}
	return fname;
}
osState pluto_delete_file(char *fname)
{
	char	path[MAX_FILENAME_LENGTH];
	gen_file_name(path,(char*)base_path,fname);
	init_file_system();
	if(remove(path)>=0)
	{
		osLogI(DBG_FS,"pluto_delete_file succeed!-->%s\r\n",path);
		return osSucceed;
	}
	return osFailed;
}
osState pluto_delete_all_file(void)
{

	char	path[MAX_FILENAME_LENGTH];
	DIR 	*dir;
	struct 	dirent *ptr;
	init_file_system();
	gen_file_name(path,(char*)base_path,NULL);
	osLogI(DBG_FS,"pluto_delete_all_file: Open dir:%s\r\n",path);
	if ((dir=opendir(path)) != NULL)
	{
		osLogI(DBG_FS,"pluto_delete_all_file:try read dir \r\n");
		while ((ptr=readdir(dir)) != NULL)
		{
			osLogI(DBG_FS,"pluto_delete_all_file:read dir succeed type:%d,name:%s \r\n",ptr->d_type,ptr->d_name);
			pluto_delete_file(ptr->d_name);
		}
		closedir(dir);
	}
	else
	{
		osLogI(DBG_FS,"pluto_delete_all_file:Open dir error...\r\n");
	}
	return osSucceed;
}
osState pluto_disk_format(void)
{
	osLogI(DBG_FS,"pluto_disk_format\r\n");
	init_file_system();
	pluto_delete_all_file();
	return osSucceed;
}
osState   pluto_rename(char *pname,char *newname)
{
	char	path0[MAX_FILENAME_LENGTH];
	char	path1[MAX_FILENAME_LENGTH];
	init_file_system();
	gen_file_name(path0,(char*)base_path,pname);
	gen_file_name(path1,(char*)base_path,newname);
	rename(path0,path1);
	return osSucceed;
}
static osState init_file_system(void)
{
	static uint8 init_flag = osFalse;
	if(init_flag==osTrue)
		return osFailed;
	init_flag = osTrue;
	osLogI(DBG_FS, "Initializing SPIFFS\r\n");
	esp_vfs_spiffs_conf_t conf = {
	  .base_path = "/spiffs",
	  .partition_label = NULL,
	  .max_files = 1,
	  .format_if_mount_failed = true
	};
	// Use settings defined above to initialize and mount SPIFFS filesystem.
	// Note: esp_vfs_spiffs_register is an all-in-one convenience function.
	esp_err_t ret = esp_vfs_spiffs_register(&conf);
	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			osLogE(DBG_ERROR, "Failed to mount or format filesystem\r\n");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			osLogE(DBG_ERROR, "Failed to find SPIFFS partition\r\n");
		} else {
			osLogE(DBG_ERROR, "Failed to initialize SPIFFS (%d)\r\n", ret);
		}
		return osFailed;
	}
	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total, &used);
	if (ret != ESP_OK) {
		osLogE(DBG_ERROR, "Failed to get SPIFFS partition information\r\n");
	} else {
		osLogI(DBG_FS, "Partition size: total: %d, used: %d\r\n", total, used);
	}
	return osSucceed;
}
static void gen_file_name(char *path, char *dir, char *file)
{
	osStrcpy(path,dir);
	osStrcat(path,"/");
	if(file!=NULL)
		osStrcat(path,file);
}
static uint32  get_file_size(char *fname)
{
	struct 	stat  Stat;
	uint32	slen = 0;
	init_file_system();
	if(stat(fname,&Stat)>=0)
	{
		slen = Stat.st_size;
		osLogI(DBG_FS,"ini_getFileLenth:len%d \r\n",slen);
	}
	else
		osLogI(DBG_FS,"ini_getFileLenth: get file:%s state error \r\n",fname);
	return slen;
}
osState	pluto_write_sn(char *str)
{
	sn_table_t 	*psn;
	uint32		dlen;
	uint32 		slen;
	dlen = osStrlen(str);
	osLogI(DBG_SN,"write_sn:%s, length:%d \r\n",str,dlen);
	slen = sizeof(sn_table_t)+dlen;
	psn = (sn_table_t*)osMalloc(slen);
	if(psn!=NULL)
	{
		osMemset(psn,0,slen);
		osLogI(DBG_SN,"write_flash:\r\n ");
		psn->useflag = FLASH_SN_USE_FLAG;
		psn->len = dlen;
		psn->crc = osGetCRC16((uint8*)str,dlen);
		osMemcpy(psn->sn,str,dlen);
		pluto_nvs_write((char*)bk_sn0,(uint8*)psn,slen);
		pluto_nvs_write((char*)bk_sn1,(uint8*)psn,slen);
		pluto_nvs_write((char*)bk_sn2,(uint8*)psn,slen);
		osFree(psn);
		return osSucceed;
	}
	return osFailed;
}
char* pluto_read_sn(void)
{
	sn_table_t 	*psn = NULL;
	char 		*p;
	uint32 		slen;
	uint16		ret;
	uint16 		crc;
	uint16		i;
	for(i=0;i<3;i++)
	{
		switch(i)
		{
		case 0:
			p = (char*)bk_sn0;
			break;
		case 1:
			p = (char*)bk_sn1;
			break;
		default:
			p = (char*)bk_sn2;
			break;
		}
		slen = pluto_nvs_length(p);
		psn = (sn_table_t *)osMalloc(slen);
		if(psn!=NULL)
		{
			ret = pluto_nvs_read(p,(uint8*)psn,slen);
			if(ret>0)
			{
				if(psn->useflag==FLASH_SN_USE_FLAG)
				{
					crc = osGetCRC16(psn->sn,psn->len);
					if(crc==psn->crc)
					{
						osLogI(DBG_SN,"pluto_read_sn: malloc:%d, realize:%d\r\n",slen,psn->len);
						p = (char*)psn;
						i = psn->len;
						osMemcpy(p,psn->sn,i);
						p[i] = '\0';
						return p;
					}
				}
			}
			osFree(psn);
		}
	}
	return NULL;
}
