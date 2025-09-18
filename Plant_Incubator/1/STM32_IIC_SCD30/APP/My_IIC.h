#ifndef __MYIIC_H
#define __MYIIC_H
 
#include <stdint.h>
//*********************************************************************************
//IIC����
//
//*********************************************************************************

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

/****IIC��������****/
#define IIC_SCL_PORT  GpioPortB
#define IIC_SCL_PIN   GpioPin1
#define IIC_SDA_PORT  GpioPortB
#define IIC_SDA_PIN   GpioPin0

//IO��������	 
#define IIC_SCL    PAout(10) //SCL
#define IIC_SDA    PAout(11) //SDA	 
//#define READ_SDA   PAin(12)  //����SDA 
#define READ_SDA Gpio_GetInputIO(IIC_SDA_PORT,IIC_SDA_PIN)

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















