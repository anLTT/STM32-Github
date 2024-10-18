/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal_usart.h"
#include "GPS.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
typedef struct
{
  uint8_t pucRcvBuf[SERIAL_RCV_BUF_LEN];
  uint16_t usRcvLen;        // * 接收的数据长�?(驱动�?)
  uint16_t usRcvProcessLen; // * 接收的数据长�?(应用�?)
  uint8_t pucSndBuf[SERIAL_SND_BUF_LEN];
  uint16_t usSndLen;
  uint8_t ucTimeOut;
  uint16_t usErrTimeOut;
}ST_SERIAL;

extern ST_SERIAL g_stSerial1;
extern ST_SERIAL g_stSerial2;


extern void Uart_Rcv_Rdy(uint8_t ucUARTNum);
extern void Uart_TimeOut(void);
extern void Uart_ErrCheck(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

