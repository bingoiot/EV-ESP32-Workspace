/*
 * pluto_nvs.c
 *
 *  Created on: 2017��8��15��
 *      Author: lort
 */

#include "osCore.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "nvs.h"

osState pluto_nvs_init(void)
{
	esp_err_t err;
	static uint8 iniFlag = 0x00;
	osLogI(DBG_NVS,"pluto_nvs_init: \r\n");
	if(iniFlag==0x00)
	{
		iniFlag = 0x01;
	    err = nvs_flash_init();
	    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
	        // NVS partition was truncated and needs to be erased
	        const esp_partition_t* nvs_partition = esp_partition_find_first( ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
	        assert(nvs_partition && "partition table must have an NVS partition");
	        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
	        osLogI(DBG_NVS,"pluto_nvs_init: nvPartition label:%s, size:%d start addr:0x%08x \r\n",nvs_partition->label,nvs_partition->size,nvs_partition->address);
	        // Retry nvs_flash_init
	        err = nvs_flash_init();
	        if(err==ESP_OK)
	        {
	        	osLogI(DBG_NVS,"pluto_nvs_init: init nvs and format disk Ok \r\n");
	        	return osSucceed;
	        }
	    }
	    else if(err==ESP_OK)
	    {
	    	osLogI(DBG_NVS,"pluto_nvs_init: init nvs Ok \r\n");
	    	const esp_partition_t* nvs_partition1 = esp_partition_find_first( ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "nvs");
	    	osLogI(DBG_NVS,"pluto_nvs_init: nvPartition label:%s, size:%d start addr:0x%08x \r\n",nvs_partition1->label,nvs_partition1->size,nvs_partition1->address);
	    }
	    ESP_ERROR_CHECK( err );
	    return osFailed;
	}
	else
		return osSucceed;
}
osState pluto_nvs_write(char *file, uint8 *pdata, uint32 len)
{
	nvs_handle 	my_handle;
	esp_err_t 	err;
	pluto_nvs_init();
	osLogI(DBG_NVS,"pluto_nvs_write: at %s\r\n",file);
	osLogB(DBG_NVS,NULL,pdata,len);
    err = nvs_open(file, NVS_READWRITE, &my_handle);
    if (err == ESP_OK)
    {
    	err = nvs_set_blob(my_handle,file,pdata,len);
    	ESP_ERROR_CHECK( err );
    	err = nvs_commit(my_handle);
    	ESP_ERROR_CHECK( err );
    	nvs_close(my_handle);
    	if(err==ESP_OK)
    	{
    		return osSucceed;
    	}
    	else
    	{
    		osLogI(DBG_ERROR,"pluto_nvs_write error at %s \r\n", file);
    	}
    }
    return osFailed;
}
uint32 pluto_nvs_read(char *file, uint8 *pdata, uint32 len)
{
	nvs_handle 	my_handle;
	size_t 		slen = 0;
	esp_err_t 	err;
	pluto_nvs_init();
	osLogI(DBG_NVS,"pluto_nvs_read: at %s\r\n",file);
    err = nvs_open(file, NVS_READONLY, &my_handle);
    if (err == ESP_OK)
    {
    	err = nvs_get_blob(my_handle, file, NULL,&slen);//get block lenth
    	if(err==ESP_OK)
    	{
    		slen = (slen>len)?len:slen;
    		err = nvs_get_blob(my_handle, file, (char*)pdata,&slen);
    		if(err==ESP_OK)
    		{
    			osLogB(DBG_NVS,"pluto_nvs_read",pdata,slen);
    		}
    		else
    		{
    			slen = 0;
    		}
    	}
    	nvs_close(my_handle);
        osLogI(0,"Error (%d) opening NVS handle! \r\n", err);
    }
    return slen;
}
uint32 pluto_nvs_length(char *file)
{
	nvs_handle 	my_handle;
	size_t 		slen = 0;
	esp_err_t 	err;
	pluto_nvs_init();
	osLogI(DBG_NVS,"pluto_nvs_length: at %s\r\n",file);
    err = nvs_open(file, NVS_READONLY, &my_handle);
    if (err == ESP_OK)
    {
    	slen = 0;
    	err = nvs_get_blob(my_handle, file, NULL,&slen);//get block lenth
    	if(err==ESP_OK)
    	{
    		osLogI(1,"pluto_nvs_read length:%d ",slen);
    	}
    	else
    		slen = 0;
    	nvs_close(my_handle);
        osLogI(0,"Error (%d) opening NVS handle! \r\n", err);
    }
    return slen;
}

