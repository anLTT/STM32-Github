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

/* 包含头文件 ----------------------------------------------------------------*/
#include "start_task.h"
#include "cmsis_os.h"
#include "bsp_can.h"
////#include "bsp_delay.h"
#include "main.h"
#include "pid.h"
#include "CAN_receive.h"
#include "remote_control.h"
#include "oscilloscope.h"
////#include "GimbalThread.h"
////#include "IMU_BMI088Thread.h"
////#include "shoot_task.h"

///* 私有类型定义 --------------------------------------------------------------*/

///* 私有宏定义 ----------------------------------------------------------------*/
#define SEND_TASK_PRIO 18  //任务优先级
#define SEND_STK_SIZE 512 //任务堆栈4
TaskHandle_t sendTask_Handler; //任务句柄
TaskHandle_t motorTask_Handler; 
////TaskHandle_t imuTask_Handler; 
////TaskHandle_t shootTask_Handler; 
osThreadId sendTaskHandle;
osThreadId motorTaskHandle;
////osThreadId imuTaskHandle;
////osThreadId shootTaskHandle;


///* 私有变量 ------------------------------------------------------------------*/
//#if INCLUDE_uxTaskGetStackHighWaterMark
//		//static uint32_t start_task_stack = 0;
//#endif


extern char usart_readbuff[30];//串口接收缓存数组
extern u8 usart_readok;//一帧数据处理标志

extern fp32 cmd_P;
extern fp32 cmd_I;
extern fp32 cmd_D;


extern PID vec;
extern PID pos;
extern int ecd_vec;
extern float ecd_pos;
extern int target_vec;
extern float target_pos;//[0,8191]
extern const RC_ctrl_t *local_rc_ctrl;


///* 扩展变量 ------------------------------------------------------------------*/

///* 私有函数原形 --------------------------------------------------------------*/












void send_task(void const*argument)
{
	//wait a time
	//空闲一段时间
	vTaskDelay(SEND_TASK_INIT_TIME);
  for(;;)
  {
    get_cmd();
	send_wave();
    osDelay(1);
  }
}


void motor_task(void const*argument)
{//发送角度数值给电机,并且电机接收
  /* Infinite loop */
  for(;;)
  {
    target_pos = (local_rc_ctrl->rc.ch[0] * 4095.5) / 660.0 + 4095.5;//遥控器数据换算成目标角度

	initPID(&pos,300,5000); //位置环PID
    setPIDParam(&pos,0.5,0,0);
    setPIDTarget(&pos,target_pos);
    ecd_pos = get_chassis_motor_measure_point(0)->ecd;
    updatePID(&pos,ecd_pos);
    	
	initPID(&vec,15000,5000);//速度环PID
	setPIDTarget(&vec,pos.output);
	setPIDParam(&vec,5,0.5,0);
	ecd_vec = get_chassis_motor_measure_point(0)->speed_rpm;
	updatePID(&vec,ecd_vec);
   
    CAN_cmd_chassis(vec.output,0,0,0);//控制电机
	  
    osDelay(2);
  }
}




///* 函数体 --------------------------------------------------------------------*/
///**
//  * @brief  Function implementing the startTask thread.
//  * @param  argument: Not used
//  * @retval None
//  */
void StartTask(void const*argument)
{
	  /* init code for USB_DEVICE */
  can_filter_init(); 
  remote_control_init();	
	
	osThreadDef(SendTask, send_task, osPriorityAboveNormal, 0, 512);//任务函数：send_task();
  sendTaskHandle = osThreadCreate(osThread(SendTask), NULL);
	
  osThreadDef(MotorTask, motor_task, osPriorityAboveNormal, 0, 512);         //任务函数：Motor_task();
  motorTaskHandle = osThreadCreate(osThread(MotorTask), NULL);
//	
//	osThreadDef(IMUTask, imu_task, osPriorityRealtime, 0, 1024);          //任务函数：imu_task();
//  imuTaskHandle = osThreadCreate(osThread(IMUTask), NULL);
//	
//	osThreadDef(ShootTask, shoot_task, osPriorityAboveNormal, 0, 512);    //任务函数：shoot_task();
//  shootTaskHandle = osThreadCreate(osThread(ShootTask), NULL);
	
	/*xTaskCreate((TaskFunction_t)Chassis_task,          //任务函数：chassis_task();
                (const char *)"Chassis_task",          //任务名称：底盘任务
                (uint16_t)CHASSIS_STK_SIZE,            //任务堆栈大小
                (void *)NULL,                        //传递给任务函数的参数
                (UBaseType_t)CHASSIS_TASK_PRIO,        //任务优先级：18
                (TaskHandle_t *)&ChassisTask_Handler); //任务句柄
	vTaskDelete(StartTask_Handler);	*/  
    for(;;)
    {
        osDelay(START_TASK_PERIOD);
#if INCLUDE_uxTaskGetStackHighWaterMark
       // start_task_stack = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}









