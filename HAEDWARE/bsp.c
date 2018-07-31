/************************************************************
*公司名称:             上海星融汽车科技
*文件名称:      bsp.c
*作者姓名:      洪春峰  QQ:1341189542
*文件内容:      系统时钟、IO、中断等的初始化
*编写日期:       2018-4-10
*************************************************************/
#include "bsp.h"
#include "includes.h"
void ALL_GPIO_Init(void);

//系统时钟滴答初始化
void SysTickInit(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
}

void SystemBspInit(void)
{
	SysTickInit();				//时钟初始化
	ALL_GPIO_Init();			//初始化引脚
	GloVar_Init();				//全局变量初始化
	
	BT_UartConfig(115200);
//	WIFI_UartConfig(15200);

	ADC_DMA_Init();				//ADC  外设参数初始化
	TIM6_Init();				//定时 1ms 基时时钟
	PWMInit();					//PWM  外设初始化
	LS595Init();				//595  引脚初始化

	RS232_UartConfig(9600);		//RS232 串口初始化
	RS485_UartConfig(9600);
	bmp280Init();

	K_Config(10400);
//	CAN1Config(0x00000000,CAN_ID_EXT,0,250000);
//	CAN2Config(0x00000000,CAN_ID_EXT,0,250000);
}
void ALL_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	
	//外部 24V 供电  4  todo:后期可调 
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13|GPIO_Pin_14;         
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//0 1-24V     X 0 - 12V    1 1-不供电
	GPIO_SetBits(GPIOC,GPIO_Pin_14);	//Pwr_24  上电供电
	GPIO_SetBits(GPIOC,GPIO_Pin_13);	//Pwr_Sw

	/*************     DCM电机控制引脚初始化     ****************/
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;		//DCM3 err
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9 ;		//DCM3 波形输出
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;	//输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//								  DCM3 fwd   DCM2 fwd   DCM2 rev   DCM1 fwd   DCM1 rev 
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	//									DCM2 err    DCM2 err
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_1|GPIO_Pin_4;	
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	/*************     模拟IIC 引脚初始化     ****************/
}


















