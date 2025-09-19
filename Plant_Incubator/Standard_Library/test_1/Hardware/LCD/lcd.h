/****************************************************************************************************
//=========================================��Դ����================================================//
//     LCDģ��                STM32��Ƭ��
//      VCC          ��          3.3V         //��Դ
//      GND          ��          GND          //��Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������ΪSPI����
//     LCDģ��                STM32��Ƭ��    
//    SDI(MOSI)      ��          PA7         //Һ����SPI��������д�ź� 
//       LED         ��          PB6         //Һ������������ź�
//       SCK         ��          PA5         //Һ����SPI����ʱ���ź�
//      DC/RS        ��          PB7         //Һ��������/��������źţ�NC��
//       RST         ��          PB8         //Һ������λ�����ź�
//       CS          ��          PB9         //Һ����Ƭѡ�����ź�
**************************************************************************************************/	
#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"	 
#include "stdlib.h"
#include <stdio.h>

/////////////////////////////////////�û�������///////////////////////////////////	 
#define USE_HORIZONTAL  	 0//����Һ����˳ʱ����ת���� 	0-0����ת��1-90����ת��2-180����ת��3-270����ת

////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���---------------- 
#define GPIO_TYPE  GPIOB  //GPIO������
#define LED      6        //�����������       PB6
#define LCD_CS   9       //Ƭѡ����            PB9
#define LCD_RS   7       //�Ĵ���/����ѡ������ PB7
#define LCD_RST  8       //��λ����            PB8


//QDtechȫϵ��ģ������������ܿ��Ʊ��������û�Ҳ���Խ�PWM���ڱ�������
#define	LCD_LED PBout(LED) //LCD����    		 PB6
//���ʹ�ùٷ��⺯���������еײ㣬�ٶȽ����½���14֡ÿ�룬���������˾�Ƽ�����
//����IO����ֱ�Ӳ����Ĵ���������IO������ˢ�����ʿ��Դﵽ28֡ÿ�룡 

//GPIO��λ�����ߣ�
#define	LCD_CS_SET  GPIO_TYPE->BSRR=1<<LCD_CS    //Ƭѡ�˿�  	
#define	LCD_RS_SET	GPIO_TYPE->BSRR=1<<LCD_RS    //����/����  	  
#define	LCD_RST_SET	GPIO_TYPE->BSRR=1<<LCD_RST   //��λ			  

//GPIO��λ�����ͣ�							    
#define	LCD_CS_CLR  GPIO_TYPE->BRR=1<<LCD_CS     //Ƭѡ�˿�  	
#define	LCD_RS_CLR	GPIO_TYPE->BRR=1<<LCD_RS     //����/����  	 
#define	LCD_RST_CLR	GPIO_TYPE->BRR=1<<LCD_RST    //��λ			  
	    											
#define LCD_Show_Xpos 3
#define LCD_Show_Large_Xpos 9

void LCD_Init(void);							  
void LCD_WR_DATA(u8 data);
void LCD_WriteRAM_Prepare(void);
void LCD_SetParam(void);
void LCD_Clear(void);
void lcd_address(u16 x,u16 y,u16 x_total,u16 y_total);
void disp_240x160(u8 *dp);
void disp_4gray_240x160(u8 *dp);
void test_display(u8 dat1,u8 dat2,u8 dat3);

void LCD_ShowChar(u16 x, u16 y, u8 num);
void LCD_ShowChar_Large(u16 x, u16 y, u8 num);


void LCD_ShowFloat_70(u16 x, u16 y, float value);
void LCD_ShowFloat_70_Large(u16 x, u16 y, float value);
void LCD_ShowInt_3000(u16 x, u16 y, int value);
void LCD_ShowInt_3000_Large(u16 x, u16 y, int value);
	

float clamp_float(float value, float min, float max);
int8_t clamp_int8(int8_t value, int8_t min, int8_t max);
int16_t clamp_int16(int16_t value, int16_t min, int16_t max);

/*-------------------------------------Graphic Drawing------------------------------------------------------------------*/
// 画点函数（如已有可直接用）
void LCD_DrawPoint(uint16_t x, uint16_t y);
// 只画圆的1/4（象限1~4），用于圆角
void LCD_DrawCirclePart(uint16_t xc, uint16_t yc, uint16_t r, uint8_t quadrant);
// 绘制圆角矩形
void LCD_DrawRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t radius, uint8_t lineWidth);
// 绘制普通矩形
void LCD_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t lineWidth);

void LCD_DrawLine_Mode(uint8_t x, uint8_t y,uint8_t x_total,uint8_t y_total);
void LCD_ClearLine_Mode(uint8_t x, uint8_t y,uint8_t x_total,uint8_t y_total);

void LCD_ShowString(u16 x, u16 y, const char* str);
void LCD_ShowString_Large(u16 x, u16 y, const char* str);


#endif