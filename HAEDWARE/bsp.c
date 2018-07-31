/************************************************************
*��˾����:             �Ϻ����������Ƽ�
*�ļ�����:      bsp.c
*��������:      �鴺��  QQ:1341189542
*�ļ�����:      ϵͳʱ�ӡ�IO���жϵȵĳ�ʼ��
*��д����:       2018-4-10
*************************************************************/
#include "bsp.h"
#include "includes.h"
void ALL_GPIO_Init(void);

//ϵͳʱ�ӵδ��ʼ��
void SysTickInit(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
}

void SystemBspInit(void)
{
	SysTickInit();				//ʱ�ӳ�ʼ��
	ALL_GPIO_Init();			//��ʼ������
	GloVar_Init();				//ȫ�ֱ�����ʼ��
	
	BT_UartConfig(115200);
//	WIFI_UartConfig(15200);

	ADC_DMA_Init();				//ADC  ���������ʼ��
	TIM6_Init();				//��ʱ 1ms ��ʱʱ��
	PWMInit();					//PWM  �����ʼ��
	LS595Init();				//595  ���ų�ʼ��

	RS232_UartConfig(9600);		//RS232 ���ڳ�ʼ��
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
	
	//�ⲿ 24V ����  4  todo:���ڿɵ� 
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13|GPIO_Pin_14;         
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//0 1-24V     X 0 - 12V    1 1-������
	GPIO_SetBits(GPIOC,GPIO_Pin_14);	//Pwr_24  �ϵ繩��
	GPIO_SetBits(GPIOC,GPIO_Pin_13);	//Pwr_Sw

	/*************     DCM����������ų�ʼ��     ****************/
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;		//DCM3 err
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;	//��������
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9 ;		//DCM3 �������
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;	//���
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//								  DCM3 fwd   DCM2 fwd   DCM2 rev   DCM1 fwd   DCM1 rev 
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	//									DCM2 err    DCM2 err
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_1|GPIO_Pin_4;	
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IPU;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	/*************     ģ��IIC ���ų�ʼ��     ****************/
}


















