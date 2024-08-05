/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

const RC_ctrl_t *local_rc_ctrl;

int SP_TARGET[2] = {0,0};
int PO_TARGET[2] = {0,0};

int id = 0;

pid_t motor_pid_speed[2];//PID速度环结构体定义
pid_t motor_pid_location[2];//PID位置环结构体定义

float motor_control_val[2] = {0,0};//PID结果输出存储






void usart_printf(const char *fmt,...)
{
    static uint8_t tx_buf[256] = {0};
    static va_list ap;
    static uint16_t len;
    va_start(ap, fmt);

    //return length of string 
    
    len = vsprintf((char *)tx_buf, fmt, ap);

    va_end(ap);

//    usart6_tx_dma_enable(tx_buf, len);
	HAL_UART_Transmit_DMA(&huart6, tx_buf, len);
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
    can_filter_init();
    remote_control_init();
    usart6_tx_dma_init();
    local_rc_ctrl = get_remote_control_point();
	
	PID_struct_init(motor_pid_speed,POSITION_PID,8000,8000,1.5f,0.0f,0.3f);//电机1速度环PID结构体初始化	
	PID_struct_init((motor_pid_speed+1),POSITION_PID,8000,8000,1.5f,0.0f,0.3f);//电机2速度环PID结构体初始化	
	PID_struct_init(motor_pid_location,POSITION_PID,8000,8000,1.5f,0.0f,0.3f);//电机1位置环PID结构体初始化	
	PID_struct_init((motor_pid_location+1),POSITION_PID,8000,8000,1.5f,0.0f,0.3f);//电机2位置环PID结构体初始化	
	
	motor_pid_speed[0].deadband=200;//电机1速度计算死区
	motor_pid_speed[1].deadband=200;//电机2速度计算死区
	motor_pid_location[0].deadband=5000;//电机1位置计算死区
	motor_pid_location[1].deadband=5000;//电机2位置计算死区
	
	
	
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  SP_TARGET[id] = (local_rc_ctrl->rc.ch[id])*16384/660;//电机预期速度
//	  PO_TARGET[id] = (local_rc_ctrl->rc.ch[id])*8191/660/2;//电机预期转子位置
	  const motor_measure_t *motor_data = get_chassis_motor_measure_point(id);
	  
	  motor_control_val[id] = motor_pid_location[id].f_cal_pid((motor_pid_location+id),motor_data->ecd,SP_TARGET[id]);//这两边该填什么motor_data->speed_rpm,TARGET[id]);//PID计算
	  
	  

	if((motor_control_val[id] > SP_TARGET[id])&&(SP_TARGET[id]>=0))//速度阈值限制
	{motor_control_val[id] = SP_TARGET[id];}
	else if((motor_control_val[id] < -SP_TARGET[id])&&(SP_TARGET[id]>=0))
	{motor_control_val[id] = -SP_TARGET[id];}
	else if((motor_control_val[id] < SP_TARGET[id])&&(SP_TARGET[id]<0))
	{motor_control_val[id] = SP_TARGET[id];}
	else if((motor_control_val[id] > -SP_TARGET[id])&&(SP_TARGET[id]<0))
	{motor_control_val[id] = -SP_TARGET[id];}

	  
	  motor_control_val[id] = motor_pid_speed[id].f_cal_pid((motor_pid_speed+id),motor_data->speed_rpm,motor_control_val[id]);//位置环PID计算
	  
	  
	  
     usart_printf(
 "**********\r\n\
 ch0:%d\r\n\
 ch1:%d\r\n\
 ch2:%d\r\n\
 ch3:%d\r\n\
 ch4:%d\r\n\
 s1:%d\r\n\
 s2:%d\r\n\
 mouse_x:%d\r\n\
 mouse_y:%d\r\n\
 press_l:%d\r\n\
 press_r:%d\r\n\
 key:%d\r\n\
 SP_TARGET:%d\r\n\
 **********\r\n",
             local_rc_ctrl->rc.ch[0], local_rc_ctrl->rc.ch[1], local_rc_ctrl->rc.ch[2], local_rc_ctrl->rc.ch[3], local_rc_ctrl->rc.ch[4],
             local_rc_ctrl->rc.s[0], local_rc_ctrl->rc.s[1],
             local_rc_ctrl->mouse.x, local_rc_ctrl->mouse.y,local_rc_ctrl->mouse.z, local_rc_ctrl->mouse.press_l, local_rc_ctrl->mouse.press_r,
             local_rc_ctrl->key.v,SP_TARGET);

	
    CAN_cmd_chassis(motor_control_val[id],0,0,0);

	

    HAL_Delay(2);
  
  
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
