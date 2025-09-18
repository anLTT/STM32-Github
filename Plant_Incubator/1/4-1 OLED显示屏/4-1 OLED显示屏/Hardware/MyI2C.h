#ifndef __MYI2C_H
#define __MYI2C_H
#include "stm32f10x.h"
 
 
 
#define SDA_IN()  {GPIOB->CRH &= 0xFFFF0FFF; GPIOB->CRH |= 0x00008000;}  // 设置PB11为输入模式
#define SDA_OUT() {GPIOB->CRH &= 0xFFFF0FFF; GPIOB->CRH |= 0x00003000;}  // 设置PB11为输出模式
 
 
#define IIC_SCL(n) (n ? GPIO_WriteBit(GPIOB, GPIO_Pin_10, 1) : GPIO_WriteBit(GPIOB, GPIO_Pin_10, 0)) // SCL设置为PB10
#define IIC_SDA(n) (n ? GPIO_WriteBit(GPIOB, GPIO_Pin_11, 1) : GPIO_WriteBit(GPIOB, GPIO_Pin_11, 0)) // SDA设置为PB11
 
#define READ_SDAA  		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//输入SDA
 
 
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
 
 
#endif