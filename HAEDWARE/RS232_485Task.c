#include "includes.h"



extern void RS485SendByte(uint8_t dat);
extern uint8_t RS485SendDatas(const uint8_t* s,uint16_t length);
extern uint8_t RS232SendDatas(const uint8_t* s,uint16_t length);

extern OS_EVENT * RS485SendQ;		//485 发送消息队列的指针
extern OS_EVENT * RS232SendQ;		//232 发送消息队列的指针
void RS485Task(void *pdata)
{
	u8 err;
	u8 * p485Send;
	u16 frameLen = 0;
	while(1)
	{
		p485Send = OSQPend(RS485SendQ,0,&err);   //等待要发送的485数据
		
		memcpy(&frameLen,p485Send,2);
		RS485SendDatas(&p485Send[2],frameLen);	 //发送485数据
		Mem_free((void**)(&p485Send));			 //释放内存块
	}
}




void RS232Task(void *pdata)
{
	u8 err;
	u8 * p232Send;
	u16 frameLen = 0;
	while(1)
	{
		p232Send = OSQPend(RS232SendQ,0,&err);   //等待要发送的 232 数据
		
		memcpy(&frameLen,p232Send,2);
		
		RS232SendDatas(&p232Send[2],frameLen);	 //发送232数据
		Mem_free((void**)(&p232Send));			 //释放内存块
	}
}








