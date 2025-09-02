/****************************************************************************************************
//=========================================电源接线================================================//
//     LCD模块                STM32单片机
//      VCC          接          3.3V         //电源
//      GND          接          GND          //电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为SPI总线
//     LCD模块                STM32单片机    
//    SDI(MOSI)      接          PA7         //液晶屏SPI总线数据写信号 
//       LED         接          PB6         //液晶屏背光控制信号
//       SCK         接          PA5         //液晶屏SPI总线时钟信号
//      DC/RS        接          PB7         //液晶屏数据/命令控制信号（NC）
//       RST         接          PB8         //液晶屏复位控制信号
//       CS          接          PB9         //液晶屏片选控制信号
**************************************************************************************************/	#ifndef __LCD_H
#define __LCD_H		
#include "main.h"
#include "stdlib.h"
	
	
	typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

/////////////////////////////////////用户配置区///////////////////////////////////	 
#define USE_HORIZONTAL  	 0//定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转


#define PERIPH_BASE           ((uint32_t)0x40000000) /*!< Peripheral base address in the alias region */
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 
#define GPIO_TYPE  GPIOB  //GPIO组类型
#define LED      6        //背光控制引脚       PB6
#define LCD_CS   9       //片选引脚            PB9
#define LCD_RS   7       //寄存器/数据选择引脚 PB7
#define LCD_RST  8       //复位引脚            PB8


//QDtech全系列模块采用了三极管控制背光亮灭，用户也可以接PWM调节背光亮度
#define	LCD_LED PBout(LED) //LCD背光    		 PB6
//如果使用官方库函数定义下列底层，速度将会下降到14帧每秒，建议采用我司推荐方法
//以下IO定义直接操作寄存器，快速IO操作，刷屏速率可以达到28帧每秒！ 

//GPIO置位（拉高）
#define	LCD_CS_SET  GPIO_TYPE->BSRR=1<<LCD_CS    //片选端口  	
#define	LCD_RS_SET	GPIO_TYPE->BSRR=1<<LCD_RS    //数据/命令  	  
#define	LCD_RST_SET	GPIO_TYPE->BSRR=1<<LCD_RST   //复位			  

//GPIO复位（拉低）							    
#define	LCD_CS_CLR  GPIO_TYPE->BRR=1<<LCD_CS     //片选端口  	
#define	LCD_RS_CLR	GPIO_TYPE->BRR=1<<LCD_RS     //数据/命令  	 
#define	LCD_RST_CLR	GPIO_TYPE->BRR=1<<LCD_RST    //复位			  
	    															  
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
	 
	 



