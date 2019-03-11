/*
 * pluto_nvs.h
 *
 *  Created on: 2017Äê8ÔÂ15ÈÕ
 *      Author: lort
 */

#ifndef PLUTO_NVS_H_
#define PLUTO_NVS_H_

#include "osCore.h"

osState pluto_nvs_init(void);
osState pluto_nvs_write(char *file, uint8 *pdata, uint32 len);
uint32 	pluto_nvs_read(char *file, uint8 *pdata, uint32 len);
uint32 	pluto_nvs_length(char *file);

#endif /* JIFAN_JF_FS_JF_NVS_H_ */
