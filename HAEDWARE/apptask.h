
#ifndef __APPTASK_H__
#define __APPTASK_H__

#include "includes.h"


/******************	****    �������ȼ�     **********************/
#define MUX_595_PRIO			8	// 595 �������ź���

#define MESS_TASK_PRIO			12	//ƽ����մ�������

#define BTSEND_TASK_PRIO		14	//������������
#define CAN_TASK_PRIO			16	//CAN ��������

#define USB_S_TASK_PRIO			17  //USB �����������ȼ�
#define USB_R_TASK_PRIO			18  //USB �����������ȼ�


#define K_TASK_PRIO				20	//K�� ��������
#define RS232_TASK_PRIO			21	//232 �շ�����
#define RS485_TASK_PRIO			22	//485 �շ�����
#define START_TASK_PRIO			23	//ϵͳ��ʼ����
#define PWM_TASK_PRIO			27	//PWM ���� ����


#define ADS1_TASK_PRIO			35	//ADCоƬ �ɼ�����
#define ADC_TASK_PRIO			37	//ADC���� �ɼ�����

/**********************    �����ջ��С    ********************/

#define START_STK_SIZE			128  //��ʼ �����ջ
#define MESS_STK_SIZE				128  //���� ���������ջ��С
#define BTSEND_STK_SIZE			128  //���� ���������ջ��С
#define CAN_STK_SIZE			128  // CAN �շ������ջ��С
#define K_STK_SIZE				128  // CAN �շ������ջ��С
#define ADC_STK_SIZE			128  // ADC ת�������ջ��С
#define RS485_STK_SIZE			128  // 485 �շ������ջ��С
#define RS232_STK_SIZE			128  // 232 �շ������ջ��С
#define ADS1_STK_SIZE			128  // ADS1243_1 �����ջ��С
#define PWM_STK_SIZE			128  // PWM �����ջ��С
#define USB_STK_SIZE			128  // USB �����ջ��С
/**********************    ��������    ********************/

void StartTask(void *pdata);		//��ʼ��������
void MessageDealTask(void *pdata);	//�����շ���������
void BTSendTask(void *pdata);		//����������������
void RS485Task(void *pdata);		//RS485 ������
void RS232Task(void *pdata);		//RS232 ������
void ADS1243_Task1(void *pdata);	//AD    оƬ1�ɼ�
void PWMTask(void *pdata);			//PWM   ��������
void CANTask(void *pdata);			//CAN ��������
void KLineTask(void *pdata);		//K �߽�������
void ADC_Task(void *pdata);			//ADC ����ת����������

void USBSendTask(void *pdata);		//USB ���ʹ���-����
void USBRecvTask(void *pdata);		//USB ���մ���-����

/**********************     �����ͨ������        *********************/












#endif 












