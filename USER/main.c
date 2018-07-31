
#include "includes.h"
#include "apptask.h"
#include  "usbd_hid_core.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"

/*************************   任务堆栈   ******************************/

OS_STK START_TASK_STK[START_STK_SIZE];	//起始任务堆栈

OS_STK MESS_TASK_STK[MESS_STK_SIZE];    //蓝牙接收任务堆栈
OS_STK BTSEND_TASK_STK[BTSEND_STK_SIZE];//蓝牙发送任务堆栈

OS_STK USBSEND_TASK_STK[USB_STK_SIZE];	//USB发送任务
OS_STK USBRECV_TASK_STK[USB_STK_SIZE];	//USB接收任务

OS_STK RS485_TASK_STK[RS485_STK_SIZE];	//485收发任务堆栈
OS_STK RS232_TASK_STK[RS232_STK_SIZE];	//485收发任务堆栈
OS_STK ADS1_TASK_STK[ADS1_STK_SIZE];	//ADC 数据处理任务堆栈
OS_STK PWM_TASK_STK[PWM_STK_SIZE];		//PWM收发任务堆栈
OS_STK CAN_TASK_STK[CAN_STK_SIZE];		//CAN 接收  任务
OS_STK K_TASK_STK[K_STK_SIZE];			//K线 接收任务

OS_STK ADC_TASK_STK[ADC_STK_SIZE];

/*************************   任务间通信  *****************************/
OS_EVENT * RS485SendQ;				//485 发送消息队列的指针
OS_EVENT * RS232SendQ;				//485 发送消息队列的指针

OS_EVENT * testMbox;				//通讯检测 消息邮箱

OS_EVENT * KLineRecvQ;				//K线 接收消息队列的指针

OS_EVENT * lockMux595;				//595 互斥锁

OS_EVENT * WIFICmdQ;				//接收到WiFi链接信息
OS_EVENT * WIFIDatQ;				//接收到设备发来的数据信息

OS_EVENT * USBRecvQ;				//USB接收数据消息队列
OS_EVENT * USBSendQ;				//USB发送数据消息队列

OS_EVENT * MessageRecvQ;			//从平板接收消息队列的指针

OS_EVENT * BTSet;					//蓝牙配置 消息队列

OS_EVENT * CANRecvQ;				//CAN 接收消息队列的指针

OS_EVENT * KFastStartSem;			//K线快速初始化信号量
OS_EVENT * BXWSendSem;				//便携王定时发送信号量
OS_EVENT * BXWFlowSem;				//便携王数据流发送信号量

OS_EVENT * canRCV30;				// VCI 接收到0x30指令 信号量
OS_EVENT * BXWSafe;					//便携王后处理安全算法

OS_EVENT * ADCResultQ;				//存储ADC转换的结果

#define RS485SENDBUF_SIZE   5		//485 发送消息队列保存消息的最大量
#define RS232SENDBUF_SIZE   5		//232 发送消息队列保存消息的最大量
#define KSENDBUF_SIZE       5		//K线 发送消息队列保存消息的最大量
#define KRECVBUF_SIZE       5		//K线 接收消息队列保存消息的最大量
#define ADCQ_SIZE			5		//ADC 转换成功后的消息队列

#define USBSENDBUF_SIZE    17		//USB 发送消息队列保存消息的最大量
#define USBRECVBUF_SIZE    17		//USB 接收消息队列保存消息的最大量

#define BTSET_SIZE    	    5		//蓝牙设置 消息队列保存消息的最大量

#define WIFICMD_SIZE		8		//WIFI 接收命令返回信息
#define WIFIDAT_SIZE		8		//WIFI 接收数据信息

#define BTRECVBUF_SIZE      4		//蓝牙 接收消息队列保存消息的最大量
#define CANRECVBUF_SIZE     5		//CAN  接收消息队列保存消息的最大量

void *rs485SendBuf[RS485SENDBUF_SIZE];  //指向 485  接收消息的指针数组
void *rs232SendBuf[RS232SENDBUF_SIZE];  //指向 232  接收消息的指针数组
void *KLineSendBuf[KSENDBUF_SIZE];		//指向 K线  接收消息的指针数组
void *KLineRecvBuf[KRECVBUF_SIZE];		//指向 K线  接收消息的指针数组

