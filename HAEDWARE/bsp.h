/************************************************************
*公司名称:             上海星融汽车科技
*文件名称:      bsp.h
*作者姓名:      洪春峰  QQ:1341189542
*文件内容:      各种外设、定时器等的初始化
*编写日期:       2018-4-11
*************************************************************/



#ifndef __BSP_H__
#define __BSP_H__
#include "memblock.h"
#include "sys.h"
#include "includes.h"
#include "globalvar.h"
////////               常用数据定义              ///////////////

#define SYSTEMTIME   72000000		//系统时钟

#define OPEN			1		//打开
#define CLOSE			0		//关闭
#define SMALLCycle 		8999	//PWM 小周期寄存器的值

#define STEP_START_FRE	900	//步进电机起始运行周期  

#define SMALL_PWM		1		//小占空比模式
#define BIG_PWM			2		//大占空比模式
#define STEP_MOTOR		3		//步进电机模式
#define DC_MOTOR		4		//直流电机模式

#define BT_MODE			1		//系统处于蓝牙连接模式
#define WIFI_MODE		2		//系统处于WIFI连接模式
#define USB_MODE        3       //系统处于USB连接模式

#define ADBUFFSIZE		12      //12路 AD 通道

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


#pragma pack(1)             //1字节对齐
//8路PWM
__packed typedef struct
{
	uint8_t numPWMOC;		//索引到的OC通道号
	uint8_t ocType;			//关联的是哪种OC，大波形，小波形
	
	GPIO_TypeDef * GPIOPWM;	//引脚组  PWM 输出	需要初始化
	uint16_t  pinPWM;		//引脚		Out_E   需要初始化
	
	GPIO_TypeDef * GPIO_In;	//引脚组 电流 输入	需要初始化
	uint16_t  pinIn;		//状态电平	Out_U	需要初始化
	uint8_t   pinState;		//引脚电平			需要初始化
	
	TIM_TypeDef * TIMsmall;	//小占空比的定时器		需要初始化
	uint8_t  OCNumSmall;	//小占空比所在的通道	需要初始化

	uint8_t  state;			//是否需要运行
	uint8_t  level;			//默认电平(不工作时的电平)
	uint16_t dutyCycleBig;  //大占空比
	uint16_t freBig;		//大频率
	uint16_t dutyCycleSma;	//小占空比
	uint16_t freSma;		//小频率
	uint8_t num;            //本通道所在数组的序号
}PWM_Control; 

//直流电机、步进电机控制
__packed typedef struct
{
	uint8_t numPWMOC;		//索引到的OC通道号
	uint8_t stepErr;		//1-正确的接线，0-错误的接线
	GPIO_TypeDef * GPIO_FWD;//引脚组  运行方向
	uint16_t  fwd_Pin;		//引脚
	
	GPIO_TypeDef * GPIO_REV;//引脚组  PWM 输出
	uint16_t  rev_Pin;		//引脚
	
	GPIO_TypeDef * GPIO_Err;//引脚组  输入
	uint16_t  err_Pin;		//引脚

	uint8_t  state;         //是否要开启PWM状态
	uint8_t  fwd;			//运行方向
	uint16_t dutyCycle;		//大占空比
	uint16_t fre;			//大频率
	
}DCM_Control;

