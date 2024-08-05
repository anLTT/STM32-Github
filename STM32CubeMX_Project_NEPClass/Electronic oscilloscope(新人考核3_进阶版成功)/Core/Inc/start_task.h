/**
  ***************************(C) COPYRIGHT 2022 ZJNU****************************
  * @file       start_task.c/h
  * @brief      ��ʼ���������������ɸ������̣߳��������ȼ�����ջ��С������������
	*							����
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Oct-26-2022     Asen            1. ALL DONE
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ***************************(C) COPYRIGHT 2022 ZJNU****************************
  */

#ifndef COMM_TASK_H
#define COMM_TASK_H

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "main.h"
////#include "DR16.h"


///* ���Ͷ��� ------------------------------------------------------------------*/

///* �궨�� --------------------------------------------------------------------*/
///******************************************************************************
// *                                                                   ��������                                                                         *
// ******************************************************************************/
///*---------------�� ͨ������ ��---------------*/
#define START_TASK_PERIOD           100
////#define IMU_TASK_PERIOD             5
////#define CONSOLE_TASK_PERIOD         5
////#define COMM_TASK_PERIOD            1
////#define DETECT_TASK_PERIOD          20
///*---------------�� �������� ��---------------*/
#define SEND_TASK_PERIOD         5
////#define SHOOT_TASK_PERIOD           4
///*---------------�� ��̨���� ��---------------*/
#define MOTOR_TASK_PERIOD          10
////#define GIMBAL_UPLOAD_TIMER_PERIOD  15



#define SEND_TASK_INIT_TIME 357

void motor_task(void const*argument);
void send_task(void const*argument);
void StartTask(void const*argument);

///* ��չ���� ------------------------------------------------------------------*/

///* �������� ------------------------------------------------------------------*/
////void MX_USB_DEVICE_Init(void);
#endif  // COMM_TASK_H



