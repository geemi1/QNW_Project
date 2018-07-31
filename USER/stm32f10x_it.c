/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "includes.h"

void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 

 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 



 //ϵͳʱ�ӵδ𣨲�Ҫ�Ķ����ǳ���Ҫ��
void SysTick_Handler(void)
{	
	OSIntEnter();							//�����ж�
	OSTimeTick();       					//����ucos��ʱ�ӷ������               
	OSIntExit();       	 					//���������л����ж�
}
//��ʱ��6 �ж�
#define GP   GPIOE//GPIOD   GPIOE
#define PIN  GPIO_Pin_14

extern OS_EVENT * BXWSendSem;		//��Я��ͨ�����������ź���
extern OS_EVENT * KFastStartSem;	//K�߿��ٳ�ʼ���ź���

uint16_t timeKStart = 0;
uint16_t timeMode  = 0;
extern TIMEOUTPUT_pwm lowPWMOC[8];	//����PWM
uint16_t timeCnt = 0;
uint8_t hlSet = 0;
void TIM6_IRQHandler(void)
{
	uint8_t i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	OSIntEnter();
	TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	
	if(varVciRun.kFastStart == OPEN)//K �߿��ٳ�ʼ��
	{
		timeKStart ++;
		if(timeKStart<6)
			KStartPin = 0;
		else if(timeKStart>=26)
			KStartPin = 1;
		if(timeKStart>=51)	
		{
			varVciRun.kFastStart = CLOSE;
			OSSemPost(KFastStartSem);//K �߿��ٳ�ʼ�����
		}
	}
	else
		timeKStart = 0;


	if(varOperaRun.sysMode == BXW_MODE)			
	{
		if(timeMode%200 == 199)
		{	
			OSSemPost(BXWSendSem);//��ʱ���ͱ�Я����Ϣ
			timeMode = 0;
		}
		timeMode ++;
	}

	//����Сռ�ձȵĲ��Σ��鿴�Ƿ���Ҫ��ת
	
	for(i = 0;i < 8;i++)
	{
		if((lowPWMOC[i].isOPEN == CLOSE)&&(lowPWMOC[i].pwmType == 0))
			continue;	
		else   //8·��ƵPWM����
		{
			if(lowPWMOC[i].isOPEN == CLOSE)//�ر�PWMͨ��
			{
				GPIO_InitStructure.GPIO_Pin   = 1<<lowPWMOC[i].pwmControl->pinPWM;	// PWM  OUT1 I 
				GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(lowPWMOC[i].pwmControl->GPIOPWM,&GPIO_InitStructure);
				if(lowPWMOC[i].pwmControl->level == 1)
					GPIO_SetBits(lowPWMOC[i].pwmControl->GPIOPWM,(1<<lowPWMOC[i].pwmControl->pinPWM));
				else
					GPIO_ResetBits(lowPWMOC[i].pwmControl->GPIOPWM,(1<<lowPWMOC[i].pwmControl->pinPWM));
				lowPWMOC[i].pwmType = 0;
				lowPWMOC[i].pwmControl->ocType = 0;
				lowPWMOC[i].pwmControl->numPWMOC = 20;
				lowPWMOC[i].pwmControl = NULL;
			}
			else //PWM ͨ����������
			{
				lowPWMOC[i].timeCnt ++;
				if(lowPWMOC[i].timeCnt<=lowPWMOC[i].hTime)//�ߵ�ƽ
				{	
					if(lowPWMOC[i].hlSet != 1)
					{	
						
						if(lowPWMOC[i].pwmType == SMALL_PWM)
						{	//��ȡ�����Ƿ����
							lowPWMOC[i].pwmControl->pinState = GPIO_ReadInputDataBit(lowPWMOC[i].pwmControl->GPIO_In,(1<<lowPWMOC[i].pwmControl->pinIn));
							GPIO_InitStructure.GPIO_Pin   = 1<<lowPWMOC[i].pwmControl->pinPWM;	// PWM  OUT1 I 
							GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
							GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
							GPIO_Init(lowPWMOC[i].pwmControl->GPIOPWM,&GPIO_InitStructure);
						}
						else if(lowPWMOC[i].pwmType == BIG_PWM)
						{
							GPIO_InitStructure.GPIO_Pin   =1<<lowPWMOC[i].pwmControl->pinPWM;	// PWM  OUT1 I 
							GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
							GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
							GPIO_Init(lowPWMOC[i].pwmControl->GPIOPWM,&GPIO_InitStructure);
							GPIO_SetBits(lowPWMOC[i].pwmControl->GPIOPWM,(1<<lowPWMOC[i].pwmControl->pinPWM));
						}
						lowPWMOC[i].hlSet = 1;
					}
				}else//�͵�ƽ
				{
					if(lowPWMOC[i].timeCnt>= lowPWMOC[i].period)
						lowPWMOC[i].timeCnt = 0;
					if(lowPWMOC[i].hlSet !=0)
					{
						//�ɼ�PWM����״̬
						if(lowPWMOC[i].pwmType == BIG_PWM)
							lowPWMOC[i].pwmControl->pinState = GPIO_ReadInputDataBit(lowPWMOC[i].pwmControl->GPIO_In,(1<<lowPWMOC[i].pwmControl->pinIn));
//						if(lowPWMOC[i].pwmControl->num < 6)
//							
//						else if(lowPWMOC[i].pwmControl->num == 6 || lowPWMOC[i].pwmControl->num == 7)
//							lowPWMOC[i].pwmControl->pinState = GPIO_ReadInputDataBit(lowPWMOC[i].pwmControl->GPIO_In,(1<<lowPWMOC[i].pwmControl->pinIn));
						if(lowPWMOC[i].pwmType == SMALL_PWM)
						{
							GPIO_InitStructure.GPIO_Pin   = 1<<lowPWMOC[i].pwmControl->pinPWM;	// PWM  OUT1 I 
							GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
							GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
							GPIO_Init(lowPWMOC[i].pwmControl->GPIOPWM,&GPIO_InitStructure);
							//�߿أ���Ч��ƽΪ��
							if(lowPWMOC[i].pwmControl->num == 6||lowPWMOC[i].pwmControl->num == 7)//�߿�
								GPIO_ResetBits(lowPWMOC[i].pwmControl->GPIOPWM,(1<<lowPWMOC[i].pwmControl->pinPWM));
							else//�Ϳ�  ��Ч��ƽΪ��
								GPIO_SetBits(lowPWMOC[i].pwmControl->GPIOPWM,(1<<lowPWMOC[i].pwmControl->pinPWM));
						}
						else
							GPIO_ResetBits(lowPWMOC[i].pwmControl->GPIOPWM,(1<<lowPWMOC[i].pwmControl->pinPWM));
						lowPWMOC[i].hlSet = 0;
					}
				}
			}
		}
	}
	OSIntExit();			//�жϷ��������ϵͳ�����������
}

