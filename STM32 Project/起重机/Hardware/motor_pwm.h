#ifndef __PWM_H
#define __PWM_H

#include "stm32f10x.h"

// 定义PWM通道和控制引脚
#define PWMA  TIM1->CCR1 // PA8
#define PWMB  TIM1->CCR4 // PA11

// 电机A和电机B的控制引脚
#define AIN1  GPIOB->ODR |= (1 << 14) // PB14
#define AIN2  GPIOB->ODR &= ~(1 << 15) // PB15
#define BIN1  GPIOB->ODR |= (1 << 13) // PB13
#define BIN2  GPIOB->ODR &= ~(1 << 12) // PB12

// 电机A和电机B的GPIO初始化[IN引脚 PWMA/B引脚]
void Motor_Init(void);
// 电机PWM初始化
void Motor_PWM_Init(u16 arr, u16 psc);
// 绝对值函数
int myabs(int a);
// 设置PWM值
void Set_Pwm(int moto1, int moto2);

#endif
 
