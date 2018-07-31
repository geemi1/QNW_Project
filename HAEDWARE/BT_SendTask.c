#include "includes.h"
void LoadTestDataSend(void);
extern OS_EVENT * BXWSendSem;	//便携王通过蓝牙发送信号量
extern pSTORE     BXW_CAN;		//便携王接收 CAN 泵信息
extern uint8_t loadCmd;
void BTSendTask(void *pdata)
{
	uint8_t err;
	uint8_t * ptrToSend;
	uint16_t  canDatLen;
	OSTimeDlyHMSM(0,0,5,0);
//	WIFI_Init();
	while(1)
	{
		OSSemPend(BXWSendSem,50,&err);
		if(err == OS_ERR_NONE)
		{
			ptrToSend = Mem_malloc(500);
			if(ptrToSend == NULL)
			{
				continue;
			}
			canDatLen = Store_Getlength(BXW_CAN);
			memcpy(ptrToSend,(u8 *)&varBXWRun,sizeof(varBXWRun));
			if(canDatLen>0)
				Store_Getdates(&ptrToSend[sizeof(varBXWRun)],BXW_CAN,canDatLen);
			else//安卓端要求没有 CAN 包的时候，要发一个随机包 09 00 41 00 00 
			{
				canDatLen = 15;
				ptrToSend[sizeof(varBXWRun)] = 13;
				ptrToSend[sizeof(varBXWRun)+2] = 9;
				ptrToSend[sizeof(varBXWRun)+4] = 65;
			}
			DateToSend(ptrToSend,sizeof(varBXWRun)+canDatLen,0x40,0x0F);
			Mem_free((void **)&ptrToSend);
		}
		else if(varOperaRun.sysMode == LOAD_TEST_MODE)
		{
			LoadTestDataSend();//向上位机发送测试信息
		}
	}
}

void LoadTestDataSend(void)
{
	uint8_t * ptr;
	ptr = Mem_malloc(15);
	switch(loadCmd)
	{
/************    负载测试    ****************/
		case 0x11:case 0x12:case 0x13:case 0x14:
		case 0x15:case 0x16:case 0x17:case 0x18:
			ptr[0] = 0x07;ptr[1] = 0x00;//长度
			ptr[2] = loadCmd;
			memcpy(&ptr[3],(uint8_t *)&varBXWRun.elecPWM[loadCmd-0x11],2);//通道电流
			memcpy(&ptr[5],(uint8_t *)&varBXWRun.sysVol,2);//系统电压
			memcpy(&ptr[7],(uint8_t *)&varBXWRun.sysCurr,2);//系统电流
			DateToSend(ptr,9,0x02,0x8F);
			break;
/************       通断测试    ****************/
		case 0x19:
			ptr[0] = 0x03;ptr[1] = 0x00;	//长度
			ptr[2] = loadCmd;
			ptr[3] = 0;//todo:     0-通  1 - 断
			ptr[4] = 0;	
			DateToSend(ptr,5,0x02,0x8F);
			break;
/************       DCM测试    ****************/
		case 0x1A:case 0x1B:case 0x1C:
			ptr[0] = 0x07;ptr[1] = 0x00;	//长度
			ptr[2] = loadCmd;
			ptr[3] = 0;//todo:     0 - 正转  1 - 反转
			ptr[4] = 0;	
			memcpy(&ptr[5],(uint8_t *)&varBXWRun.sysVol,2);//系统电压
			memcpy(&ptr[7],(uint8_t *)&varBXWRun.sysCurr,2);//系统电流
			DateToSend(ptr,9,0x02,0x8F);
			break;
/************       六路电压测试    ****************/
		case 0x21:case 0x22:case 0x23:case 0x24:case 0x25:case 0x26:
			ptr[0] = 0x03;ptr[1] = 0x00;	//长度
			ptr[2] = loadCmd;
			memcpy(&ptr[3],(uint8_t *)&varBXWRun.Vol[loadCmd-0x21],2);//被测电压
			DateToSend(ptr,5,0x02,0x8F);
			break;
/************       四路电阻测试    ****************/
		case 0x31:case 0x32:case 0x33:case 0x34:
			ptr[0] = 0x03;ptr[1] = 0x00;	//长度
			ptr[2] = loadCmd;
			memcpy(&ptr[3],(uint8_t *)&varBXWRun.resistor[loadCmd-0x31],2);//被测电阻
			DateToSend(ptr,5,0x02,0x8F);
			break;
		default:break;
	}
	Mem_free((void **)&ptr);
}