extern TIMEOUTPUT_pwm  pwmOC[8];
uint16_t nowCnt;
uint16_t ccrVal;
extern DCM_Control DCMDat[4];
void PWMTurnOC(TIMEOUTPUT_pwm *ptrOC)
{ 
	//�ж��Ƿ���Ҫ�رմ�ͨ��
	if(ptrOC->isOPEN == CLOSE)
	{	
		switch(ptrOC->pwmType)
		{	//�߾��ȸ�Ƶ��
			case BIG_PWM:
				ptrOC->pwmControl->numPWMOC = 20;
				ptrOC->pwmControl->ocType = 0;
				if(ptrOC->pwmControl->level == 1)	//�ָ�ΪĬ�ϵ�ƽ
					GPIO_SetBits(ptrOC->pwmControl->GPIOPWM,1<<ptrOC->pwmControl->pinPWM);
				else
					GPIO_ResetBits(ptrOC->pwmControl->GPIOPWM,1<<ptrOC->pwmControl->pinPWM);
				ptrOC->pwmControl = NULL;break;
			//ֱ���������
			case DC_MOTOR:
				ptrOC->dcmControl->numPWMOC = 20;
				GPIO_SetBits(ptrOC->dcmControl->GPIO_FWD,1<<ptrOC->dcmControl->fwd_Pin);
				GPIO_SetBits(ptrOC->dcmControl->GPIO_REV,1<<ptrOC->dcmControl->rev_Pin);
				ptrOC->dcmControl = NULL;
				break;	
			//�����������
			case STEP_MOTOR:
				ptrOC->dcmControl->numPWMOC = 20;//�ر��������
				ptrOC->stepNum = 0;
				GPIO_SetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
				GPIO_SetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
				GPIO_SetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
				GPIO_SetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
				break;	
			default:break;
		}
		ptrOC->pwmType = 0;
		TIM_ITConfig(ptrOC->TIMBig,ptrOC->IT_Source,DISABLE);//�رմ�ͨ��
	}
	else	//ͨ������������ PWM ��������Ҫ�Ĳ���
	{
		switch(ptrOC->pwmType)
		{
			case BIG_PWM:
				if(ptrOC->hlSet == 1)  //�ߵ�ƽ
				{  
					ptrOC->hlSet = 0;
					GPIO_SetBits(ptrOC->pwmControl->GPIOPWM,1<<ptrOC->pwmControl->pinPWM);
					ccrVal=ptrOC->hTime;          //���Ե͵�ƽ�İٷֱ�Ϊ5000/7500=2/3����ռ�ձ�Ϊ1/3  
				}  
				else	//�͵�ƽ
				{  //Ҫ�ڷ�ת�͵�ƽ��ʱ�����
					if(ptrOC->pwmControl->num < 6)
						ptrOC->pwmControl->pinState = GPIO_ReadInputDataBit(ptrOC->pwmControl->GPIO_In,(1<<ptrOC->pwmControl->pinIn));
					else if(ptrOC->pwmControl->num == 6 || ptrOC->pwmControl->num == 7)
						ptrOC->pwmControl->pinState = GPIO_ReadInputDataBit(ptrOC->pwmControl->GPIO_In,(1<<ptrOC->pwmControl->pinIn));
					ptrOC->hlSet = 1;
					GPIO_ResetBits(ptrOC->pwmControl->GPIOPWM,1<<ptrOC->pwmControl->pinPWM);
					ccrVal=ptrOC->lTime; 
				}break;
			case DC_MOTOR:
				if(ptrOC->hlSet == 1) //�ߵ�ƽ
				{  
					ptrOC->hlSet = 0;
					if(ptrOC->dcmControl->fwd == 1)//��ǰ
					{
						GPIO_ResetBits(ptrOC->dcmControl->GPIO_FWD,1<<ptrOC->dcmControl->fwd_Pin);
						GPIO_ResetBits(ptrOC->dcmControl->GPIO_REV,1<<ptrOC->dcmControl->rev_Pin);
					}
					else	//���
					{
						GPIO_ResetBits(ptrOC->dcmControl->GPIO_FWD,1<<ptrOC->dcmControl->fwd_Pin);
						GPIO_SetBits(ptrOC->dcmControl->GPIO_REV,1<<ptrOC->dcmControl->rev_Pin);
					}
					ccrVal=ptrOC->hTime;//���Ե͵�ƽ�İٷֱ�Ϊ 5000/7500 = 2/3����ռ�ձ�Ϊ1/3  
				}else	//�͵�ƽ
				{  
					ptrOC->hlSet = 1;
					GPIO_SetBits(ptrOC->dcmControl->GPIO_FWD,1<<ptrOC->dcmControl->fwd_Pin);
					GPIO_SetBits(ptrOC->dcmControl->GPIO_REV,1<<ptrOC->dcmControl->rev_Pin);
					ccrVal=ptrOC->lTime; 
				}break;
			case STEP_MOTOR:
				if(ptrOC->direct == 1)	//��������
					ptrOC->stepNum ++;
				else					//��������
					ptrOC->stepNum --;
				if(ptrOC->stepNum < 1)
					ptrOC->stepNum = 4;
				else if(ptrOC->stepNum > 4)
					ptrOC->stepNum = 1;
				if(DCMDat[3].stepErr == 0)
				{
					switch(ptrOC->stepNum)
					{
						case 1://A+
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_SetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						case 2://A-
							GPIO_SetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						case 3://B+
						
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_SetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_SetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						case 4://B-
							GPIO_SetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_SetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						default:
							break;
					}
				}else{
					switch(ptrOC->stepNum)
					{
						case 1://A+
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_SetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						case 2://A-
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_SetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_SetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						case 3://B+
							GPIO_SetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						case 4://B-
							GPIO_SetBits(ptrOC->dcmControl1->GPIO_FWD,1<<ptrOC->dcmControl1->fwd_Pin);
							GPIO_ResetBits(ptrOC->dcmControl1->GPIO_REV,1<<ptrOC->dcmControl1->rev_Pin);
							GPIO_ResetBits(ptrOC->dcmControl2->GPIO_FWD,1<<ptrOC->dcmControl2->fwd_Pin);
							GPIO_SetBits(ptrOC->dcmControl2->GPIO_REV,1<<ptrOC->dcmControl2->rev_Pin);
							break;
						default:break;
					}
				}
				ccrVal = ptrOC->periodCalc;
				break;
			default:break;
		}
		switch(ptrOC->OCNumBig)
		{
			case 1:nowCnt = TIM_GetCapture1(ptrOC->TIMBig);  //��õ�ǰʱ��
					TIM_SetCompare1(ptrOC->TIMBig, nowCnt + ccrVal); break;
			case 2:nowCnt = TIM_GetCapture2(ptrOC->TIMBig);  //��õ�ǰʱ��
					TIM_SetCompare2(ptrOC->TIMBig, nowCnt + ccrVal); break;
			case 3:nowCnt = TIM_GetCapture3(ptrOC->TIMBig);  //��õ�ǰʱ��
					TIM_SetCompare3(ptrOC->TIMBig, nowCnt + ccrVal); break;
			case 4:nowCnt = TIM_GetCapture4(ptrOC->TIMBig);  //��õ�ǰʱ��
					TIM_SetCompare4(ptrOC->TIMBig, nowCnt + ccrVal); break;
		}
	}
}

