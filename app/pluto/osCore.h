/*
 * jf_core.h
 *
 *  Created on: 2017��1��6��
 *      Author: Administrator
 */

#ifndef OS_CORE_H_
#define OS_CORE_H_

#include "osType.h"
#include "external_include.h"

typedef struct
{
	int 	wday;
    uint16  year;
    uint8   mon;
    uint8   day;

    uint8   hour;
    uint8   min;
    uint8   sec;
    uint8	rstat;
} osLocalTime_t;

typedef struct
{
	void 		*next;
	uint32		cnt;
	uint32		reload;
	task_t		fuc;
	int			sig;
	int			len;
	void		*arg;
}osTimer_t;


#define os_get_u32_hh(x)	((x>>24)&0x000000FF)
#define os_get_u32_h(x)		((x>>16)&0x000000FF)
#define os_get_u32_l(x)		((x>>8)&0x000000FF)
#define os_get_u32_ll(x)	((x)&0x000000FF)

#define os_get_u16_h(x)		((x>>8)&0x00FF)
#define os_get_u16_l(x)		((x)&0x00FF)

extern void 			osSrand(uint32 seek);
extern uint32 			osRand (void);
extern void 			osRandbytes(uint8 *d, int len);

#define 	osMemcpy	memcpy
#define 	osMemcmp	memcmp
#define 	osMemset	memset
#define 	osStrcpy 	strcpy
#define 	osStrcat 	strcat
#define 	osStrcmp	strcmp
#define 	osStrncmp	strncmp
#define 	osStrlen	strlen
#define 	osStrchr	strchr

extern int 				osMemchkz(void *s, int len);
extern int  			osStrnlen(const char *s, int count);
extern char  			*osStrstr(char *str0,char *str1);
extern char  			*osCaseStrstr(char *str0,char *str1);

extern int  			osToupper(int c);
extern uint64 			osBtoU64(uint8 *pdata);
extern uint32 			osBtoU32(uint8 *pdata);
extern uint16 			osBtoU16(uint8 *pdata);
extern void 			osU64toB(uint8 *pdata, uint64 val);
extern void 			osU32toB(uint8 *pdata, uint32 val);
extern void 			osU16toB(uint8 *pdata, uint16 val);

extern char *			osBtoStr(uint8 *pdata, int len);
extern int	 			osBtoHexStr(char *s,char split,uint8 *b, int blen);
extern int 				osHexStrtoB(uint8 *d,int dlen,char *s,char split);

//extern uint32  			os_inet_addr(char *ip);
extern char  			osBtoA(uint8 x);
extern uint8  			osAtoB(char c);

extern uint32 			osSubAbs(uint32 s,uint32 n);
extern uint32 			osSubLoop32(uint32 s, uint32 n);
extern uint16  			osGetCheckSum16(uint8 *pdata, int len);
//crc16 x16+x15+x2+1
extern void  			osGenRandomArrary(uint8 *p,int len);
#define 				osGetCRC16(pdata, len)		osCRC16(0,pdata,len)
extern 	uint16  		osCRC16(uint16 crc, uint8 *pdata, int len);
extern	uint8  			osGetXOR(uint8 *pdata, int len);

extern int  			osAtoI(char *str);
extern char*			osItoA(int n, char * chBuffer);
extern int 				osFtoA(char *str, float num, int n);        //n��ת���ľ��ȣ������ַ���'.'���м�λС��
extern float 			osAtoF(const char *str);
extern int  			osSkipAtoI(const char **s);

#define 	os_htons			htons
#define 	os_ntohs			ntohs
#define 	os_inet_addr	    inet_addr
#define 	jf_atoi				atoi
#define 	jf_itoa 			itoa

#define 	osSleep(x)		vTaskDelay(x/portTICK_RATE_MS)

extern osState	 		osInit(void);
extern osState 			osSendMessage(task_t fuc, int sig, void *arg,int len);
extern void 			osTickUpdata(uint32 ticks);

extern osState  		osStartSuperTimer(uint32 tim, task_t fuc, int sig, void *arg,int len);
extern osState  		osStartSuperReloadTimer(uint32 tim, task_t fuc, int sig, void *arg,int len);
extern osState  		osStartTimer(uint32 tim, task_t fuc, int sig, void *arg);
extern osState  		osStartReloadTimer(uint32 tim, task_t fuc, int sig, void *arg);
extern osState 			osAddTimer(uint32 tim,uint32 reload, task_t fuc,int sig,void *arg,int len);
extern osState 			osDeleteTimer(task_t fuc, int sig);
extern void 			osTimerInfo();

extern unsigned long  	osLocalToUnix(int zone,osLocalTime_t *date_time);
extern int 				osUnixtoLocal (int zone, osLocalTime_t *tp,uint64 t);

extern uint32  			osGetmsSecond(void);
extern uint32  			osGetmsTimestamp(void);
extern uint32  			osGetUnixTime(void);
extern void  			osSetUnixTime(uint32 sec);
#define 	osMalloc(x)		malloc(x)
#define		osFree(x)		free(x)

#define  	osEntercritical() //	vTaskSuspendAll()//vPortEnterCritical()
#define  	osExitcritical()	//xTaskResumeAll()//vPortExitCritical()

#define DBG_UDP_SOC			0
#define DBG_TCP_SOC			0
#define DBG_SN				0
#define DBG_NVS				0
#define DBG_FS				0
#define DBG_FS_LEVEL1		0
#define DBG_FS_LEVEL2		0

#define DBG_ERROR			1


#define osSprintf	sprintf
void osPrint2b(char *s,uint8 *pdata, int len);

#define osPrintb(fmt, ...)\
		do{ \
			if(fmt)		\
			osPrint2b(__VA_ARGS__);\
		}while(0)

#define osPrintf(fmt,...)\
	do{ \
		if(fmt)		\
		printf(__VA_ARGS__);\
	}while(0)

#define osLogI(fmt, ...)\
		do{ \
			if(fmt)		\
			{printf("log -I %s %s() line:%d :",__FILE__,__FUNCTION__,__LINE__);\
			printf(__VA_ARGS__);}\
		}while(0)

#define osLogD(fmt, ...)\
		do{ \
			if(fmt)		\
			{\
				{printf("log -D %s %s() line:%d :",__FILE__, __FUNCTION__,__LINE__);\
				printf(__VA_ARGS__);}\
			}\
		}while(0)

#define osLogE(fmt, ...)\
		do{ \
			if(fmt)		\
			{\
				{printf("log -E %s %s() line:%d :",__FILE__,__FUNCTION__,__LINE__);\
				printf(__VA_ARGS__);}\
			}\
		}while(0)

#define osLogB(fmt, ...)\
		do{ \
			if(fmt)		\
			{\
				{printf("log -B %s %s() line:%d :",__FILE__,__FUNCTION__,__LINE__);osPrint2b(__VA_ARGS__);}\
			}\
		}while(0)

#endif /* OSCORE_H_ */
