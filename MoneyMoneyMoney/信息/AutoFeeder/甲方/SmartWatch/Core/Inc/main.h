/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_it.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define SERIAL_RCV_BUF_LEN   300
#define SERIAL_SND_BUF_LEN   50

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define TRUE  1
#define FALSE 0

#define GPIO_READ_PIN(GPIOx, PINx)       HAL_GPIO_ReadPin(GPIOx, PINx)
#define GPIO_WRITE_PIN(GPIOx, PINx, VAL) HAL_GPIO_WritePin(GPIOx, PINx, VAL)

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IO_GPS_TX_Pin GPIO_PIN_2
#define IO_GPS_TX_GPIO_Port GPIOA
#define IO_GPS_RX_Pin GPIO_PIN_3
#define IO_GPS_RX_GPIO_Port GPIOA
#define IO_EE_SCL_Pin GPIO_PIN_10
#define IO_EE_SCL_GPIO_Port GPIOB
#define IO_EE_SDA_Pin GPIO_PIN_11
#define IO_EE_SDA_GPIO_Port GPIOB
#define IO_BT_TX_Pin GPIO_PIN_9
#define IO_BT_TX_GPIO_Port GPIOA
#define IO_BT_RX_Pin GPIO_PIN_10
#define IO_BT_RX_GPIO_Port GPIOA
#define IO_HEART_SCL_Pin GPIO_PIN_6
#define IO_HEART_SCL_GPIO_Port GPIOB
#define IO_HEART_SDA_Pin GPIO_PIN_7
#define IO_HEART_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