void TIM4_IRQHandler(void)
{
	OSIntEnter();
	if(TIM_GetITStatus(TIM4, TIM_IT_CC1) != RESET)  
    {  
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);  
		PWMTurnOC(&pwmOC[0]);
    }  
    else if (TIM_GetITStatus(TIM4, TIM_IT_CC2) != RESET)  
    {  
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC2); 
		PWMTurnOC(&pwmOC[1]);
    }  
    else if (TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)  
    {  
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC3);  
		PWMTurnOC(&pwmOC[2]);
    }  
    else  
    {  
        TIM_ClearITPendingBit(TIM4, TIM_IT_CC4);  
		PWMTurnOC(&pwmOC[3]);
    }  
	OSIntExit();
}
//���� PWM ���ж�
void TIM5_IRQHandler(void)
{
	OSIntEnter();
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)  
    {  
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);  
		PWMTurnOC(&pwmOC[4]);
    }  
    else if (TIM_GetITStatus(TIM5, TIM_IT_CC2) != RESET)  
    {  
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC2); 
		PWMTurnOC(&pwmOC[5]);
    }  
    else if (TIM_GetITStatus(TIM5, TIM_IT_CC3) != RESET)  
    {  
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC3);  
		PWMTurnOC(&pwmOC[6]);
    }  
    else  //ֱ���������
    {  
        TIM_ClearITPendingBit(TIM5, TIM_IT_CC4); 
		PWMTurnOC(&pwmOC[7]);
    }  
	OSIntExit();
}






