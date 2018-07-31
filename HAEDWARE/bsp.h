/************************************************************
*��˾����:             �Ϻ����������Ƽ�
*�ļ�����:      bsp.h
*��������:      �鴺��  QQ:1341189542
*�ļ�����:      �������衢��ʱ���ȵĳ�ʼ��
*��д����:       2018-4-11
*************************************************************/



#ifndef __BSP_H__
#define __BSP_H__
#include "memblock.h"
#include "sys.h"
#include "includes.h"
#include "globalvar.h"
////////               �������ݶ���              ///////////////

#define SYSTEMTIME   72000000		//ϵͳʱ��

#define OPEN			1		//��
#define CLOSE			0		//�ر�
#define SMALLCycle 		8999	//PWM С���ڼĴ�����ֵ

#define STEP_START_FRE	900	//���������ʼ��������  

#define SMALL_PWM		1		//Сռ�ձ�ģʽ
#define BIG_PWM			2		//��ռ�ձ�ģʽ
#define STEP_MOTOR		3		//�������ģʽ
#define DC_MOTOR		4		//ֱ�����ģʽ

#define BT_MODE			1		//ϵͳ������������ģʽ
#define WIFI_MODE		2		//ϵͳ����WIFI����ģʽ
#define USB_MODE        3       //ϵͳ����USB����ģʽ

#define ADBUFFSIZE		12      //12· AD ͨ��

//--------------ADS1243-COMMAND-------------------------
#define ADS1243_RDATA       0x01
#define ADS1243_RDATAC      0x03
#define ADS1243_STOPC       0x0f
#define ADS1243_RREG        0x10
#define ADS1243_WREG        0x50
#define ADS1243_SELFCAL     0xf0
#define ADS1243_SELFOCAL    0xf1
#define ADS1243_SELFGCAL    0xf2
#define ADS1243_SYSOCAL     0xf3
#define ADS1243_SYSGCAL     0xf4
#define ADS1243_WAKEUP      0xfb
#define ADS1243_DSYNC       0xfc
#define ADS1243_SLEEP       0xfd
#define ADS1243_RESET       0xfe

//--------------ADS1243-Register------------------------
#define  ADS1243_ADDER_SETUP   0x00
#define  ADS1243_ADDER_MUX     0x01
#define  ADS1243_ADDER_ACR     0x02
#define  ADS1243_ADDER_ODAC    0x03
#define  ADS1243_ADDER_DIO     0x04
#define  ADS1243_ADDER_DIR     0x05
#define  ADS1243_ADDER_IOCON   0x06
#define  ADS1243_ADDER_OCR0    0x07
#define  ADS1243_ADDER_OCR1    0x08
#define  ADS1243_ADDER_OCR2    0x09
#define  ADS1243_ADDER_FSR0    0x0A
#define  ADS1243_ADDER_FSR1    0x0B
#define  ADS1243_ADDER_FSR2    0x0C
#define  ADS1243_ADDER_DOR2    0x0D
#define  ADS1243_ADDER_DOR1    0x0E
#define  ADS1243_ADDER_DOR0    0x0F


#pragma pack(1)             //1�ֽڶ���
//8·PWM
__packed typedef struct
{
	uint8_t numPWMOC;		//��������OCͨ����
	uint8_t ocType;			//������������OC�����Σ�С����
	
	GPIO_TypeDef * GPIOPWM;	//������  PWM ���	��Ҫ��ʼ��
	uint16_t  pinPWM;		//����		Out_E   ��Ҫ��ʼ��
	
	GPIO_TypeDef * GPIO_In;	//������ ���� ����	��Ҫ��ʼ��
	uint16_t  pinIn;		//״̬��ƽ	Out_U	��Ҫ��ʼ��
	uint8_t   pinState;		//���ŵ�ƽ			��Ҫ��ʼ��
	
	TIM_TypeDef * TIMsmall;	//Сռ�ձȵĶ�ʱ��		��Ҫ��ʼ��
	uint8_t  OCNumSmall;	//Сռ�ձ����ڵ�ͨ��	��Ҫ��ʼ��

	uint8_t  state;			//�Ƿ���Ҫ����
	uint8_t  level;			//Ĭ�ϵ�ƽ(������ʱ�ĵ�ƽ)
	uint16_t dutyCycleBig;  //��ռ�ձ�
	uint16_t freBig;		//��Ƶ��
	uint16_t dutyCycleSma;	//Сռ�ձ�
	uint16_t freSma;		//СƵ��
	uint8_t num;            //��ͨ��������������
}PWM_Control; 

//ֱ������������������
__packed typedef struct
{
	uint8_t numPWMOC;		//��������OCͨ����
	uint8_t stepErr;		//1-��ȷ�Ľ��ߣ�0-����Ľ���
	GPIO_TypeDef * GPIO_FWD;//������  ���з���
	uint16_t  fwd_Pin;		//����
	
	GPIO_TypeDef * GPIO_REV;//������  PWM ���
	uint16_t  rev_Pin;		//����
	
	GPIO_TypeDef * GPIO_Err;//������  ����
	uint16_t  err_Pin;		//����

	uint8_t  state;         //�Ƿ�Ҫ����PWM״̬
	uint8_t  fwd;			//���з���
	uint16_t dutyCycle;		//��ռ�ձ�
	uint16_t fre;			//��Ƶ��
	
}DCM_Control;

