/*
 * jf_virgo.c
 *
 *  Created on: Apr 23, 2017
 *      Author: root
 *
 */

#include "osCore.h"
#include "pluto.h"
#include "pluto_file_system.h"
#include "pluto.h"
#include "aID.h"

/*
 * virgo_bind.c
 *
 *  Created on: Apr 23, 2017
 *      Author: root
 */
 /*
  * binding samples:
  *
   {
		"while":
		{
			"reason":"x<10"//time(z:xx,date:YYYY-MM-DD,time:hh-mm-ss,wd:xx)
			"if":
			{
				"reason":"wait(addr=xxx,keyID=xx,port=xx,otype=xx,dtype=xx,dlen=xx,data=xxx)"
				"send":"addr=xx,permit=xx,psw=xx,port=xx,otype=xx,dtype=xx,aID=xx,pdata=xx,len=xx;
				"delay":"1000"
				"send":"addr1,permit,psw,port,otype,dtype,aID,pdata,len";
				"delay":"1000"
				"send":"addr2,permit,psw,port,otype,dtype,aID,pdata,len";
			}
			"do":"R=R+1";
		}
	}
*/

//const uint8 devAddr[]={0x01,0x00,0x01,0x02,0x00,0x00,0x00,0x1B};
const uint8	devData[]={0x01,0xAB,0x34,0x12};
const address_t TestAddr=
{
		.addr={0x01,0x00,0x01,0x02,0x00,0x00,0x00,0x1B},
		.port = 0x01,
		.keyID = 0x00
};
const command_t TestCmd =
{
	.aID = aID_Gen_Type_Switch,
	.cmd_id = cmd_return,
	.option = 0x00,
};
void  vm_test0(void)
{
	void Virgo_init(char *str);
	cJSON *jbody;
	cJSON *jwhile;
	cJSON *jif;
	char *out;

	jbody = Scene_CreatBody();
	jwhile = Scene_AddWhile(jbody,"x < 3");
	//jwait = virgo_addif(jwhile,"if","wait(addr=010001020000001B,keyID=01,otype=01,dtype=00,aID=7890,dlen=04,data=01AB3412)");
	jif = Scene_Addif(jwhile,"if","time(z:8,date:2000-1-1,time:8:0:5)==-1&&time(z:8,date:2000-1-1,time:8:0:1)==1");//,&&
	Scene_AddAction(jif,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0003,data:012345");
	Scene_AddAction(jif,"delay","100");
	Scene_AddAction(jif,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:1000");
	Scene_AddAction(jif,"delay","100");
	Scene_AddAction(jif,"send","addr:0102030405060708,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:01AB");
	//jelse = Scene_AddElse(jwhile,"else");//,&&
	//Scene_AddAction(jelse,"do","x=0");
	Scene_AddAction(jif,"do","x++");
	out = cJSON_Print(jbody);
	//osPrintf(1,"%s",out);
	pluto_write_text_file("test0.vm",out);
	osFree(out);
}
void  vm_test1(void)
{
	void Virgo_init(char *str);
	cJSON *jbody;
	cJSON *jwhile;
	cJSON *jif;
	char *out;

	jbody = Scene_CreatBody();
	jwhile = Scene_AddWhile(jbody,"x < 10");
	//jwait = virgo_addif(jwhile,"if","wait(addr=010001020000001B,keyID=01,otype=01,dtype=00,aID=7890,dlen=04,data=01AB3412)");
	jif = Scene_Addif(jwhile,"if","wait(addr=010001010000000D,port=04,otype=08,dtype=01,aID=008010)");//,&&
	Scene_AddAction(jif,"send","addr:010001010000000D,keyID:00,port:01,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	//Scene_AddAction(jif,"delay","200");
	Scene_AddAction(jif,"send","addr:010001010000000D,keyID:00,port:02,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	//Scene_AddAction(jif,"delay","200");
	Scene_AddAction(jif,"send","addr:010001010000000D,keyID:00,port:03,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	//Scene_AddAction(jif,"delay","200");
	//Scene_AddAction(jif,"send","addr:010001010000000c,keyID:00,port:04,otype:02,dtype:01,aID:008010,dlen:0001,data:01");

	//Scene_AddAction(jif,"send","addr:010001010000000c,keyID:00,port:04,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	Scene_AddAction(jif,"delay","300");
	Scene_AddAction(jif,"send","addr:010001010000000D,keyID:00,port:03,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	//Scene_AddAction(jif,"delay","200");
	Scene_AddAction(jif,"send","addr:010001010000000D,keyID:00,port:02,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	//Scene_AddAction(jif,"delay","200");
	Scene_AddAction(jif,"send","addr:010001010000000D,keyID:00,port:01,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	Scene_AddAction(jif,"delay","200");
	//jelse = Scene_AddElse(jwhile,"else");//,&&
	//Scene_AddAction(jelse,"do","x=0");
	Scene_AddAction(jif,"do","x++");
	out = cJSON_Print(jbody);
	//osPrintf(1,"%s",out);
	pluto_write_text_file("test0.vm",out);
	osFree(out);
}
void  vm_test2(void)
{
	void Virgo_init(char *str);
	cJSON *jbody;
	cJSON *jwhile,*jwhile1;
	cJSON *jif;
	char *out;

	jbody = Scene_CreatBody();
	Scene_AddAction(jbody,"do","x=0");
	jwhile = Scene_AddWhile(jbody,"x < 3");
	//jwait = virgo_addif(jwhile,"if","wait(addr=010001020000001B,keyID=01,otype=01,dtype=01,aID=7890,dlen=04,data=01AB3412)");
	jif = Scene_Addif(jwhile,"if","wait(addr=010001010000000D,port=01,otype=0A,dtype=01,aID=008010)");//,&&
	Scene_AddAction(jif,"do","k=0");
	jwhile1 = Scene_AddWhile(jif,"k < 5");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:01,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:02,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:03,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	Scene_AddAction(jwhile1,"delay","200");
	//Scene_AddAction(jif,"send","addr:010001010000000c,keyID:00,port:04,otype:02,dtype:01,aID:008010,dlen:0001,data:01");

	//Scene_AddAction(jif,"send","addr:010001010000000c,keyID:00,port:04,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	//Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:03,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:02,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:01,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"do","k++");
	//jelse = Scene_AddElse(jwhile,"else");//,&&
	//Scene_AddAction(jelse,"do","x=0");
	Scene_AddAction(jif,"do","x++");
	out = cJSON_Print(jbody);
	//osPrintf(1,"%s",out);
	//Scene_CreateTask(out,"test");
	pluto_write_text_file("test0.vm",out);
	osFree(out);
}
void  vm_test3(void)
{
	void Virgo_init(char *str);
	cJSON *jbody;
	cJSON *jwhile,*jwhile1;
	cJSON *jif;
	char *out;

	jbody = Scene_CreatBody();
	Scene_AddAction(jbody,"do","x=0");
	jwhile = Scene_AddWhile(jbody,"x < 10");
	//jwait = virgo_addif(jwhile,"if","wait(addr=010001020000001B,keyID=01,otype=01,dtype=01,aID=7890,dlen=04,data=01AB3412)");
	jif = Scene_Addif(jwhile,"if","wait(addr=010001010000000D,port=04,otype=08,dtype=01,aID=008010)");//,&&
	Scene_AddAction(jif,"do","k=0");
	jwhile1 = Scene_AddWhile(jif,"k < 5");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:01,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	//Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:02,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	//Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:03,otype:02,dtype:01,aID:008010,dlen:0001,data:01");
	Scene_AddAction(jwhile1,"delay","200");
	//Scene_AddAction(jif,"send","addr:010001010000000c,keyID:00,port:04,otype:02,dtype:01,aID:008010,dlen:0001,data:01");

	//Scene_AddAction(jif,"send","addr:010001010000000c,keyID:00,port:04,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	//Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:03,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	//Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:02,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	//Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"send","addr:010001010000000D,keyID:00,port:01,otype:02,dtype:01,aID:008010,dlen:0001,data:00");
	Scene_AddAction(jwhile1,"delay","200");
	Scene_AddAction(jwhile1,"do","k++");
	//jelse = Scene_AddElse(jwhile,"else");//,&&
	//Scene_AddAction(jelse,"do","x=0");
	Scene_AddAction(jif,"do","x++");
	out = cJSON_Print(jbody);
	//osPrintf(1,"%s",out);
	//Scene_CreateTask(out,"test");
	pluto_write_text_file("test0.vm",out);
	osFree(out);
}
void  vm_test4(void)
{
	void Virgo_init(char *str);
	cJSON *jbody;
	cJSON *jwhile;
	cJSON *jif;
	char *out;

	jbody = Scene_CreatBody();
	jwhile = Scene_AddWhile(jbody,"x < 3");
	//jwait = virgo_addif(jwhile,"if","wait(addr=010001020000001B,keyID=01,otype=01,dtype=00,aID=7890,dlen=04,data=01AB3412)");
	jif = Scene_AddReadFor(jwhile,"expire(5000)&&wait(addr=0100010100000012,port=04,otype=08,dtype=01,aID=008010)");//,&&
	Scene_AddAction(jif,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0003,data:012345");
	Scene_AddAction(jif,"delay","100");
	Scene_AddAction(jif,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:1000");
	Scene_AddAction(jif,"delay","100");
	Scene_AddAction(jif,"send","addr:0102030405060708,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:01AB");
	//jelse = Scene_AddElse(jwhile,"else");//,&&
	//Scene_AddAction(jelse,"do","x=0");
	Scene_AddAction(jif,"do","x++");
	out = cJSON_Print(jbody);
	//osPrintf(1,"%s",out);
	//Scene_CreateTask(out,"test");
	pluto_write_text_file("test0.vm",out);
	osFree(out);
}
void  vm_test5(void)
{
	void Virgo_init(char *str);
	cJSON *jbody;
	cJSON *jwhile;
	cJSON *jif;
	cJSON *ex;
	char *out;

	jbody = Scene_CreatBody();
	jwhile = Scene_AddWhile(jbody,"x < 3");
	//jwait = virgo_addif(jwhile,"if","wait(addr=010001020000001B,keyID=01,otype=01,dtype=00,aID=7890,dlen=04,data=01AB3412)");
	jif = Scene_AddReadFor(jwhile,"expire(5000)&&wait(addr=0100010100000012,port=04,otype=08,dtype=01,aID=008010)");//,&&
	Scene_AddAction(jif,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0003,data:012345");
	Scene_AddAction(jif,"delay","100");
	Scene_AddAction(jif,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:1000");
	Scene_AddAction(jif,"delay","100");
	Scene_AddAction(jif,"send","addr:0102030405060708,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:01AB");
	//jelse = Scene_AddElse(jwhile,"else");//,&&
	//Scene_AddAction(jelse,"do","x=0");
	Scene_AddAction(jif,"do","x++");
	ex = Scene_AddException(jwhile);
	Scene_AddAction(ex,"send","addr:0102030405060708,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:01AB");
	Scene_AddAction(ex,"delay","100");
	Scene_AddAction(ex,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:1000");

	Scene_AddAction(jwhile,"do","x++");
	Scene_AddAction(jwhile,"delay","100");
	Scene_AddAction(jwhile,"send","addr:0102030405060708,key:12345678123456781234567812345678,keyID:00,port:01,otype:01,dtype:01,aID:1234,dlen:0004,data:1000");
	Scene_AddAction(jwhile,"delay","5000");
	out = cJSON_Print(jbody);
	//osPrintf(1,"%s",out);
	//Scene_CreateTask(out,"test");
	pluto_write_text_file("test0.vm",out);
	osFree(out);
}




