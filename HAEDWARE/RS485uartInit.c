/************************************************************
*公司名称:             上海星融汽车科技
*文件名称:      RS485_uart_init.c
*作者姓名:      洪春峰  QQ:1341189542
*文件内容:      RS232  串口初始化、收发
*编写日期:       2018-4-10
*************************************************************/

#include "includes.h"

extern pCIR_QUEUE send485_Q;	//指向 485 串口发送队列  的指针
extern pSTORE     rece485_S;	//指向 485 串口接收数据堆的指针
extern OS_EVENT * RS485SendQ;				//485 发送消息队列的指针
void RS485SendByte(uint8_t dat)
{
#if OS_CRITICAL_METHOD == 3u           /* Allocate storage for CPU status register           */
	OS_CPU_SR  cpu_sr = 0u;
#endif
	if(CirQ_GetLength(send485_Q) > 0)
	{
		OS_ENTER_CRITICAL();
		CirQ_OnePush(send485_Q,dat);
		OS_EXIT_CRITICAL();
	}
	else
		USART_SendData(UART5, dat);
}
uint8_t RS485SendDatas(const uint8_t* s,uint16_t length)
{
#if OS_CRITICAL_METHOD == 3u           /* Allocate storage for CPU status register           */
	OS_CPU_SR  cpu_sr = 0u;
#endif
//	uint8_t data;
	if(length < 1 || length >500)
		return 1;
	OS_ENTER_CRITICAL();
	
	if(CirQ_Pushs(send485_Q,s,length) != OK)
	{
		OS_EXIT_CRITICAL();
		return 2;
	}
	OS_EXIT_CRITICAL();
	if(CirQ_GetLength(send485_Q) > 0)
	{
		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
	}
	return  0;
}

static NORMAL_STATE RS485State;
static u8 rx5Data = 0;
static u8 tx5Data = 0;
static u8 clear5;
u8* p485Rece;
static uint16_t frameLen;
extern OS_EVENT * testMbox;		//通讯检测 消息邮箱
static uint16_t i;
void UART5_IRQHandler(void)
{
	OSIntEnter();//系统进入中断服务程序
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//接收到数据
  	{
    	rx5Data = USART_ReceiveData(UART5);
		Store_Push(rece485_S,rx5Data);   
		USART_ClearITPendingBit(UART5, USART_IT_RXNE) ;
	}
	else if(USART_GetITStatus(UART5, USART_IT_TC) != RESET)//发送完毕  移位寄存器空
	{
		if(CirQ_GetLength(send485_Q) > 0)
		{
			RS485State = CirQ_Pop(send485_Q,&tx5Data);
			if(RS485State == OK)
				USART_SendData(UART5,tx5Data);
		}
		USART_ClearITPendingBit(UART5, USART_IT_TC);
	}
	else if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)//空  发送寄存器空
	{
		if(CirQ_GetLength(send485_Q) > 0)
		{
			RS485State = CirQ_Pop(send485_Q,&tx5Data);
			if(RS485State == OK)
				USART_SendData(UART5,tx5Data);
		}
		USART_ITConfig(UART5,USART_IT_TXE,DISABLE);
		USART_ClearITPendingBit(UART5,USART_IT_TXE);
	}
	else if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)//接收完一帧数据
	{
		clear5 = UART5->SR;
		clear5 = UART5->DR;
		clear5 = clear5;
		frameLen = Store_Getlength(rece485_S);
		if(varOperaRun.sysMode == TEST_PORT_SEND_MODE)      //主测 模式
		{
			p485Rece = Mem_malloc(frameLen);
			if(p485Rece != NULL)	//内存块申请成功
			{
				Store_Getdates(p485Rece,rece485_S,frameLen);
				if(OSMboxPost(testMbox,p485Rece) != OS_ERR_NONE)//推送不成功需要释放内存块
				{
					Mem_free((void **)(&p485Rece));
					Store_Clear(rece485_S);//舍弃本次接收的数据
				}
			}
		}else if(varOperaRun.sysMode == TEST_PORT_RECV_MODE)//被测 模式
		{
			p485Rece = Mem_malloc(frameLen+2);
			if(p485Rece != NULL)	//内存块申请成功
			{
				memcpy(p485Rece,&frameLen,2);
				Store_Getdates(&p485Rece[2],rece485_S,frameLen);
				for(i = 0;i < frameLen;i++)
				{
					p485Rece[i+2] ++;
				}
				if( OSQPost(RS485SendQ,p485Rece)!=OS_ERR_NONE)
					Mem_free((void **)(&p485Rece));
				Store_Clear(rece485_S);//舍弃本次接收的数据
			}
		}
		else 
		{
			Store_Clear(rece485_S);
		}
	}
	OSIntExit();  //中断服务结束，系统进行任务调度
}

void RS485_UartConfig(u32 baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; // 中断初始化

	/* 打开GPIOC时钟、AFIO时钟，UART5时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12; //UART5 TX；
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure); //端口C；

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2; //UART5 RX；
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure); //端口D；
	
//	GPIO_PinRemapConfig(GPIO_Remap_UART5,ENABLE);
								
	/* USART 配置 */
	USART_DeInit(UART5);
	USART_InitStructure.USART_BaudRate   = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
	/* 使能UART5收发中断 */
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART5, USART_IT_TC, ENABLE);		   
	USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
	/* 使能 USART2 */
	USART_Cmd(UART5, ENABLE);

	   
	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;		//串口中断设置
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //响应优先级；
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}






