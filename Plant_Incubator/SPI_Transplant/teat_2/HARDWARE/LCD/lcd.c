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
**************************************************************************************************/	
#include "lcd.h"
#include "stdlib.h"
#include "delay.h"	 
#include "spi.h"


/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u8 data)
{ 
   LCD_CS_CLR;     	  
   SPI_WriteByte(SPI1,data,0);
   LCD_CS_SET;	
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u8 data)
{
   LCD_CS_CLR;
	 SPI_WriteByte(SPI1,data,1);
   LCD_CS_SET;
}
   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(0x2c);
}	 


/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_GPIOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	      
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);	//使能GPIOB时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9; //GPIOB6,7,8,9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   //推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
	delay_ms(100);	
	LCD_RST_SET;
	delay_ms(50);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  
	SPI1_Init(); //硬件SPI1初始化
	LCD_GPIOInit();//LCD GPIO初始化										 
 	LCD_RESET(); //LCD 复位
	
	//************* ST7586S初始化**********//		
	LCD_WR_REG(0x11); //退出睡眠模式
	LCD_WR_REG(0xC0); // 设置 VOP
	LCD_WR_DATA(0xf1); // 设置 VOP 的值的低 8 位（总共 9 位）,每调一级是 0.03667V
	LCD_WR_DATA(0x00); // 设置 VOP 的值的第 9 位，也是最高一位
	LCD_WR_REG(0xC3); // 设置 BIAS
	LCD_WR_DATA(0x02); // 00：BIAS = 1/14 02 = 1/12
	LCD_WR_REG(0xC4); // 设置升压倍数
	LCD_WR_DATA(0x07); // 07：8 倍压
	LCD_WR_REG(0xD0); // 允许模拟电路
	LCD_WR_DATA(0x1D); // 允许模拟电路
	LCD_WR_REG(0xB5); // N-Line = 13
	LCD_WR_DATA(0x00); // 8d
	LCD_WR_REG(0x38); // 0x38：设置为灰度模式； 0x39: 设置为黑白模式。
	LCD_WR_REG(0x3A); // 允许 DDRAM 接口：单色模式、4 灰度级、16 灰度级；
	LCD_WR_DATA(0x02); // 0x03:16 灰度级；0x02:4 灰度级或单色模式。

	LCD_WR_REG(0x36); // 扫描顺序设置
	LCD_WR_DATA(0x00); // 扫描顺序设置:MX=1,MY=1: 从左到右，从上到下的扫描顺序
	LCD_WR_REG(0xB0); // Duty 设置
	LCD_WR_DATA(0x9f); // Duty 设置:1/160
	LCD_WR_REG(0x20); // 反显设置：OFF
	LCD_WR_REG(0xf1); //温度补偿，温度变化改变帧频
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_REG(0xb1);  // 扫描起始行设置
	LCD_WR_DATA(0x00); // 扫描起始行设置：从 COM0 开始
	LCD_WR_REG(0x29);  // 打开显示：DISPLAY ON
	
	LCD_LED=1;     //点亮背光	 
	LCD_Clear();   //清全屏白色
}
 

/*写 LCD 行列地址：X 为起始的列地址，Y 为起始的行地址，x_total,y_total 分别为列地址及行地址的起点到终点的差值 */
void lcd_address(u16 x,u16 y,u16 x_total,u16 y_total)
{
	u16 x_end,y_end;
	
	x_end=x+(x_total-1)/3;
	y_end=y+y_total-1;
	
	LCD_WR_REG(0x2a);
	LCD_WR_DATA((x>>8)&0x00ff);
	LCD_WR_DATA(x&0x00ff);
	LCD_WR_DATA(x_end>>8&0x00ff);
	LCD_WR_DATA(x_end&0x00ff);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA((y>>8)&0x00ff);
	LCD_WR_DATA(y&0x00ff);
	LCD_WR_DATA(y_end>>8&0x00ff);
	LCD_WR_DATA(y_end&0x00ff);
}	
	
	
//传送同一个地址的 3 个点阵的黑白的数据：比如 SEG0、SEG1、SEG2（这 3 个点阵是同一个列地址，无法分开）
//送数据时左起第 1 列的数据是“D7 D6 D5 D4 D3 D2 D1 D0” 中的高 3 位---D7 D6 D5, 第 2 列是中 3 位---D4 D3 D2,第 3 列是低两位---D1 D0。
void transfer_mono_data_3pixel(u8 mono_data)
{
	u8 gray_data=0;
	
	if(mono_data&0x80)
	{
		gray_data=0xe0; //二进制 11100000，就是给 D7、D6、D5 赋值
	}
	else
	{
		gray_data=0;
	}
	mono_data<<=1;
	if(mono_data&0x80)
	{
		gray_data+=0x1c; //二进制 00011100，就是给 D4、D3、D2 赋值
	}
	else;
	mono_data<<=1;
	if(mono_data&0x80)
	{
		gray_data+=0x03; //二进制 00000011，就是给 D1、D0 赋值
	}
	else;
	LCD_WR_DATA(gray_data); //display 3 dots (seg_N,seg_N+1,seg_N+2)
}	