void *USBSendBuf[USBSENDBUF_SIZE];	//指向 USB  接收消息的指针数组
void *USBRecvBuf[USBRECVBUF_SIZE];	//指向 USB  接收消息的指针数组

void *ADCQresultBuf[ADCQ_SIZE];			//指向 ADC 转换完成的指针数组

void *WifiCmdBuf[WIFICMD_SIZE];			//wifi 指令返回值的消息队列
void *WifiDatBuf[WIFIDAT_SIZE];			//wifi 接收到的数据消息列

void *btsetBuf[BTSET_SIZE];		//指向 BT   接收消息的指针数组

void *BTRecvBuf[BTRECVBUF_SIZE];		//指向 BT   接收消息的指针数组
void *CANRecvBuf[CANRECVBUF_SIZE];		//指向 CAN  接收消息的指针数组

/*************************   全局变量   ******************************/
pCIR_QUEUE send485_Q;	//指向 485 串口发送缓冲区的指针
pSTORE     rece485_S;	//指向 485 串口接收缓冲区的指针

pCIR_QUEUE sendK_Q;		//指向 K线  串口发送缓冲区的指针
pSTORE     receK_S;		//指向 K线  串口接收缓冲区的指针

pCIR_QUEUE sendBT_Q;	//指向 蓝牙 串口发送缓冲区的指针
pSTORE     receBT_S;	//指向 蓝牙 串口接收缓冲区的指针

pCIR_QUEUE send232_Q;	//指向 232 串口发送缓冲区的指针
pSTORE     rece232_S;	//指向 232 串口接收缓冲区的指针

pSTORE     BXW_CAN;		//便携王接收 CAN 泵信息

ROM_FLASH_PARAM updatDataRam ;	//程序升级相关参数结构体变量

VCI_RunVar varVciRun;			//诊断能手相关参数

BXWAllData varBXWRun;			//便携王相关参数

SYSTEM_Var varOperaRun;

VCICAN_Data canVCIData;

BXWCAN_Data canBXWToSend;

Str_Encrypt Eyt;  				//过安全算法相关结构体
void debugDelay(u32 z)
{
	u32 i,j;
	for(i = 0;i < 100000;i++)
		for(j = z;j > 0; j--);
}
/*************************   主函数   ******************************/
int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x4800);      //中断向量重映射
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      //中断优先级分组2 
	
	OSInit();		//系统初始化
	
//	debugDelay(500);//给屈工测试用，开机10秒左右的延时
	
	MemBuf_Init();	//内存块初始化
	
	send485_Q = Cir_Queue_Init(500);	//485  发送循环队列 缓冲区
	rece485_S = Store_Init(500);		//485  串口接收 数据堆 缓冲区
	
	send232_Q = Cir_Queue_Init(250);	//232  发送循环队列 缓冲区
	rece232_S = Store_Init(250);		//232  串口接收 数据堆 缓冲区
	
	sendK_Q   = Cir_Queue_Init(500);	//K线 串口发送 数据堆 缓冲区
	receK_S   = Store_Init(500);		//K线 串口接收 数据堆 缓冲区
	
	sendBT_Q  = Cir_Queue_Init(1000);	//蓝牙 串口发送 数据堆 缓冲区
	receBT_S  = Store_Init(1000);		//蓝牙 串口接收 数据堆 缓冲区
	
	BXW_CAN   = Store_Init(500);		//便携王接收 CAN 泵信息
	
	SystemBspInit();		            //外设初始化
	//创建起始任务
	OSTaskCreate(StartTask,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE - 1],START_TASK_PRIO);
	OSStart();				            //系统启动  开始任务调度
}

uint8_t abcd = 0;//

