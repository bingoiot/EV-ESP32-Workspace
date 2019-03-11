/*
 * jf_types.h
 *
 *  Created on: Aug 20, 2016
 *      Author: lort
 */

#ifndef OS_TYPES_H_
#define OS_TYPES_H_

#define FUC_ATTR //	__attribute__((section(".irom0.text")))
//ICACHE_FLASH_ATTR

typedef enum
{
	osFalse = 0x00,
	osTrue = 0x01,

	osDisable = 0x00,
	osEnable = 0x01,

	osError = -1,

	osSucceed = 0x00,
	osFailed = 0x01,

	osParamError = 0x02,
	osPasswordError = 0x04,
	osVersionError = 0x06

}osState;

#ifndef NULL
#define NULL	((void*)0)
#endif

#ifndef uint8
#define uint8 unsigned char
#endif

#ifndef uint16
#define uint16 unsigned short
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef uint64
#define uint64 unsigned long
#endif

#ifndef int8
#define  int8 char
#endif

#ifndef int16
#define  int16 short
#endif

#ifndef int32
#define int32 unsigned int
#endif


#ifndef int64
#define int64 unsigned long
#endif

typedef void (*task_t)(int sig, void *param,int len);

#endif /* JF_OS_JF_TYPES_H_ */