//显示 6 个点阵
void transfer_mono_data_6pixel(u8 dat1)
{
	transfer_mono_data_3pixel(dat1);
	transfer_mono_data_3pixel(dat1<<3);
}
//显示 8 个点阵
void transfer_mono_data_8pixel(u8 dat1)
{
	transfer_mono_data_3pixel(dat1);//传送 dat1 的 D7\D6\D5 这 3 位，对应 3 个点阵(第 1、2、3 个）会显示出来；列地址是自动+1 的
	transfer_mono_data_3pixel(dat1<<3);//传送 dat1 的 D4\D3\D2 这 3 位，对应 3 个点阵(第 4、5、6 个）会显示出来；列地址是自动+1 的
	transfer_mono_data_3pixel(dat1<<6);//传送 dat1 的 D1\D0 这 2 位，对应 3 个点阵(第 7、8、9 个）会显示出来
	//这个液晶驱动 IC 的每个列地址管 3 个点阵，无法分开，所以第 7、8 个点阵会连累到第 9 个点阵，结果是每次显示 9 个点阵，只不过第 9 个点阵会补“0”
	//如果第 9 个点阵本来有显示内容，就会被无情地清掉
}
//显示 9 个点阵
void transfer_mono_data_9pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_6pixel(dat1);//先显示 6 个点阵
	transfer_mono_data_3pixel((dat1<<6)|(dat2>>2)); //显示 dat1 的 D1、D0 和 dat2 的 D7 位，对应 3 个点阵(第 7、7、9 个）会显示出来；列地址是自动+1 的
}
//显示 12 个点阵
void transfer_mono_data_12pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_9pixel(dat1,dat2);//先显示 9 个点阵
	transfer_mono_data_3pixel(dat2<<1); //传送 dat2 的 D6\D5\D4 这 3 位，对应第 10、11、12 个个点阵会显示出来；列地址是自动+1的
}
//显示 15 个点阵
void transfer_mono_data_15pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_12pixel(dat1,dat2); //先显示 12 个点阵
	transfer_mono_data_3pixel(dat2<<4); //传送 dat2 的 D3\D2\D1 这 3 位，对应第 13、14、15 个点阵会显示出来；列地址是自动+1的
}
//显示 16 个点阵
void transfer_mono_data_16pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_15pixel(dat1,dat2); //先显示 15 个点阵
	transfer_mono_data_3pixel(dat2<<7); //显示第 16 个点阵,对应 dat2 的 D0 位。
//这个液晶驱动 IC 的每个列地址管 3 个点阵，无法分开，所以第 16 个点阵会连累到第 17、18 个点阵，结果是每次显示 18 个点阵， 只不过第17、18 个点阵会补“0”
//如果第 17、18 个点阵本来有显示内容，就会被无情地清掉
}
//显示 18 个点阵
void transfer_mono_data_18pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_15pixel(dat1,dat2); //先显示 15 个点阵
	transfer_mono_data_3pixel((dat2<<7)|(dat3>>1)); //传送 dat2 的 D0 和 dat3 的 D7、D6 这 3 位，对应第 16、17、18 个点阵会显示出来；列地址是自动+1 的
}
//显示 21 个点阵
void transfer_mono_data_21pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_18pixel(dat1,dat2,dat3); //先显示 18 个点阵
	transfer_mono_data_3pixel(dat3<<2); //传送 dat3 的 D5、D4、D3 这 3 位，对应第 19、20、21 个点阵会显示出来；列地址是自动+1的
}
//显示 24 个点阵。方法一：
void transfer_mono_data_24pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_21pixel(dat1,dat2,dat3); //先显示 21 个点阵
	transfer_mono_data_3pixel(dat3<<5); //传送 dat3 的 D2、D1、D0 这 3 位，对应第 22、23、24 个点阵会显示出来；列地址是自动+1的
}
//显示 27 个点阵
void transfer_mono_data_27pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //先显示 24 个点阵
	transfer_mono_data_3pixel(dat4); //传送 dat4 的 D7、D6、D5 这 3 位，对应第 25、 26、27 个点阵会显示出来；列地址是自动+1的
}
//显示 30 个点阵
void transfer_mono_data_30pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //先显示 24 个点阵
	transfer_mono_data_6pixel(dat4); //再显示 6 个点阵，24+6=30
}
//显示 32 个点阵
void transfer_mono_data_32pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //先显示 24 个点阵
	transfer_mono_data_8pixel(dat4); //再显示 8 个点阵，24+8=32
//这个液晶驱动 IC 的每个列地址管 3 个点阵，无法分开，所以第 31、32 个点阵会连累到第 33 个点阵，结果是每次显示 33 个点阵， 只不过第33 个点阵会补“0”
//如果第 33 个点阵本来有显示内容，就会被无情地清掉
}
//显示 33 个点阵
void transfer_mono_data_33pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4,u8 dat5)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //先显示 24 个点阵
	transfer_mono_data_9pixel(dat4,dat5); //再显示 9 个点阵
}
//显示 48 个点阵
void transfer_mono_data_48pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4,u8 dat5,u8 dat6)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //先显示 24 个点阵
	transfer_mono_data_24pixel(dat4,dat5,dat6); //再显示 24 个点阵
}	
	