// 起始任务
void StartTask(void *pdata)
{
	uint8_t i,err;
	uint16_t data595;
	float barometric,humidity,elevation;
/*************************    创建任务间通信的消息     ******************************/	
	
	RS485SendQ	= OSQCreate(&rs485SendBuf[0],RS485SENDBUF_SIZE);	//建立 485  发送 消息队列
	RS232SendQ	= OSQCreate(&rs232SendBuf[0],RS232SENDBUF_SIZE);	//建立 232  发送 消息队列
	lockMux595  = OSMutexCreate(MUX_595_PRIO,&err);					//建立 595  互斥型信号量
	KLineRecvQ	= OSQCreate(&KLineRecvBuf[0],KRECVBUF_SIZE);		//建立 K线  接收 消息队列
	MessageRecvQ = OSQCreate(&BTRecvBuf[0],BTRECVBUF_SIZE);			//建立 平板 接收 消息队列
	CANRecvQ	= OSQCreate(&CANRecvBuf[0],CANRECVBUF_SIZE);		//建立 CAN  接收 消息队列
	WIFICmdQ	= OSQCreate(&WifiCmdBuf[0],WIFICMD_SIZE);	        //WIFI 接收命令返回的消息队列
	WIFIDatQ	= OSQCreate(&WifiDatBuf[0],WIFIDAT_SIZE);	        //WIFI 接收数据的消息队列
	ADCResultQ  = OSQCreate(&ADCQresultBuf[0],ADCQ_SIZE);	        //
	
	USBRecvQ    = OSQCreate(&USBRecvBuf[0],USBRECVBUF_SIZE);	    //USB接收数据消息队列
	USBSendQ    = OSQCreate(&USBSendBuf[0],USBSENDBUF_SIZE);		//USB发送数据消息队列
	
	BTSet		= OSQCreate(&btsetBuf[0],BTSET_SIZE);				//蓝牙设置相关 消息队列
	
	KFastStartSem = OSSemCreate(0);
	BXWSendSem = OSSemCreate(0);
	canRCV30 = OSSemCreate(0);				// VCI 接收到0x30指令 信号量
	BXWSafe  = OSSemCreate(0);				//便携王后处理安全算法接收到 ECU 返回的 Seed
	
	BXWFlowSem = OSSemCreate(0);;			//便携王数据流发送信号量
	
	testMbox = OSMboxCreate((void *)0);		//创建消息邮箱
	
/*************************************     创建各任务    ************************************************/		

	OSTaskCreate(MessageDealTask,(void *)0,(OS_STK *)&MESS_TASK_STK[MESS_STK_SIZE-1],		MESS_TASK_PRIO );
	OSTaskCreate(BTSendTask,     (void *)0,(OS_STK *)&BTSEND_TASK_STK[BTSEND_STK_SIZE-1],	BTSEND_TASK_PRIO);
	OSTaskCreate(RS485Task,      (void *)0,(OS_STK *)&RS485_TASK_STK[RS485_STK_SIZE-1],		RS485_TASK_PRIO);
	OSTaskCreate(RS232Task,      (void *)0,(OS_STK *)&RS232_TASK_STK[RS232_STK_SIZE-1],		RS232_TASK_PRIO);
	OSTaskCreate(ADS1243_Task1,  (void *)0,(OS_STK *)&ADS1_TASK_STK[ADS1_STK_SIZE-1],		ADS1_TASK_PRIO);
	OSTaskCreate(PWMTask,        (void *)0,(OS_STK *)&PWM_TASK_STK[PWM_STK_SIZE-1],			PWM_TASK_PRIO);
	OSTaskCreate(CANTask,		 (void *)0,(OS_STK *)&CAN_TASK_STK[CAN_STK_SIZE-1],			CAN_TASK_PRIO);
	OSTaskCreate(KLineTask,		 (void *)0,(OS_STK *)&K_TASK_STK[K_STK_SIZE-1],				K_TASK_PRIO);
	OSTaskCreate(ADC_Task,		 (void *)0,(OS_STK *)&ADC_TASK_STK[ADC_STK_SIZE-1],			ADC_TASK_PRIO);
	//USB相关任务初始化
	OSTaskCreate(USBSendTask,(void *)0,(OS_STK *)&USBSEND_TASK_STK[USB_STK_SIZE-1],USB_S_TASK_PRIO);
	OSTaskCreate(USBRecvTask,(void *)0,(OS_STK *)&USBRECV_TASK_STK[USB_STK_SIZE-1],USB_R_TASK_PRIO);
// 
	for(i = 0;i<24;i++)  				//上电流水灯
	{
		LS595Deal(varOperaRun.dat595);
		OSTimeDlyHMSM(0,0,0,100);
		varOperaRun.dat595 <<= 1;
		varOperaRun.dat595 |= 0x0001;
		varOperaRun.dat595 &= 0xFE7F;
		if(varOperaRun.dat595 == 0x007F)
			varOperaRun.dat595 = 0x007D;
	}
//	GPIO_ResetBits(GPIOC,GPIO_Pin_14);	//Pwr_24  上电供电
//	GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//Pwr_Sw
	varOperaRun.dat595 = 0x007F;		//灯全灭
	LS595Deal(varOperaRun.dat595);
	varOperaRun.ledFlag[0] = LEDON;		//电源灯亮
	while(1)							//进行 LED 灯光显示、四路 CAN 切换、等
	{
		data595 = varOperaRun.dat595;
		OSTimeDlyHMSM(0,0,0,50);		//50ms 执行一次
		abcd ++;
		
		if(abcd > 20)//环境传感器 气压、温度、海拔（海拔数据需要修正）
		{
			abcd = 0;
			bmp280GetData(&barometric,&humidity,&elevation);
			varBXWRun.barometric = (u16)(barometric*10);
			varBXWRun.humidity   = (u16)(humidity*100);
			varBXWRun.weizhi	 = (u16)(elevation*10);
		}
		varOperaRun.timeLed ++;
		
		for(i=0;i<6;i++)
		{
			if(varOperaRun.ledFlag[i] == LEDOFF)		//灭
			{
				varOperaRun.dat595 |= (1<<(1+i));
			}
			else if(varOperaRun.ledFlag[i] == LEDON)	//亮
			{
				varOperaRun.dat595 &= ~(1<<(1+i));
			}
			else if(varOperaRun.ledFlag[i] == LEDBLINK)	//闪烁
			{	//如果程序中忘记赋值了
				if(varOperaRun.ledBlinTime[i] == 0||varOperaRun.ledBlinTime[i]>50)
					varOperaRun.ledBlinTime[i] = 50;
				if(varOperaRun.ledTimeCnt[i] == varOperaRun.timeLed)
				{
					varOperaRun.ledTimeCnt[i] = varOperaRun.ledBlinTime[i] + varOperaRun.timeLed;
					if(varOperaRun.ledHL[i]!=0)
					{
						varOperaRun.dat595 &= ~(1<<(1+i));
						varOperaRun.ledHL[i] = 0;
					}
					else{
						varOperaRun.dat595 |= (1 << (1+i));
						varOperaRun.ledHL[i] = 1;
					}
				}	
			}
		}
		if(data595 == varOperaRun.dat595)
			continue;
		LS595Deal(varOperaRun.dat595);//将灯新的状态写入
	}
}

