/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_can.h"
#include "CAN_receive.h"
#include "remote_control.h"
#include "bsp_usart.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include "pid.h"
#include "oscilloscope.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
PID vec;
PID pos;
int ecd_vec = 0;
int ecd_pos = 0;
int target_vec = 200;
float target_pos = 0;//[0,8191]

fp32 cmd_P = 0;
fp32 cmd_I = 0;
fp32 cmd_D = 0;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId PIDHandle;
osThreadId yPlotHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void pid_task(void const * argument);
void yplot_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of PID */
  osThreadDef(PID, pid_task, osPriorityNormal, 0, 128);
  PIDHandle = osThreadCreate(osThread(PID), NULL);

  /* definition and creation of yPlot */
  osThreadDef(yPlot, yplot_task, osPriorityIdle, 0, 128);
  yPlotHandle = osThreadCreate(osThread(yPlot), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_pid_task */
/**
  * @brief  Function implementing the PID thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_pid_task */
void pid_task(void const * argument)
{
  /* USER CODE BEGIN pid_task */
  /* Infinite loop */
  for(;;)
  {
    // target_pos = (local_rc_ctrl->rc.ch[0] * 4095.5) / 660.0 + 4095.5;//[0,8191]-8192/2 ?  so float_pos?
	  target_pos = 4000;

	  initPID(&pos,300,5000); //482rpm-->Max_Speed
    setPIDParam(&pos,0.5,0,0);//The outer ring is a parameter, and not too large.Go from small to big and see what happens
    setPIDTarget(&pos,target_pos);//target_pos Units are the same as  ecd_pos  It's not the speed.
    ecd_pos = get_chassis_motor_measure_point(0)->ecd;
    updatePID(&pos,ecd_pos);//The resulting value is velocity, similar to taking a derivative
    
	
		initPID(&vec,15000,5000);//vec pid output current,input vec??
		setPIDTarget(&vec,pos.output);//init is first.brcause init = 0
		setPIDParam(&vec,5,0.5,0);
		ecd_vec = get_chassis_motor_measure_point(0)->speed_rpm;
		updatePID(&vec,ecd_vec);
   
	
    CAN_cmd_chassis(vec.output,0,0,0);

    HAL_Delay(2);

    osDelay(1);
  }
  /* USER CODE END pid_task */
}

/* USER CODE BEGIN Header_yplot_task */
/**
* @brief Function implementing the yPlot thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_yplot_task */
void yplot_task(void const * argument)
{
  /* USER CODE BEGIN yplot_task */
  /* Infinite loop */
  for(;;)
  {
    get_cmd();
	  send_wave();
    osDelay(1);
  }
  /* USER CODE END yplot_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
