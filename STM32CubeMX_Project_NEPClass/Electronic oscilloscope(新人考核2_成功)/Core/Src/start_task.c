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

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "start_task.h"
#include "cmsis_os.h"
#include "bsp_can.h"
////#include "bsp_delay.h"
#include "main.h"
#include "pid.h"
#include "CAN_receive.h"
#include "remote_control.h"
////#include "GimbalThread.h"
////#include "IMU_BMI088Thread.h"
////#include "shoot_task.h"

///* ˽�����Ͷ��� --------------------------------------------------------------*/

///* ˽�к궨�� ----------------------------------------------------------------*/
#define SEND_TASK_PRIO 18  //�������ȼ�
#define SEND_STK_SIZE 512 //�����ջ4
TaskHandle_t sendTask_Handler; //������
TaskHandle_t motorTask_Handler; 
////TaskHandle_t imuTask_Handler; 
////TaskHandle_t shootTask_Handler; 
osThreadId sendTaskHandle;
osThreadId motorTaskHandle;
////osThreadId imuTaskHandle;
////osThreadId shootTaskHandle;


///* ˽�б��� ------------------------------------------------------------------*/
//#if INCLUDE_uxTaskGetStackHighWaterMark
//		//static uint32_t start_task_stack = 0;
//#endif


extern char usart_readbuff[30];//���ڽ��ջ�������
extern u8 usart_readok;//һ֡���ݴ����־

extern fp32 cmd_P;
extern fp32 cmd_I;
extern fp32 cmd_D;


extern PID vec;
extern PID pos;
extern int ecd_vec;
extern float ecd_pos;
extern int target_vec;
extern float target_pos;//[0,8191]

///* ��չ���� ------------------------------------------------------------------*/

///* ˽�к���ԭ�� --------------------------------------------------------------*/












void send_task(void const*argument)
{
	//wait a time
	//����һ��ʱ��
	vTaskDelay(SEND_TASK_INIT_TIME);
	
	//������λ�����͵�����
	get_cmd();
	
	while(1)
	{
	osDelay(1);
	}
		
	
}


void motor_task(void const*argument)
{//���ͽǶ���ֵ�����,���ҵ������
	target_pos = cmd_P;
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


	while(1)
	{
		
	}
}




///* ������ --------------------------------------------------------------------*/
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
	
	osThreadDef(SendTask, send_task, osPriorityAboveNormal, 0, 512);//��������send_task();
  sendTaskHandle = osThreadCreate(osThread(SendTask), NULL);
	
  osThreadDef(MotorTask, motor_task, osPriorityAboveNormal, 0, 512);         //��������Motor_task();
  motorTaskHandle = osThreadCreate(osThread(MotorTask), NULL);
//	
//	osThreadDef(IMUTask, imu_task, osPriorityRealtime, 0, 1024);          //��������imu_task();
//  imuTaskHandle = osThreadCreate(osThread(IMUTask), NULL);
//	
//	osThreadDef(ShootTask, shoot_task, osPriorityAboveNormal, 0, 512);    //��������shoot_task();
//  shootTaskHandle = osThreadCreate(osThread(ShootTask), NULL);
	
	/*xTaskCreate((TaskFunction_t)Chassis_task,          //��������chassis_task();
                (const char *)"Chassis_task",          //�������ƣ���������
                (uint16_t)CHASSIS_STK_SIZE,            //�����ջ��С
                (void *)NULL,                        //���ݸ��������Ĳ���
                (UBaseType_t)CHASSIS_TASK_PRIO,        //�������ȼ���18
                (TaskHandle_t *)&ChassisTask_Handler); //������
	vTaskDelete(StartTask_Handler);	*/  
    for(;;)
    {
        osDelay(START_TASK_PERIOD);
#if INCLUDE_uxTaskGetStackHighWaterMark
       // start_task_stack = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}