//	switch(count)
//	{
//		case 1  :    { HC595(0x00Fe); break;}  // CAN1默认通道（2、14）0欧姆电阻    亮灯LD2                                       HC595(0xFDFD); break;
//		case 2  :    { HC595(0x0bFD); break;}  // CAN1默认通道（2、14）60欧姆电阻   亮灯LD3                                       HC595(0xFBFB); break;
//		case 3  :    { HC595(0x0aFB); break;}  // CAN1默认通道（2、14）120欧姆电阻  亮灯LD4                                       HC595(0xF7F7); break;
//		case 4  :    { HC595(0x04F7); break;}  // CAN1切换通道（6、10）0欧姆电阻    亮灯LD5                                       HC595(0xEFEF); break;                
//		case 5  :    { HC595(0x0fEF); break;}  // CAN1切换通道（6、10）60欧姆电阻   亮灯LD6                                       HC595(0xDFDF); break;                
//		case 6  :    { HC595(0x0eDF); break;}  // CAN1切换通道（6、10）120欧姆电阻  亮灯LD7                                       HC595(0xBFBF); break;
//																																								
//		case 7  :    { HC595(0x00BF); break;}  // CAN2默认通道（3、11）0欧姆电阻    亮灯LD8                                                                       
//		case 8  :    { HC595(0xd07c); break;}  // CAN2默认通道（3、11）60欧姆电阻   亮灯LD2和LD3                                                                      
//		case 9  :    { HC595(0x90F9); break;}  // CAN2默认通道（3、11）120欧姆电阻  亮灯LD3和LD4                                                                       
//		case 10 :    { HC595(0x20f3); break;}  // CAN2切换通道（1、9）0欧姆电阻     亮灯LD4和LD5
//		case 11 :    { HC595(0xf0e7); break;}  // CAN2切换通道（1、9）60欧姆电阻    亮灯LD5和LD6
//		case 12 :    { HC595(0x70cF); break;}  // CAN2切换通道（1、9）120欧姆电阻   亮灯LD6和LD7
//		case 13 :    { HC595(0x0000); break;}
//		default : break;
//	}











