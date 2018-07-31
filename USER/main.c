
#include "includes.h"
#include "apptask.h"
#include  "usbd_hid_core.h"
#include  "usbd_usr.h"
#include  "usbd_desc.h"

/*************************   �����ջ   ******************************/

OS_STK START_TASK_STK[START_STK_SIZE];	//��ʼ�����ջ

OS_STK MESS_TASK_STK[MESS_STK_SIZE];    //�������������ջ
OS_STK BTSEND_TASK_STK[BTSEND_STK_SIZE];//�������������ջ

OS_STK USBSEND_TASK_STK[USB_STK_SIZE];	//USB��������
OS_STK USBRECV_TASK_STK[USB_STK_SIZE];	//USB��������

OS_STK RS485_TASK_STK[RS485_STK_SIZE];	//485�շ������ջ
OS_STK RS232_TASK_STK[RS232_STK_SIZE];	//485�շ������ջ
OS_STK ADS1_TASK_STK[ADS1_STK_SIZE];	//ADC ���ݴ��������ջ
OS_STK PWM_TASK_STK[PWM_STK_SIZE];		//PWM�շ������ջ
OS_STK CAN_TASK_STK[CAN_STK_SIZE];		//CAN ����  ����
OS_STK K_TASK_STK[K_STK_SIZE];			//K�� ��������

OS_STK ADC_TASK_STK[ADC_STK_SIZE];

/*************************   �����ͨ��  *****************************/
OS_EVENT * RS485SendQ;				//485 ������Ϣ���е�ָ��
OS_EVENT * RS232SendQ;				//485 ������Ϣ���е�ָ��

OS_EVENT * testMbox;				//ͨѶ��� ��Ϣ����

OS_EVENT * KLineRecvQ;				//K�� ������Ϣ���е�ָ��

OS_EVENT * lockMux595;				//595 ������

OS_EVENT * WIFICmdQ;				//���յ�WiFi������Ϣ
OS_EVENT * WIFIDatQ;				//���յ��豸������������Ϣ

OS_EVENT * USBRecvQ;				//USB����������Ϣ����
OS_EVENT * USBSendQ;				//USB����������Ϣ����

OS_EVENT * MessageRecvQ;			//��ƽ�������Ϣ���е�ָ��

OS_EVENT * BTSet;					//�������� ��Ϣ����

OS_EVENT * CANRecvQ;				//CAN ������Ϣ���е�ָ��

OS_EVENT * KFastStartSem;			//K�߿��ٳ�ʼ���ź���
OS_EVENT * BXWSendSem;				//��Я����ʱ�����ź���
OS_EVENT * BXWFlowSem;				//��Я�������������ź���

OS_EVENT * canRCV30;				// VCI ���յ�0x30ָ�� �ź���
OS_EVENT * BXWSafe;					//��Я������ȫ�㷨

OS_EVENT * ADCResultQ;				//�洢ADCת���Ľ��

#define RS485SENDBUF_SIZE   5		//485 ������Ϣ���б�����Ϣ�������
#define RS232SENDBUF_SIZE   5		//232 ������Ϣ���б�����Ϣ�������
#define KSENDBUF_SIZE       5		//K�� ������Ϣ���б�����Ϣ�������
#define KRECVBUF_SIZE       5		//K�� ������Ϣ���б�����Ϣ�������
#define ADCQ_SIZE			5		//ADC ת���ɹ������Ϣ����

#define USBSENDBUF_SIZE    17		//USB ������Ϣ���б�����Ϣ�������
#define USBRECVBUF_SIZE    17		//USB ������Ϣ���б�����Ϣ�������

#define BTSET_SIZE    	    5		//�������� ��Ϣ���б�����Ϣ�������

#define WIFICMD_SIZE		8		//WIFI �����������Ϣ
#define WIFIDAT_SIZE		8		//WIFI ����������Ϣ

#define BTRECVBUF_SIZE      4		//���� ������Ϣ���б�����Ϣ�������
#define CANRECVBUF_SIZE     5		//CAN  ������Ϣ���б�����Ϣ�������

