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
void LCD_ShowString(u16 x, u16 y, const char *p);
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len);
void LCD_ShowChar_Large(u16 x, u16 y, u8 num);
				  		 
#endif