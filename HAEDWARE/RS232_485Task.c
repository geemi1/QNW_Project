#include "includes.h"



extern void RS485SendByte(uint8_t dat);
extern uint8_t RS485SendDatas(const uint8_t* s,uint16_t length);
extern uint8_t RS232SendDatas(const uint8_t* s,uint16_t length);

extern OS_EVENT * RS485SendQ;		//485 ������Ϣ���е�ָ��
extern OS_EVENT * RS232SendQ;		//232 ������Ϣ���е�ָ��
void RS485Task(void *pdata)
{
	u8 err;
	u8 * p485Send;
	u16 frameLen = 0;
	while(1)
	{
		p485Send = OSQPend(RS485SendQ,0,&err);   //�ȴ�Ҫ���͵�485����
		
		memcpy(&frameLen,p485Send,2);
		RS485SendDatas(&p485Send[2],frameLen);	 //����485����
		Mem_free((void**)(&p485Send));			 //�ͷ��ڴ��
	}
}




void RS232Task(void *pdata)
{
	u8 err;
	u8 * p232Send;
	u16 frameLen = 0;
	while(1)
	{
		p232Send = OSQPend(RS232SendQ,0,&err);   //�ȴ�Ҫ���͵� 232 ����
		
		memcpy(&frameLen,p232Send,2);
		
		RS232SendDatas(&p232Send[2],frameLen);	 //����232����
		Mem_free((void**)(&p232Send));			 //�ͷ��ڴ��
	}
}








