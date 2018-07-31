

//���ļ��������� ���ز���

#include "includes.h"

extern OS_EVENT * BTSet;					//����������Ϣ����

const char BTInf[3]	="$$$";					//��������ģʽ
const char btinf[2] 	= {'X',0x0D};			//��ȡָ��������Ϣ
const char btinf1[2]	= {'h',0x0D};			//��ѯ���������б�
const char btNameCh[18] = "SN,CRSF-QNW-%s\r";	//�޸���������
extern void BXW_PWMDataDeal(PWM_Data * ptr,uint8_t PWMId);
extern uint8_t BTSendDatas(const uint8_t* s,uint16_t length);
extern uint8_t RS485SendDatas(const uint8_t* s,uint16_t length);
extern void DCMCh(uint8_t mode,uint8_t cha,uint8_t resCh,uint8_t loadCmd);
extern void BXW_StartUp(STARTUP *ptr);//��Դѡ��
extern void BXW_DCMDataDeal(DCM_Data * ptr,uint8_t PWMId);
void Vol_Res_Test(uint8_t volCh);
void LoadCh(uint8_t mode,uint8_t cha,uint8_t resCh,uint8_t loadCmd,uint8_t outPut);
uint8_t loadCmd;
char btMAC[20];//��ʱ����MAC��ַ�������Ż�ȥ��
uint8_t testMin = 0,testSec = 5;
void LoadTest(void )
{
	char * ptrSet,*ptrMac,*ptrToSend;
	uint8_t err,mode,cha,resCh;
	
	uint8_t i = 0;
	char sendCmd[22];
	uint8_t sendlen = 0;
	uint8_t checkSum = 0;

/*************   �������ý׶�    *************/	
	BTSendDatas((const u8*)BTInf,3);//��������ģʽ
	ptrSet = OSQPend(BTSet,1000,&err);
	if(strstr(ptrSet,"CMD") == NULL)//
	{
		//��������ʧ�ܣ��޷����и��ز���
		ptrToSend = Mem_malloc(20);
		ptrToSend[0] = 0x01;
		ptrToSend[1] = 0x00;
		ptrToSend[2] = 0x01;		//��������ģʽ ����ʧ��
		DateToSend((uint8_t *)ptrToSend,1+2,0x02,0x8F);
		Mem_free((void **)&ptrToSend);
		varOperaRun.sysMode = IDLE_MODE;//��ģʽ
		return;
	}
	BTSendDatas((const u8*)btinf,2);//������Ϣ
	for(i = 0; i<5;i++)
	{
		ptrSet = OSQPend(BTSet,200,&err);
		if(strstr(ptrSet,"BTA=") != NULL)
		{
			ptrMac =strstr(ptrSet,"BTA="); 
			memcpy(btMAC,&ptrMac[4],strlen(ptrMac)-4);//����������ַ
			//֪ͨ��λ�����յ� ���� MAC ��ַ
			ptrToSend = Mem_malloc(20);
			ptrToSend[0] = 0x0D;
			ptrToSend[1] = 0x00;
			ptrToSend[2] = 0x02;		//����MAC ��ַ��ȡ�ɹ�
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
	sendlen = sprintf(sendCmd,(const char*)btNameCh,&btMAC[8]);// TCP ����
	BTSendDatas((const u8*)sendCmd,17);//�޸���������
	ptrSet = OSQPend(BTSet,2000,&err);
	if(strstr(ptrSet,"AOK") != NULL)
	{
		//todo:֪ͨ��λ�� ���������޸ĳɹ�
		ptrToSend = Mem_malloc(24);
		ptrToSend[0] = 14;
		ptrToSend[1] = 0x00;
		ptrToSend[2] = 0x03;		//���������޸ĳɹ�
		memcpy(&ptrToSend[3],&sendCmd[3],13);
		DateToSend((uint8_t *)ptrToSend,14+2,0x02,0x8F);
		Mem_free((void **)&ptrToSend);
	}
	else
	{
		ptrToSend = Mem_malloc(20);
		ptrToSend[0] = 0x01;
		ptrToSend[1] = 0x00;
		ptrToSend[2] = 0x04;		//���������޸�ʧ��
		DateToSend((uint8_t *)ptrToSend,1+2,0x02,0x8F);
		Mem_free((void **)&ptrToSend);
		varOperaRun.sysMode = IDLE_MODE;//��ģʽ
		return;
	}
	Mem_free((void**)&ptrSet);
	
/*********************     ���ز��Խ׶�     **********************/
	ptrToSend = Mem_malloc(20);

	ptrToSend[1] = 1;// 0- �ر� 1- 24V 2 - 12V
	BXW_StartUp((STARTUP *)ptrToSend);
	
/************         8·����   ��Ҫ�޸�ռ�ձ�       ***********/  
//DB03  ����1
	mode = 0x03;cha = 0x01;resCh = 0x01;loadCmd = 0x11;		//N ����ģʽ��DB03,����1
	LoadCh(mode,cha,resCh,loadCmd,0);                             
//DB04  ����1                                                   
	mode = 0x03;cha = 0x02;resCh = 0x01;loadCmd = 0x12;		//N ����ģʽ��DB04,����1 
	LoadCh(mode,cha,resCh,loadCmd,0);     
//DB20  ����1                                                   
	mode = 0x03;cha = 0x03;resCh = 0x01;loadCmd = 0x13;		//N ����ģʽ��DB20,����1 
	LoadCh(mode,cha,resCh,loadCmd,0);            
//DB21  ����1                                                   
	mode = 0x03;cha = 0x04;resCh = 0x01;loadCmd = 0x14;		//N ����ģʽ��DB21,����1 
	LoadCh(mode,cha,resCh,loadCmd,0);              
//DB22  ����1                                                   
	mode = 0x03;cha = 0x05;resCh = 0x01;loadCmd = 0x15;		//N ����ģʽ��DB22,����1 
	LoadCh(mode,cha,resCh,loadCmd,0);                
//DB23  ����1                                                   
	mode = 0x03;cha = 0x06;resCh = 0x01;loadCmd = 0x16;		//N ����ģʽ��DB23,����1 
	LoadCh(mode,cha,resCh,loadCmd,0);   
	
	for(i = 0;i < 3;i++)
	{
		//DB24  ����1
		mode = 0x02;cha = 0x01;resCh = 0x01;loadCmd = 0x17;		//P ����ģʽ��DB24,����1 
		LoadCh(mode,cha,resCh,loadCmd,i);
	}            
	//DB25  ����1
	for(i = 0;i < 3;i++)
	{                                                 
		mode = 0x02;cha = 0x02;resCh = 0x01;loadCmd = 0x18;		//P ����ģʽ��DB25,����1 
		LoadCh(mode,cha,resCh,loadCmd,i);    
	}
//	//DB25  ����1
//	mode = 0x02;cha = 0x02;resCh = 0x01;loadCmd = 0x19;		//P ����ģʽ��DB25,����1 
//	LoadCh(mode,cha,resCh,loadCmd,0);    
	
/************          3·ֱ���������           ***********/
//DCM1  ��·����
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
	
	ptrToSend[1] = 0;// 0- �ر� 1- 24V 2 - 12V
	BXW_StartUp((STARTUP *)ptrToSend);
	
	OSTimeDlyHMSM(0,0,1,0);for(i = 0;i< 3;i++)
	{
		ptrToSend[0] = 0x01;ptrToSend[1] = 0x00;	//����ָ��
		ptrToSend[2] = 0xFF;
		DateToSend((uint8_t *)ptrToSend,3,0x02,0x8F);
		OSTimeDlyHMSM(0,0,0,5);
	}	
	
	Mem_free((void **)&ptrToSend);
	varOperaRun.sysMode = IDLE_MODE;//��ģʽ
}
//��ѹ�������
void Vol_Res_Test(uint8_t volCh)
{
	uint8_t mode,cha,resCh,checkSum,i;
	uint8_t *ptrToSend;
	ptrToSend = Mem_malloc(15);
	switch(volCh)
	{
/**************      ��·��ѹ����      ****************/
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
/**************      ��·�������      ****************/
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
//ͨ���������л�
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
	
	RS485SendDatas((const uint8_t *)ptrToSend,7);	//������λ��	�л�ͨ��
	OSTimeDlyHMSM(0,0,0,100);//��ʱ 1 ������ͨ��
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
	
	BXW_PWMDataDeal((PWM_Data *)ptrSet,loadCmd - 0x11);	//����ͨ��       
	OSTimeDlyHMSM(0,testMin,testSec,0);	//����ʱ��   
	ptrSet[1] = 0;						//ָֹͣ��
	BXW_PWMDataDeal((PWM_Data *)ptrSet,loadCmd - 0x11);	//�ر�ͨ��
	
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
	
	RS485SendDatas((const uint8_t *)ptrToSend,7);//������λ�� �л�ͨ��
	OSTimeDlyHMSM(0,0,0,100);//��ʱ ����ͨ��
	ptrSet[0] = 0;			//cmdID
	ptrSet[1] = 1;ptrSet[2] = 1;
	duty = 990;fre = 1000;
	memcpy(&ptrSet[3],&duty,2);
	memcpy(&ptrSet[5],&fre,2);
	
	BXW_DCMDataDeal((DCM_Data *)ptrSet,loadCmd - 0x1A);	//����ͨ��   ��ת    
	OSTimeDlyHMSM(0,testMin,testSec/2,500);	//����ʱ��
	ptrSet[1] = 1;ptrSet[2] = 0;
	BXW_DCMDataDeal((DCM_Data *)ptrSet,loadCmd - 0x1A);	//����ͨ��   ��ת   
	OSTimeDlyHMSM(0,testMin,testSec/2,500);	//����ʱ��
	
	ptrSet[1] = 0;							//ָֹͣ��
	BXW_DCMDataDeal((DCM_Data *)ptrSet,loadCmd - 0x1A);	//�ر�ͨ��
	
	Mem_free((void **)&ptrSet);
	Mem_free((void **)&ptrToSend);
	OSTimeDlyHMSM(0,0,0,100);
}










