#ifndef __MY_HEAD_H
#define __MY_HEAD_H


// 电机A和电机B的方向控制宏
#define MOTOR_A_FWD() GPIO_SetBits(GPIOA, GPIO_Pin_6) // A电机正转
#define MOTOR_A_REV() GPIO_ResetBits(GPIOA, GPIO_Pin_6) // A电机反转
#define MOTOR_B_FWD() GPIO_SetBits(GPIOA, GPIO_Pin_0) // B电机正转
#define MOTOR_B_REV() GPIO_ResetBits(GPIOA, GPIO_Pin_0) // B电机反转

// 电机A和电机B的使能控制宏
#define MOTOR_A_EN()  GPIO_SetBits(GPIOA, GPIO_Pin_7) // A电机使能
#define MOTOR_A_DIS() GPIO_ResetBits(GPIOA, GPIO_Pin_7) // A电机禁用
#define MOTOR_B_EN()  GPIO_SetBits(GPIOA, GPIO_Pin_1) // B电机使能
#define MOTOR_B_DIS() GPIO_ResetBits(GPIOA, GPIO_Pin_1) // B电机禁用




#endif
