__packed typedef struct
{
	uint8_t	 	isOPEN;		//此结构体是否被占用
	TIM_TypeDef * TIMBig;	//初始化的时候写死
	uint8_t		OCNumBig;	//第几通道  初始化的时候，写死
	uint16_t  	IT_Source;	//中断源，是否开启、关闭中断，中断标志位   初始化的时候写死
	
	uint8_t		pwmType;	//波形类型  0默认 1-小占空比 2-大占空比  3-步进电机 4-直流电机
	uint8_t  	hlSet;		//在定时器运行中决定引脚的高低电平
	
	uint8_t     direct;		//方向	
	uint16_t 	hTime;		//高电平持续时间  PWM波形	(us)
	uint16_t 	lTime;		//低电平持续时间			(us)
	uint16_t 	timeCnt;	//time计数
	uint32_t 	period;		//计算频率计数
	
	uint32_t 	periodCalc;	//步进电机调速时的周期值
	uint8_t     stepNum;	//步号
	
	uint16_t 	timeWork;		//工作时间，5秒之内无动作，就关闭相应端口	

	PWM_Control * pwmControl;	//8路PWM	结构体
	DCM_Control * dcmControl;	//直流电机 	结构体
	DCM_Control * dcmControl1;	//步进电机 	结构体
	DCM_Control * dcmControl2;	//步进电机 	结构体
}TIMEOUTPUT_pwm;



#pragma pack () 

////////               常用枚举类型定义              ///////////////
typedef enum
{
	CANBAUD_100K = 0,
	CANBAUD_250K,
	CANBAUD_500K,
	CANBAUD_1M
}CANBAUD_Enum;//CAN 波特率标志


//////               引脚宏定义             //////////////

#define HCP_595 PDout(15)//时钟，上升沿移位
#define TCP_595 PCout(8)//上升沿时移位寄存器数据进入数据存储寄存器，下降沿数据不变
#define SDI_595 PCout(9)//串行数据输入端

//直流电机  步进电机 控制方向 和 PWM 的宏
#define DCM1_ERR    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)

#define DCM2_ERR    GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)

#define DCM3_ERR    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)

#define KStartPin PCout(10)

////// ***************  函数声明   **************//////////////

void SystemBspInit(void);               //系统外设初始化
void SysTickInit(void);                 //系统滴答初始化

void ADC_DMA_Init(void); 				//STM32 ADC 初始化
void SPI3_ConfigInit(void);				//ADS1243_1 SPI3初始化
void ADS1243_Init1(void);				//ADS1243_1 芯片初始化
u32 ADS1243_ReadDataC(u8 channel);		//ADS1243_1 读取通道值

void BT_UartConfig(uint32_t baud);      //BT 串口初始化
void WIFI_UartConfig(uint32_t baud);	//WIFI 串口初始化
void WIFI_Init(void);					//WIFI 配置初始化

void TIM2_Init(void);                   //定时器2 初始化
void DateToSend(uint8_t * ptrToSend,uint16_t datLen,uint8_t frameType,uint8_t serverType);

void WIFIDataToSend(uint8_t * ptrToSend,uint16_t datLen,uint8_t frameType,uint8_t serverType);


void RS232_UartConfig(uint32_t baud);   //RS232 串口初始化
void TIM6_Init(void);                   //定时器6初始化

void RS485_UartConfig(u32 baud);        //485 通讯初始化

void PWMInit(void);                     //PWM相关操作
void ComTest(void);						//测试通讯程序
void LoadTest(void );					//负载 测试
void CAN1Config(uint32_t canId,uint32_t canIde,uint32_t mask,uint32_t baud);                   //CAN1配置
void CAN2Config(uint32_t canId,uint32_t canIde,uint32_t mask,uint32_t baud);					 //CAN2配置

void Can1_SendData(u32 canId,u32 ide,u8* pdat); // CAN1 发送数据
void Can2_SendData(u32 canId,u32 ide,u8* pdat); // CAN2 发送数据

void K_Config(u32 baud);                //K 线配置
void KLineFastInit(uint8_t *ptrData,uint16_t datLen);//K线快速初始化
uint8_t KLineSendDatas(const uint8_t* s,uint16_t length);//K线发送数据

void LS595Init(void);                   //595 IO口初始化
void LS595Deal(u16 data595);			//向两片595写入数据


void CANChannelCheck(u8 ch,u8 res); //CAN 切换


u8 bmp280Init(void);				//IIC 初始化
void bmp280GetData(float* pressure,float* temperature,float* asl);//获取气压温度
			

/*************************   全局变量   ******************************/



#endif














