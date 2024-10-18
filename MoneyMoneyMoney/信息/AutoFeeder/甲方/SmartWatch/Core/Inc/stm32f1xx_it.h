/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
 ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdint.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct
{
  uint16_t usCnt;// * 计时�?
  uint8_t  ucFlg;// * 计时结束
}ST_TIMER_STA;

typedef struct
{
  ST_TIMER_STA st_10ms;
  ST_TIMER_STA st_100ms;
}ST_TIMER;

typedef enum
{
  E_TIMER_10MS = 0,
  E_TIMER_100MS = 1,
}ENUM_TIMER;

typedef enum
{
  E_100MS_IN_10MS_CNT = 10,
}ENUM_TIME_CNT;

extern ST_TIMER g_st_timer;// * stm32f1xx_it.c 外部只允许查�? g_st_timer 状�?�，不允许修�?

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM1_UP_IRQHandler(void);
void TIM4_IRQHandler(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
/* USER CODE BEGIN EFP */
extern uint8_t TIM_TIMER_STA(ENUM_TIMER e_timer);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */
