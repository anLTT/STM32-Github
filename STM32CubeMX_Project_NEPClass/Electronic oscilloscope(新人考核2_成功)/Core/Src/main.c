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
#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oscilloscope.h"
#include "struct_typedef.h"
#include <stdio.h>
#include "string.h"
#include "bsp_can.h"
#include "bsp_usart.h"
#include "CAN_receive.h"
#include "remote_control.h"
#include <stdarg.h>
#include "pid.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_hal_uart.h"
#include "start_task.h"
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

extern char usart_readbuff[30];//串口接收缓存数组
extern u8 usart_readok;//一帧数据处理标志


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const RC_ctrl_t *local_rc_ctrl;

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



PID vec;
PID pos;
int ecd_vec = 0;
float ecd_pos = 0;
int target_vec = 200;
float target_pos = 0;//[0,8191]

fp32 cmd_P = 0;
fp32 cmd_I = 0;
fp32 cmd_D = 0;



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

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
      can_filter_init();
    remote_control_init();
    usart6_tx_dma_init();
    local_rc_ctrl = get_remote_control_point();
	    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);  //receive interrupt
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);  //idle interrupt
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);  //receive interrupt
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);  //idle interrupt

  
  /* Start scheduler */

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  
	  	
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

	get_cmd();
	send_wave();
	

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

void USART6_IRQHandler(void)//串口6中断服务函数
{

  u8 temp;
  static u8 count = 0;//接收数组控制变量
  if(__HAL_UART_GET_IT_SOURCE(&huart6,USART_IT_RXNE) != RESET)//判断是否为接收中断
  {
    temp = HAL_UART_Receive(&huart6,&temp,1,HAL_MAX_DELAY);//读取接收到的数据,并清除中断标志
    if(temp == '#' && usart_readok == 0)
    {
      usart_readbuff[count] = '#';
      usart_readok = 1;
      count = 0;
    }
    else if (usart_readok == 0)
    {
      usart_readbuff[count] = temp;//保存接收到的数据到接收缓存数组
      count++;//数组下标切换
      if(count >= 30)//防止数据越界
        count = 0;      
    }
  }
  
  
  HAL_UART_IRQHandler(&huart6);
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