//传送同一个地址的 3 个点阵的 4 灰度级的数据：比如 SEG0、 SEG1、SEG2,这 3 个点阵是同一个列地址， 无法分开
//送灰度数据(gray_data)时，SEG0 对应高 3 位（D7、 D6、D5）,SEG1 对应中 3 位（D4、D3、D2),SEG2 对应低两位（D1、D0）。
void transfer_gray_data_3pixel(u8 dat1)
{
	u8 gray_data;
	
	gray_data=dat1&0xc0;; //给 gray_data 的 D7、D6 赋值(=dat1 的 D7、D6)
	if((dat1&0xc0)==0xc0)
	{
		gray_data|=0x20; //给 gray_data 的 D5 赋值，当 dat1 的 D7、D6 都是 1 的时候，gray_data 的 D5=1,当 dat1 的 D7\D6 不都是 1 的时候，gray_data 的 D5=0
	}
	gray_data|=((dat1>>1)&0x18); //给 gray_data 的 D4、D3 赋值（=dat1 的 D5、D4)
	if((dat1&0x30)==0x30)
	{
		gray_data|=0x04; //给 gray_data 的 D2 赋值，当 dat1 的 D5、D4 都是 1 的时候，gray_data 的 D2=1,当 dat1 的 D7、D6 不都是 1 的时候，gray_data 的 D2=0
	}
	gray_data|=((dat1>>2)&0x03); //给 gray_data 的 D1、D0 赋值(=dat1 的 D3、D2)
	LCD_WR_DATA(gray_data); //传送 1 个字节灰度数据给液晶驱动 IC,对应的 3 个点阵会显示(seg_N,seg_N+1,seg_N+2)
}


//传送同一个地址的 12 个点阵的 4 灰度的数据：比如 SEG0、SEG1、SEG2......SEG9、SEG10、SEG11（这 12 个点阵是 4 个列地址）
//每 2 位数据对应一个点阵，12 个点阵用：2*12=24 位，即 3 个字节:dat1、dat2、dat3
void transfer_gray_data_12pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_gray_data_3pixel(dat1); //显示 3 个点阵(seg_N,seg_N+1,SEG_N+2)
	transfer_gray_data_3pixel((dat1<<6)|(dat2>>2)); //显示 3 个点阵(seg_N+3,seg_N+4,SEG_N+5)
	transfer_gray_data_3pixel((dat2<<4)|(dat3>>4)); //显示 3 个点阵(seg_N+6,seg_N+7,SEG_N+8)
	transfer_gray_data_3pixel(dat3<<2); //显示 3 个点阵(seg_N+9,seg_N+10,SEG_N+11)
}


/*清屏*/
void LCD_Clear(void)
{
	int i,j;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	
	for(i=0;i<160;i++)
	{
		for(j=0;j<20;j++)
		{
			transfer_mono_data_18pixel(0x00,0x00,0x00); //每个字节显示 8 个点阵，显示 8*3=24 个点阵
		}
	}
}


//显示 240*160 点阵的图像
void disp_240x160(u8 *dp)
{
	u16 i,j;
	u8 dat1,dat2,dat3;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	for(i=0;i<160;i++)
	{
		for(j=0;j<10;j++)//循环 10 次，每次显示 24 个点阵，合计 240 个点阵
		{
			dat1=*dp;dp++;
			dat2=*dp;dp++;
			dat3=*dp;dp++;
			transfer_mono_data_24pixel(dat1,dat2,dat3); //每个字节显示 8 个点阵，显示 8*3=24 个点阵
		}
	}
}


//===显示测试画面：例如全显示，隔行显示，隔列显示，雪花显示=====
void test_display(u8 dat1,u8 dat2,u8 dat3)
{
	u16 i,j;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	for(i=0;i<160;i++)
	{
		for(j=0;j<10;j++)//循环 10 次，每次显示 24 个点阵，合计 240 个点阵
		{
			transfer_mono_data_24pixel(dat1,dat2,dat3); //每个字节显示 8 个点阵，显示 8*3=24 个点阵
		}
	}
}


//显示 240*160 点阵的 4 灰度级图像
void disp_4gray_240x160(u8 *dp)
{
	u8 i,j;
	u8 dat1,dat2,dat3;
	
	lcd_address(0,0,240,160); //
	LCD_WriteRAM_Prepare();
	
	for(i=0;i<160;i++)
	{
		for(j=0;j<20;j++)//循环 20 次，每次显示 12 个点阵，合计 20*12=240 个点阵
		{
			dat1=*dp;dp++;
			dat2=*dp;dp++;
			dat3=*dp;dp++;
			transfer_gray_data_12pixel(dat1,dat2,dat3); //每个字节显示 4 个点阵，共显示 4*3=12 个点阵
		}
	}
}