__packed typedef struct
{
	uint8_t	 	isOPEN;		//�˽ṹ���Ƿ�ռ��
	TIM_TypeDef * TIMBig;	//��ʼ����ʱ��д��
	uint8_t		OCNumBig;	//�ڼ�ͨ��  ��ʼ����ʱ��д��
	uint16_t  	IT_Source;	//�ж�Դ���Ƿ������ر��жϣ��жϱ�־λ   ��ʼ����ʱ��д��
	
	uint8_t		pwmType;	//��������  0Ĭ�� 1-Сռ�ձ� 2-��ռ�ձ�  3-������� 4-ֱ�����
	uint8_t  	hlSet;		//�ڶ�ʱ�������о������ŵĸߵ͵�ƽ
	
	uint8_t     direct;		//����	
	uint16_t 	hTime;		//�ߵ�ƽ����ʱ��  PWM����	(us)
	uint16_t 	lTime;		//�͵�ƽ����ʱ��			(us)
	uint16_t 	timeCnt;	//time����
	uint32_t 	period;		//����Ƶ�ʼ���
	
	uint32_t 	periodCalc;	//�����������ʱ������ֵ
	uint8_t     stepNum;	//����
	
	uint16_t 	timeWork;		//����ʱ�䣬5��֮���޶������͹ر���Ӧ�˿�	

	PWM_Control * pwmControl;	//8·PWM	�ṹ��
	DCM_Control * dcmControl;	//ֱ����� 	�ṹ��
	DCM_Control * dcmControl1;	//������� 	�ṹ��
	DCM_Control * dcmControl2;	//������� 	�ṹ��
}TIMEOUTPUT_pwm;



#pragma pack () 

////////               ����ö�����Ͷ���              ///////////////
typedef enum
{
	CANBAUD_100K = 0,
	CANBAUD_250K,
	CANBAUD_500K,
	CANBAUD_1M
}CANBAUD_Enum;//CAN �����ʱ�־


//////               ���ź궨��             //////////////

#define HCP_595 PDout(15)//ʱ�ӣ���������λ
#define TCP_595 PCout(8)//������ʱ��λ�Ĵ������ݽ������ݴ洢�Ĵ������½������ݲ���
#define SDI_595 PCout(9)//�������������

//ֱ�����  ������� ���Ʒ��� �� PWM �ĺ�
#define DCM1_ERR    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)

#define DCM2_ERR    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)

#define DCM3_ERR    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)

#define KStartPin PCout(10)

////// ***************  ��������   **************//////////////

void SystemBspInit(void);               //ϵͳ�����ʼ��
void SysTickInit(void);                 //ϵͳ�δ��ʼ��

void ADC_DMA_Init(void); 				//STM32 ADC ��ʼ��
void SPI3_ConfigInit(void);				//ADS1243_1 SPI3��ʼ��
void ADS1243_Init1(void);				//ADS1243_1 оƬ��ʼ��
u32 ADS1243_ReadDataC(u8 channel);		//ADS1243_1 ��ȡͨ��ֵ

void BT_UartConfig(uint32_t baud);      //BT ���ڳ�ʼ��
void WIFI_UartConfig(uint32_t baud);	//WIFI ���ڳ�ʼ��
void WIFI_Init(void);					//WIFI ���ó�ʼ��

void TIM2_Init(void);                   //��ʱ��2 ��ʼ��
void DateToSend(uint8_t * ptrToSend,uint16_t datLen,uint8_t frameType,uint8_t serverType);

void WIFIDataToSend(uint8_t * ptrToSend,uint16_t datLen,uint8_t frameType,uint8_t serverType);


void RS232_UartConfig(uint32_t baud);   //RS232 ���ڳ�ʼ��
void TIM6_Init(void);                   //��ʱ��6��ʼ��

void RS485_UartConfig(u32 baud);        //485 ͨѶ��ʼ��

void PWMInit(void);                     //PWM��ز���
void ComTest(void);						//����ͨѶ����
void LoadTest(void );					//���� ����
void CAN1Config(uint32_t canId,uint32_t canIde,uint32_t mask,uint32_t baud);                   //CAN1����
void CAN2Config(uint32_t canId,uint32_t canIde,uint32_t mask,uint32_t baud);					 //CAN2����

void Can1_SendData(u32 canId,u32 ide,u8* pdat); // CAN1 ��������
void Can2_SendData(u32 canId,u32 ide,u8* pdat); // CAN2 ��������

void K_Config(u32 baud);                //K ������
void KLineFastInit(uint8_t *ptrData,uint16_t datLen);//K�߿��ٳ�ʼ��
uint8_t KLineSendDatas(const uint8_t* s,uint16_t length);//K�߷�������

void LS595Init(void);                   //595 IO�ڳ�ʼ��
void LS595Deal(u16 data595);			//����Ƭ595д������


void CANChannelCheck(u8 ch,u8 res); //CAN �л�


u8 bmp280Init(void);				//IIC ��ʼ��
void bmp280GetData(float* pressure,float* temperature,float* asl);//��ȡ��ѹ�¶�
			

/*************************   ȫ�ֱ���   ******************************/



#endif