void *rs485SendBuf[RS485SENDBUF_SIZE];  //ָ�� 485  ������Ϣ��ָ������
void *rs232SendBuf[RS232SENDBUF_SIZE];  //ָ�� 232  ������Ϣ��ָ������
void *KLineSendBuf[KSENDBUF_SIZE];		//ָ�� K��  ������Ϣ��ָ������
void *KLineRecvBuf[KRECVBUF_SIZE];		//ָ�� K��  ������Ϣ��ָ������

void *USBSendBuf[USBSENDBUF_SIZE];	//ָ�� USB  ������Ϣ��ָ������
void *USBRecvBuf[USBRECVBUF_SIZE];	//ָ�� USB  ������Ϣ��ָ������

void *ADCQresultBuf[ADCQ_SIZE];			//ָ�� ADC ת����ɵ�ָ������

void *WifiCmdBuf[WIFICMD_SIZE];			//wifi ָ���ֵ����Ϣ����
void *WifiDatBuf[WIFIDAT_SIZE];			//wifi ���յ���������Ϣ��

void *btsetBuf[BTSET_SIZE];		//ָ�� BT   ������Ϣ��ָ������

void *BTRecvBuf[BTRECVBUF_SIZE];		//ָ�� BT   ������Ϣ��ָ������
void *CANRecvBuf[CANRECVBUF_SIZE];		//ָ�� CAN  ������Ϣ��ָ������

/*************************   ȫ�ֱ���   ******************************/
pCIR_QUEUE send485_Q;	//ָ�� 485 ���ڷ��ͻ�������ָ��
pSTORE     rece485_S;	//ָ�� 485 ���ڽ��ջ�������ָ��

pCIR_QUEUE sendK_Q;		//ָ�� K��  ���ڷ��ͻ�������ָ��
pSTORE     receK_S;		//ָ�� K��  ���ڽ��ջ�������ָ��

pCIR_QUEUE sendBT_Q;	//ָ�� ���� ���ڷ��ͻ�������ָ��
pSTORE     receBT_S;	//ָ�� ���� ���ڽ��ջ�������ָ��

pCIR_QUEUE send232_Q;	//ָ�� 232 ���ڷ��ͻ�������ָ��
pSTORE     rece232_S;	//ָ�� 232 ���ڽ��ջ�������ָ��

pSTORE     BXW_CAN;		//��Я������ CAN ����Ϣ

ROM_FLASH_PARAM updatDataRam ;	//����������ز����ṹ�����

VCI_RunVar varVciRun;			//���������ز���

BXWAllData varBXWRun;			//��Я����ز���

SYSTEM_Var varOperaRun;

VCICAN_Data canVCIData;

BXWCAN_Data canBXWToSend;

Str_Encrypt Eyt;  				//����ȫ�㷨��ؽṹ��
void debugDelay(u32 z)
{
	u32 i,j;
	for(i = 0;i < 100000;i++)
		for(j = z;j > 0; j--);
}
/*************************   ������   ******************************/
int main(void)
{
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x4800);      //�ж�������ӳ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);      //�ж����ȼ�����2 
	
	OSInit();		//ϵͳ��ʼ��
	
//	debugDelay(500);//�����������ã�����10�����ҵ���ʱ
	
	MemBuf_Init();	//�ڴ���ʼ��
	
	send485_Q = Cir_Queue_Init(500);	//485  ����ѭ������ ������
	rece485_S = Store_Init(500);		//485  ���ڽ��� ���ݶ� ������
	
	send232_Q = Cir_Queue_Init(250);	//232  ����ѭ������ ������
	rece232_S = Store_Init(250);		//232  ���ڽ��� ���ݶ� ������
	
	sendK_Q   = Cir_Queue_Init(500);	//K�� ���ڷ��� ���ݶ� ������
	receK_S   = Store_Init(500);		//K�� ���ڽ��� ���ݶ� ������
	
	sendBT_Q  = Cir_Queue_Init(1000);	//���� ���ڷ��� ���ݶ� ������
	receBT_S  = Store_Init(1000);		//���� ���ڽ��� ���ݶ� ������
	
	BXW_CAN   = Store_Init(500);		//��Я������ CAN ����Ϣ
	
	SystemBspInit();		            //�����ʼ��
	//������ʼ����
	OSTaskCreate(StartTask,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE - 1],START_TASK_PRIO);
	OSStart();				            //ϵͳ����  ��ʼ�������
}

