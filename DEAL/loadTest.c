

//此文件用来处理 负载测试

#include "includes.h"

extern OS_EVENT * BTSet;					//蓝牙配置消息队列

const char BTInf[3]	="$$$";					//进入命令模式
const char btinf[2] 	= {'X',0x0D};			//获取指令配置信息
const char btinf1[2]	= {'h',0x0D};			//查询蓝牙命令列表
const char btNameCh[18] = "SN,CRSF-QNW-%s\r";	//修改蓝牙名称
extern void BXW_PWMDataDeal(PWM_Data * ptr,uint8_t PWMId);
extern uint8_t BTSendDatas(const uint8_t* s,uint16_t length);
extern uint8_t RS485SendDatas(const uint8_t* s,uint16_t length);
extern void DCMCh(uint8_t mode,uint8_t cha,uint8_t resCh,uint8_t loadCmd);
extern void BXW_StartUp(STARTUP *ptr);//电源选择
extern void BXW_DCMDataDeal(DCM_Data * ptr,uint8_t PWMId);
void Vol_Res_Test(uint8_t volCh);
void LoadCh(uint8_t mode,uint8_t cha,uint8_t resCh,uint8_t loadCmd,uint8_t outPut);
uint8_t loadCmd;
char btMAC[20];//暂时保存MAC地址，可以优化去掉
uint8_t testMin = 0,testSec = 5;
void LoadTest(void )
{
	char * ptrSet,*ptrMac,*ptrToSend;
	uint8_t err,mode,cha,resCh;
	
	uint8_t i = 0;
	char sendCmd[22];
	uint8_t sendlen = 0;
	uint8_t checkSum = 0;

/*************   蓝牙配置阶段    *************/	
	BTSendDatas((const u8*)BTInf,3);//进入命令模式
	ptrSet = OSQPend(BTSet,1000,&err);
	if(strstr(ptrSet,"CMD") == NULL)//
	{
		//蓝牙设置失败，无法进行负载测试
		ptrToSend = Mem_malloc(20);
		ptrToSend[0] = 0x01;
		ptrToSend[1] = 0x00;
		ptrToSend[2] = 0x01;		//蓝牙设置模式 进入失败
		DateToSend((uint8_t *)ptrToSend,1+2,0x02,0x8F);
		Mem_free((void **)&ptrToSend);
		varOperaRun.sysMode = IDLE_MODE;//空模式
		return;
	}
	BTSendDatas((const u8*)btinf,2);//蓝牙信息
	for(i = 0; i<5;i++)
	{
		ptrSet = OSQPend(BTSet,200,&err);
		if(strstr(ptrSet,"BTA=") != NULL)
		{
			ptrMac =strstr(ptrSet,"BTA="); 
			memcpy(btMAC,&ptrMac[4],strlen(ptrMac)-4);//复制蓝牙地址
			//通知上位机接收到 蓝牙 MAC 地址
			ptrToSend = Mem_malloc(20);
			ptrToSend[0] = 0x0D;
			ptrToSend[1] = 0x00;
			ptrToSend[2] = 0x02;		//蓝牙MAC 地址读取成功
			memcpy(&ptrToSend[3],btMAC,12);
			DateToSend((uint8_t *)ptrToSend,0x0D + 2,0x02,0x8F);
			Mem_free((void **)&ptrToSend);
			
			Mem_free((void**)&ptrSet);
			break;
		}
		Mem_free((void**)&ptrSet);
	}
	while(err == OS_ERR_NONE)
	{
		ptrSet = OSQPend(BTSet,200,&err);
		Mem_free((void**)&ptrSet);
	}
	sendlen = sprintf(sendCmd,(const char*)btNameCh,&btMAC[8]);// TCP 连接
	BTSendDatas((const u8*)sendCmd,17);//修改蓝牙名称
	ptrSet = OSQPend(BTSet,2000,&err);
	if(strstr(ptrSet,"AOK") != NULL)
	{
		//todo:通知上位机 蓝牙名称修改成功
		ptrToSend = Mem_malloc(24);
		ptrToSend[0] = 14;
		ptrToSend[1] = 0x00;
		ptrToSend[2] = 0x03;		//蓝牙名称修改成功
		memcpy(&ptrToSend[3],&sendCmd[3],13);
		DateToSend((uint8_t *)ptrToSend,14+2,0x02,0x8F);
		Mem_free((void **)&ptrToSend);
	}
	else
	{
		ptrToSend = Mem_malloc(20);
		ptrToSend[0] = 0x01;
		ptrToSend[1] = 0x00;
		ptrToSend[2] = 0x04;		//蓝牙名称修改失败
		DateToSend((uint8_t *)ptrToSend,1+2,0x02,0x8F);
		Mem_free((void **)&ptrToSend);
		varOperaRun.sysMode = IDLE_MODE;//空模式
		return;
	}
	Mem_free((void**)&ptrSet);
	
/*********************     负载测试阶段     **********************/
	ptrToSend = Mem_malloc(20);

	ptrToSend[1] = 1;// 0- 关闭 1- 24V 2 - 12V
	BXW_StartUp((STARTUP *)ptrToSend);
	
/************         8路负载   需要修改占空比       ***********/  
//DB03  负载1
	mode = 0x03;cha = 0x01;resCh = 0x01;loadCmd = 0x11;		//N 负载模式，DB03,负载1
	LoadCh(mode,cha,resCh,loadCmd,0);                             
//DB04  负载1                                                   
	mode = 0x03;cha = 0x02;resCh = 0x01;loadCmd = 0x12;		//N 负载模式，DB04,负载1 
	LoadCh(mode,cha,resCh,loadCmd,0);     
//DB20  负载1                                                   
	mode = 0x03;cha = 0x03;resCh = 0x01;loadCmd = 0x13;		//N 负载模式，DB20,负载1 
	LoadCh(mode,cha,resCh,loadCmd,0);            
//DB21  负载1                                                   
	mode = 0x03;cha = 0x04;resCh = 0x01;loadCmd = 0x14;		//N 负载模式，DB21,负载1 
	LoadCh(mode,cha,resCh,loadCmd,0);              
//DB22  负载1                                                   
	mode = 0x03;cha = 0x05;resCh = 0x01;loadCmd = 0x15;		//N 负载模式，DB22,负载1 
	LoadCh(mode,cha,resCh,loadCmd,0);                
//DB23  负载1                                                   
	mode = 0x03;cha = 0x06;resCh = 0x01;loadCmd = 0x16;		//N 负载模式，DB23,负载1 
	LoadCh(mode,cha,resCh,loadCmd,0);   
	
	for(i = 0;i < 3;i++)
	{
		//DB24  负载1
		mode = 0x02;cha = 0x01;resCh = 0x01;loadCmd = 0x17;		//P 负载模式，DB24,负载1 
		LoadCh(mode,cha,resCh,loadCmd,i);
	}            
	//DB25  负载1
	for(i = 0;i < 3;i++)
	{                                                 
		mode = 0x02;cha = 0x02;resCh = 0x01;loadCmd = 0x18;		//P 负载模式，DB25,负载1 
		LoadCh(mode,cha,resCh,loadCmd,i);    
	}
//	//DB25  负载1
//	mode = 0x02;cha = 0x02;resCh = 0x01;loadCmd = 0x19;		//P 负载模式，DB25,负载1 
//	LoadCh(mode,cha,resCh,loadCmd,0);    
	
/************          3路直流电机测试           ***********/
//DCM1  电路电流
	mode = 0x01;cha = 0x01;resCh = 0x01;loadCmd = 0x1A;
	DCMCh(mode,cha,resCh,loadCmd);
//DCM2
	mode = 0x01;cha = 0x02;resCh = 0x01;loadCmd = 0x1B;
	DCMCh(mode,cha,resCh,loadCmd);
//DCM3
	mode = 0x01;cha = 0x03;resCh = 0x01;loadCmd = 0x1C;
	DCMCh(mode,cha,resCh,loadCmd);
	
	for(i = 0;i<6;i++)
	{
		loadCmd = 0x21+i;
		Vol_Res_Test(loadCmd);
	}
	for(i = 0;i<4;i++)
	{
		loadCmd = 0x31+i;
		Vol_Res_Test(loadCmd);
	}
	
	ptrToSend[1] = 0;// 0- 关闭 1- 24V 2 - 12V
	BXW_StartUp((STARTUP *)ptrToSend);
	
	OSTimeDlyHMSM(0,0,1,0);for(i = 0;i< 3;i++)
	{
		ptrToSend[0] = 0x01;ptrToSend[1] = 0x00;	//结束指令
		ptrToSend[2] = 0xFF;
		DateToSend((uint8_t *)ptrToSend,3,0x02,0x8F);
		OSTimeDlyHMSM(0,0,0,5);
	}	
	
	Mem_free((void **)&ptrToSend);
	varOperaRun.sysMode = IDLE_MODE;//空模式
}
//电压电阻测量
void Vol_Res_Test(uint8_t volCh)
{
	uint8_t mode,cha,resCh,checkSum,i;
	uint8_t *ptrToSend;
	ptrToSend = Mem_malloc(15);
	switch(volCh)
	{
/**************      六路电压测量      ****************/
		case 0x21:case 0x22:case 0x23:
		case 0x24:case 0x25:case 0x26:
			mode = 0x05;cha = loadCmd - 0x20;resCh = 0;
			ptrToSend[0] = 0x7E;ptrToSend[1] = 0x04;
			ptrToSend[2] = mode;ptrToSend[3] = cha;	ptrToSend[4] = resCh;
			for(i = 0;i < 4;i ++)
				checkSum += ptrToSend[i + 1];
			ptrToSend[5] = checkSum;ptrToSend[6] = 0x7E;
			RS485SendDatas((const uint8_t *)ptrToSend,7);
			OSTimeDlyHMSM(0,0,5,0);
			break;
/**************      四路电阻测量      ****************/
		case 0x31:case 0x32:case 0x33:case 0x34:
			mode = 0x04;cha = loadCmd - 0x30;resCh = 0;
			ptrToSend[0] = 0x7E;ptrToSend[1] = 0x04;
			ptrToSend[2] = mode;ptrToSend[3] = cha;	ptrToSend[4] = resCh;
			for(i = 0;i < 4;i ++)
				checkSum += ptrToSend[i + 1];
			ptrToSend[5] = checkSum;ptrToSend[6] = 0x7E;
			RS485SendDatas((const uint8_t *)ptrToSend,7);
			OSTimeDlyHMSM(0,0,5,0);
			break;
			
		default:
			break;
	}
	Mem_free((void **)&ptrToSend);
}
//通道、负载切换
void LoadCh(uint8_t mode,uint8_t cha,uint8_t resCh,uint8_t loadCmd,uint8_t outPut)
{
	uint16_t freBig,dutyBig,dutySmall,freSmall;
	char *ptrToSend,*ptrSet;
	uint8_t checkSum = 0,i;
	
	ptrToSend = Mem_malloc(15);
	ptrSet = Mem_malloc(15);
	
	ptrToSend[0] = 0x7E;ptrToSend[1] = 0x04;
	ptrToSend[2] = mode;ptrToSend[3] = cha;	ptrToSend[4] = resCh;
	for(i = 0;i < 4;i ++)
		checkSum += ptrToSend[i + 1];
	ptrToSend[5] = checkSum;ptrToSend[6] = 0x7E;
	
	RS485SendDatas((const uint8_t *)ptrToSend,7);	//告诉下位机	切换通道
	OSTimeDlyHMSM(0,0,0,100);//延时 1 秒启动通道
	ptrSet[0] = loadCmd - 0x0F;
	ptrSet[1] = 1;ptrSet[2] = 1;
	switch(outPut)
	{	
		case 0x00: dutyBig=10 ;freBig=1000;dutySmall=1000;break;
		case 0x01: dutyBig=200;freBig=1000;dutySmall=1000;break;
		case 0x02: dutyBig=400;freBig=1000;dutySmall=1000;break;
		case 0x03: dutyBig=600;freBig=1000;dutySmall=1000;break;
		default:break;
	}
	
	memcpy(&ptrSet[3],&dutyBig,2);
	memcpy(&ptrSet[5],&freBig,2);
	memcpy(&ptrSet[7],&dutySmall,2);
	
	BXW_PWMDataDeal((PWM_Data *)ptrSet,loadCmd - 0x11);	//启动通道       
	OSTimeDlyHMSM(0,testMin,testSec,0);	//测试时间   
	ptrSet[1] = 0;						//停止指令
	BXW_PWMDataDeal((PWM_Data *)ptrSet,loadCmd - 0x11);	//关闭通道
	
	Mem_free((void **)&ptrSet);
	Mem_free((void **)&ptrToSend);
	OSTimeDlyHMSM(0,0,0,30);
}
void DCMCh(uint8_t mode,uint8_t cha,uint8_t resCh,uint8_t loadCmd)
{
	uint16_t fre,duty;
	char *ptrToSend,*ptrSet;
	uint8_t checkSum = 0,i;
	
	ptrToSend = Mem_malloc(15);
	ptrSet = Mem_malloc(15);
	
	ptrToSend[0] = 0x7E;ptrToSend[1] = 0x04;
	ptrToSend[2] = mode;ptrToSend[3] = cha;	ptrToSend[4] = resCh;
	for(i = 0;i < 4;i ++)
		checkSum += ptrToSend[i + 1];
	ptrToSend[5] = checkSum;ptrToSend[6] = 0x7E;
	
	RS485SendDatas((const uint8_t *)ptrToSend,7);//告诉下位机 切换通道
	OSTimeDlyHMSM(0,0,0,100);//延时 启动通道
	ptrSet[0] = 0;			//cmdID
	ptrSet[1] = 1;ptrSet[2] = 1;
	duty = 990;fre = 1000;
	memcpy(&ptrSet[3],&duty,2);
	memcpy(&ptrSet[5],&fre,2);
	
	BXW_DCMDataDeal((DCM_Data *)ptrSet,loadCmd - 0x1A);	//启动通道   正转    
	OSTimeDlyHMSM(0,testMin,testSec/2,500);	//测试时间
	ptrSet[1] = 1;ptrSet[2] = 0;
	BXW_DCMDataDeal((DCM_Data *)ptrSet,loadCmd - 0x1A);	//启动通道   反转   
	OSTimeDlyHMSM(0,testMin,testSec/2,500);	//测试时间
	
	ptrSet[1] = 0;							//停止指令
	BXW_DCMDataDeal((DCM_Data *)ptrSet,loadCmd - 0x1A);	//关闭通道
	
	Mem_free((void **)&ptrSet);
	Mem_free((void **)&ptrToSend);
	OSTimeDlyHMSM(0,0,0,100);
}










