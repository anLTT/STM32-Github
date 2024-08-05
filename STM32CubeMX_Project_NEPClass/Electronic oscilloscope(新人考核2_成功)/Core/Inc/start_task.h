/**
  ***************************(C) COPYRIGHT 2022 ZJNU****************************
  * @file       start_task.c/h
  * @brief      开始任务函数，负责生成各任务线程，设置优先级、堆栈大小、任务函数名等
	*							参数
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

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
////#include "DR16.h"


///* 类型定义 ------------------------------------------------------------------*/

///* 宏定义 --------------------------------------------------------------------*/
///******************************************************************************
// *                                                                   任务周期                                                                         *
// ******************************************************************************/
///*---------------↓ 通用任务 ↓---------------*/
#define START_TASK_PERIOD           100
////#define IMU_TASK_PERIOD             5
////#define CONSOLE_TASK_PERIOD         5
////#define COMM_TASK_PERIOD            1
////#define DETECT_TASK_PERIOD          20
///*---------------↓ 底盘任务 ↓---------------*/
#define SEND_TASK_PERIOD         5
////#define SHOOT_TASK_PERIOD           4
///*---------------↓ 云台任务 ↓---------------*/
#define MOTOR_TASK_PERIOD          10
////#define GIMBAL_UPLOAD_TIMER_PERIOD  15



#define SEND_TASK_INIT_TIME 357

void motor_task(void const*argument);
void send_task(void const*argument);
void StartTask(void const*argument);

///* 扩展变量 ------------------------------------------------------------------*/

///* 函数声明 ------------------------------------------------------------------*/
////void MX_USB_DEVICE_Init(void);
#endif  // COMM_TASK_H



