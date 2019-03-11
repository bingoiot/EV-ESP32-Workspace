/*
 * pluto_ini.h
 *
 *  Created on: 2017Äê3ÔÂ29ÈÕ
 *      Author: lort
 */

#ifndef PLUTO_INI_H_
#define PLUTO_INI_H_

#include "osCore.h"



extern char*	pluto_read_text_file(char *fname);
extern osState	pluto_write_text_file(char *fname,char *str);


extern uint32 	pluto_get_file_length(char *fname);
extern uint32 	pluto_read_file(char *fname, uint8 *pdata, uint32 len);
extern osState 	pluto_write_file(char *fname, uint8 *pdata, uint32 len);

extern char*	pluto_get_file_name(char *filter, uint32 id);
extern osState 	pluto_delete_file(char *fname);
extern osState 	pluto_delete_all_file(void);
extern osState 	pluto_disk_format(void);

extern osState  pluto_rename(char *pname,char *newname);

extern osState	pluto_write_sn(char *str);
extern char* 	pluto_read_sn(void);

#endif /* JIFAN_JF_FS_JF_INI_H_ */
