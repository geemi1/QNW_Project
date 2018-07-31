
#ifndef __APPTASK_H__
#define __APPTASK_H__

#include "includes.h"


/******************	****    任务优先级     **********************/
#define MUX_595_PRIO			8	// 595 互斥型信号量

#define MESS_TASK_PRIO			12	//平板接收处理任务

#define BTSEND_TASK_PRIO		14	//蓝牙发送任务
#define CAN_TASK_PRIO			16	//CAN 接收任务

#define USB_S_TASK_PRIO			17  //USB 发送任务优先级
#define USB_R_TASK_PRIO			18  //USB 发送任务优先级


#define K_TASK_PRIO				20	//K线 接收任务
#define RS232_TASK_PRIO			21	//232 收发任务
#define RS485_TASK_PRIO			22	//485 收发任务
#define START_TASK_PRIO			23	//系统起始任务
#define PWM_TASK_PRIO			27	//PWM 产生 任务


#define ADS1_TASK_PRIO			35	//ADC芯片 采集任务
#define ADC_TASK_PRIO			37	//ADC主控 采集任务

/**********************    任务堆栈大小    ********************/

#define START_STK_SIZE			128  //起始 任务堆栈
#define MESS_STK_SIZE				128  //蓝牙 接收任务堆栈大小
#define BTSEND_STK_SIZE			128  //蓝牙 发送任务堆栈大小
#define CAN_STK_SIZE			128  // CAN 收发任务堆栈大小
#define K_STK_SIZE				128  // CAN 收发任务堆栈大小
#define ADC_STK_SIZE			128  // ADC 转换处理堆栈大小
#define RS485_STK_SIZE			128  // 485 收发任务堆栈大小
#define RS232_STK_SIZE			128  // 232 收发任务堆栈大小
#define ADS1_STK_SIZE			128  // ADS1243_1 任务堆栈大小
#define PWM_STK_SIZE			128  // PWM 任务堆栈大小
#define USB_STK_SIZE			128  // USB 任务堆栈大小
/**********************    任务声明    ********************/

void StartTask(void *pdata);		//起始任务声明
void MessageDealTask(void *pdata);	//蓝牙收发任务声明
void BTSendTask(void *pdata);		//蓝牙发送任务声明
void RS485Task(void *pdata);		//RS485 任务处理
void RS232Task(void *pdata);		//RS232 任务处理
void ADS1243_Task1(void *pdata);	//AD    芯片1采集
void PWMTask(void *pdata);			//PWM   生成任务
void CANTask(void *pdata);			//CAN 接收任务
void KLineTask(void *pdata);		//K 线接收任务
void ADC_Task(void *pdata);			//ADC 主控转换处理任务

void USBSendTask(void *pdata);		//USB 发送处理-任务
void USBRecvTask(void *pdata);		//USB 接收处理-任务

/**********************     任务间通信声明        *********************/












#endif 












