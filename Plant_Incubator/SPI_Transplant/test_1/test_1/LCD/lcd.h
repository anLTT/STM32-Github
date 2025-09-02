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
**************************************************************************************************/	#ifndef __LCD_H
#define __LCD_H		
#include "main.h"
#include "stdlib.h"
	
	
	typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

/////////////////////////////////////�û�������///////////////////////////////////	 
#define USE_HORIZONTAL  	 0//����Һ����˳ʱ����ת���� 	0-0����ת��1-90����ת��2-180����ת��3-270����ת


#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< Peripheral base address in the alias region */
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
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


u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte,u8 cmd);
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet);
				  		 
#endif  
	 
	 