uint8_t abcd = 0;//

// ��ʼ����
void StartTask(void *pdata)
{
	uint8_t i,err;
	uint16_t data595;
	float barometric,humidity,elevation;
/*************************    ���������ͨ�ŵ���Ϣ     ******************************/	
	
	RS485SendQ	= OSQCreate(&rs485SendBuf[0],RS485SENDBUF_SIZE);	//���� 485  ���� ��Ϣ����
	RS232SendQ	= OSQCreate(&rs232SendBuf[0],RS232SENDBUF_SIZE);	//���� 232  ���� ��Ϣ����
	lockMux595  = OSMutexCreate(MUX_595_PRIO,&err);					//���� 595  �������ź���
	KLineRecvQ	= OSQCreate(&KLineRecvBuf[0],KRECVBUF_SIZE);		//���� K��  ���� ��Ϣ����
	MessageRecvQ = OSQCreate(&BTRecvBuf[0],BTRECVBUF_SIZE);			//���� ƽ�� ���� ��Ϣ����
	CANRecvQ	= OSQCreate(&CANRecvBuf[0],CANRECVBUF_SIZE);		//���� CAN  ���� ��Ϣ����
	WIFICmdQ	= OSQCreate(&WifiCmdBuf[0],WIFICMD_SIZE);	        //WIFI ��������ص���Ϣ����
	WIFIDatQ	= OSQCreate(&WifiDatBuf[0],WIFIDAT_SIZE);	        //WIFI �������ݵ���Ϣ����
	ADCResultQ  = OSQCreate(&ADCQresultBuf[0],ADCQ_SIZE);	        //
	
	USBRecvQ    = OSQCreate(&USBRecvBuf[0],USBRECVBUF_SIZE);	    //USB����������Ϣ����
	USBSendQ    = OSQCreate(&USBSendBuf[0],USBSENDBUF_SIZE);		//USB����������Ϣ����
	
	BTSet		= OSQCreate(&btsetBuf[0],BTSET_SIZE);				//����������� ��Ϣ����
	
	KFastStartSem = OSSemCreate(0);
	BXWSendSem = OSSemCreate(0);
	canRCV30 = OSSemCreate(0);				// VCI ���յ�0x30ָ�� �ź���
	BXWSafe  = OSSemCreate(0);				//��Я������ȫ�㷨���յ� ECU ���ص� Seed
	
	BXWFlowSem = OSSemCreate(0);;			//��Я�������������ź���
	
	testMbox = OSMboxCreate((void *)0);		//������Ϣ����
	
/*************************************     ����������    ************************************************/		

	OSTaskCreate(MessageDealTask,(void *)0,(OS_STK *)&MESS_TASK_STK[MESS_STK_SIZE-1],		MESS_TASK_PRIO );
	OSTaskCreate(BTSendTask,     (void *)0,(OS_STK *)&BTSEND_TASK_STK[BTSEND_STK_SIZE-1],	BTSEND_TASK_PRIO);
	OSTaskCreate(RS485Task,      (void *)0,(OS_STK *)&RS485_TASK_STK[RS485_STK_SIZE-1],		RS485_TASK_PRIO);
	OSTaskCreate(RS232Task,      (void *)0,(OS_STK *)&RS232_TASK_STK[RS232_STK_SIZE-1],		RS232_TASK_PRIO);
	OSTaskCreate(ADS1243_Task1,  (void *)0,(OS_STK *)&ADS1_TASK_STK[ADS1_STK_SIZE-1],		ADS1_TASK_PRIO);
	OSTaskCreate(PWMTask,        (void *)0,(OS_STK *)&PWM_TASK_STK[PWM_STK_SIZE-1],			PWM_TASK_PRIO);
	OSTaskCreate(CANTask,		 (void *)0,(OS_STK *)&CAN_TASK_STK[CAN_STK_SIZE-1],			CAN_TASK_PRIO);
	OSTaskCreate(KLineTask,		 (void *)0,(OS_STK *)&K_TASK_STK[K_STK_SIZE-1],				K_TASK_PRIO);
	OSTaskCreate(ADC_Task,		 (void *)0,(OS_STK *)&ADC_TASK_STK[ADC_STK_SIZE-1],			ADC_TASK_PRIO);
	//USB��������ʼ��
	OSTaskCreate(USBSendTask,(void *)0,(OS_STK *)&USBSEND_TASK_STK[USB_STK_SIZE-1],USB_S_TASK_PRIO);
	OSTaskCreate(USBRecvTask,(void *)0,(OS_STK *)&USBRECV_TASK_STK[USB_STK_SIZE-1],USB_R_TASK_PRIO);
// 
	for(i = 0;i<24;i++)  				//�ϵ���ˮ��
	{
		LS595Deal(varOperaRun.dat595);
		OSTimeDlyHMSM(0,0,0,100);
		varOperaRun.dat595 <<= 1;
		varOperaRun.dat595 |= 0x0001;
		varOperaRun.dat595 &= 0xFE7F;
		if(varOperaRun.dat595 == 0x007F)
			varOperaRun.dat595 = 0x007D;
	}
//	GPIO_ResetBits(GPIOC,GPIO_Pin_14);	//Pwr_24  �ϵ繩��
//	GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//Pwr_Sw
	varOperaRun.dat595 = 0x007F;		//��ȫ��
	LS595Deal(varOperaRun.dat595);
	varOperaRun.ledFlag[0] = LEDON;		//��Դ����
	while(1)							//���� LED �ƹ���ʾ����· CAN �л�����
	{
		data595 = varOperaRun.dat595;
		OSTimeDlyHMSM(0,0,0,50);		//50ms ִ��һ��
		abcd ++;
		
		if(abcd > 20)//���������� ��ѹ���¶ȡ����Σ�����������Ҫ������
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
			if(varOperaRun.ledFlag[i] == LEDOFF)		//��
			{
				varOperaRun.dat595 |= (1<<(1+i));
			}
			else if(varOperaRun.ledFlag[i] == LEDON)	//��
			{
				varOperaRun.dat595 &= ~(1<<(1+i));
			}
			else if(varOperaRun.ledFlag[i] == LEDBLINK)	//��˸
			{	//������������Ǹ�ֵ��
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
		LS595Deal(varOperaRun.dat595);//�����µ�״̬д��
	}
}

//	switch(count)
//	{
//		case 1  :    { HC595(0x00Fe); break;}  // CAN1Ĭ��ͨ����2��14��0ŷķ����    ����LD2                                       HC595(0xFDFD); break;
//		case 2  :    { HC595(0x0bFD); break;}  // CAN1Ĭ��ͨ����2��14��60ŷķ����   ����LD3                                       HC595(0xFBFB); break;
//		case 3  :    { HC595(0x0aFB); break;}  // CAN1Ĭ��ͨ����2��14��120ŷķ����  ����LD4                                       HC595(0xF7F7); break;
//		case 4  :    { HC595(0x04F7); break;}  // CAN1�л�ͨ����6��10��0ŷķ����    ����LD5                                       HC595(0xEFEF); break;                
//		case 5  :    { HC595(0x0fEF); break;}  // CAN1�л�ͨ����6��10��60ŷķ����   ����LD6                                       HC595(0xDFDF); break;                
//		case 6  :    { HC595(0x0eDF); break;}  // CAN1�л�ͨ����6��10��120ŷķ����  ����LD7                                       HC595(0xBFBF); break;
//																																								
//		case 7  :    { HC595(0x00BF); break;}  // CAN2Ĭ��ͨ����3��11��0ŷķ����    ����LD8                                                                       
//		case 8  :    { HC595(0xd07c); break;}  // CAN2Ĭ��ͨ����3��11��60ŷķ����   ����LD2��LD3                                                                      
//		case 9  :    { HC595(0x90F9); break;}  // CAN2Ĭ��ͨ����3��11��120ŷķ����  ����LD3��LD4                                                                       
//		case 10 :    { HC595(0x20f3); break;}  // CAN2�л�ͨ����1��9��0ŷķ����     ����LD4��LD5
//		case 11 :    { HC595(0xf0e7); break;}  // CAN2�л�ͨ����1��9��60ŷķ����    ����LD5��LD6
//		case 12 :    { HC595(0x70cF); break;}  // CAN2�л�ͨ����1��9��120ŷķ����   ����LD6��LD7
//		case 13 :    { HC595(0x0000); break;}
//		default : break;
//	}











