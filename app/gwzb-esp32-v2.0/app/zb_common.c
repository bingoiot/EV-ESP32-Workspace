/*
 * zb_common.c
 *
 *  Created on: 2017��7��10��
 *      Author: lort
 */

#include "osCore.h"
#include "zb_common.h"

static uint8 localSeq = 0;
uint8  zb_get_seq(void)
{
	return localSeq++;
}
uint16 zb_btou16(uint8 *pdata)
{
	uint16 temp;
	temp = pdata[1];
	temp <<= 8;
	temp |= pdata[0];
	return temp;
}
void zb_u16tob(uint8 *pdata, uint16 val)
{
	pdata[0] = val&0x00FF;
	pdata[1] = (uint8)(val>>8);
}
uint32 zb_btou32(uint8 *pdata)
{
	uint32 temp;
	temp = pdata[3];
	temp <<= 8;
	temp = pdata[2];
	temp <<= 8;
	temp = pdata[1];
	temp <<= 8;
	temp |= pdata[0];
	return temp;
}
void zb_u32tob(uint8 *pdata, uint32 val)
{
	pdata[0] = (uint8)val;
	pdata[1] = (uint8)(val>>8);
	pdata[2] = (uint8)(val>>16);
	pdata[3] = (uint8)(val>>24);
}



