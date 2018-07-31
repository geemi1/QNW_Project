#include "includes.h"
void LoadTestDataSend(void);
extern OS_EVENT * BXWSendSem;	//��Я��ͨ�����������ź���
extern pSTORE     BXW_CAN;		//��Я������ CAN ����Ϣ
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
			else//��׿��Ҫ��û�� CAN ����ʱ��Ҫ��һ������� 09 00 41 00 00 
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
			LoadTestDataSend();//����λ�����Ͳ�����Ϣ
		}
	}
}

void LoadTestDataSend(void)
{
	uint8_t * ptr;
	ptr = Mem_malloc(15);
	switch(loadCmd)
	{
/************    ���ز���    ****************/
		case 0x11:case 0x12:case 0x13:case 0x14:
		case 0x15:case 0x16:case 0x17:case 0x18:
			ptr[0] = 0x07;ptr[1] = 0x00;//����
			ptr[2] = loadCmd;
			memcpy(&ptr[3],(uint8_t *)&varBXWRun.elecPWM[loadCmd-0x11],2);//ͨ������
			memcpy(&ptr[5],(uint8_t *)&varBXWRun.sysVol,2);//ϵͳ��ѹ
			memcpy(&ptr[7],(uint8_t *)&varBXWRun.sysCurr,2);//ϵͳ����
			DateToSend(ptr,9,0x02,0x8F);
			break;
/************       ͨ�ϲ���    ****************/
		case 0x19:
			ptr[0] = 0x03;ptr[1] = 0x00;	//����
			ptr[2] = loadCmd;
			ptr[3] = 0;//todo:     0-ͨ  1 - ��
			ptr[4] = 0;	
			DateToSend(ptr,5,0x02,0x8F);
			break;
/************       DCM����    ****************/
		case 0x1A:case 0x1B:case 0x1C:
			ptr[0] = 0x07;ptr[1] = 0x00;	//����
			ptr[2] = loadCmd;
			ptr[3] = 0;//todo:     0 - ��ת  1 - ��ת
			ptr[4] = 0;	
			memcpy(&ptr[5],(uint8_t *)&varBXWRun.sysVol,2);//ϵͳ��ѹ
			memcpy(&ptr[7],(uint8_t *)&varBXWRun.sysCurr,2);//ϵͳ����
			DateToSend(ptr,9,0x02,0x8F);
			break;
/************       ��·��ѹ����    ****************/
		case 0x21:case 0x22:case 0x23:case 0x24:case 0x25:case 0x26:
			ptr[0] = 0x03;ptr[1] = 0x00;	//����
			ptr[2] = loadCmd;
			memcpy(&ptr[3],(uint8_t *)&varBXWRun.Vol[loadCmd-0x21],2);//�����ѹ
			DateToSend(ptr,5,0x02,0x8F);
			break;
/************       ��·�������    ****************/
		case 0x31:case 0x32:case 0x33:case 0x34:
			ptr[0] = 0x03;ptr[1] = 0x00;	//����
			ptr[2] = loadCmd;
			memcpy(&ptr[3],(uint8_t *)&varBXWRun.resistor[loadCmd-0x31],2);//�������
			DateToSend(ptr,5,0x02,0x8F);
			break;
		default:break;
	}
	Mem_free((void **)&ptr);
}



