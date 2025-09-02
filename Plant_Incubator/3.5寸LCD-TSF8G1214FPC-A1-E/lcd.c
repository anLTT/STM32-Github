//-----------------LCD端口定义（HAL库版本）---------------- 
#define LCD_GPIO_PORT   GPIOB                  // GPIO端口组
#define LCD_LED_PIN     GPIO_PIN_6              // 背光控制引脚 (PB6)
#define LCD_CS_PIN      GPIO_PIN_9              // 片选信号引脚 (PB9)
#define LCD_RS_PIN      GPIO_PIN_7              // 命令/数据选择引脚 (PB7)
#define LCD_RST_PIN     GPIO_PIN_8              // 复位信号引脚 (PB8)


// LCD背光控制
#define LCD_LED_ON      HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_LED_PIN, GPIO_PIN_SET)    // 打开背光
#define LCD_LED_OFF     HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_LED_PIN, GPIO_PIN_RESET)  // 关闭背光


// 片选信号控制 (CS)
#define LCD_CS_SET      HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_SET)     // 片选置高（取消选中）
#define LCD_CS_CLR      HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_CS_PIN, GPIO_PIN_RESET)   // 片选置低（选中LCD）


// 命令/数据选择信号控制 (RS)
#define LCD_RS_SET      HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RS_PIN, GPIO_PIN_SET)     // RS置高（传输数据）
#define LCD_RS_CLR      HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RS_PIN, GPIO_PIN_RESET)   // RS置低（传输命令）


// 复位信号控制 (RST)
#define LCD_RST_SET     HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RST_PIN, GPIO_PIN_SET)    // 复位置高（正常工作）
#define LCD_RST_CLR     HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_RST_PIN, GPIO_PIN_RESET)  // 复位置低（复位操作）



/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  
	SPI1_Init(); //???SPI1?????
	LCD_GPIOInit();//LCD GPIO?????										 
 	LCD_RESET(); //LCD ??λ
	
	//************* ST7586S?????**********//		
	LCD_WR_REG(0x11); //????????
	LCD_WR_REG(0xC0); // ???? VOP
	LCD_WR_DATA(0xf1); // ???? VOP ?????? 8 λ????? 9 λ??,???????? 0.03667V
	LCD_WR_DATA(0x00); // ???? VOP ?????? 9 λ?????????λ
	LCD_WR_REG(0xC3); // ???? BIAS
	LCD_WR_DATA(0x02); // 00??BIAS = 1/14 02 = 1/12
	LCD_WR_REG(0xC4); // ???????????
	LCD_WR_DATA(0x07); // 07??8 ???
	LCD_WR_REG(0xD0); // ????????·
	LCD_WR_DATA(0x1D); // ????????·
	LCD_WR_REG(0xB5); // N-Line = 13
	LCD_WR_DATA(0x00); // 8d
	LCD_WR_REG(0x38); // 0x38?????????????? 0x39: ????????????
	LCD_WR_REG(0x3A); // ???? DDRAM ???????????4 ??????16 ??????
	LCD_WR_DATA(0x02); // 0x03:16 ??????0x02:4 ???????????

	LCD_WR_REG(0x36); // ??????????
	LCD_WR_DATA(0x00); // ??????????:MX=1,MY=1: ?????????????????????
	LCD_WR_REG(0xB0); // Duty ????
	LCD_WR_DATA(0x9f); // Duty ????:1/160
	LCD_WR_REG(0x20); // ?????????OFF
	LCD_WR_REG(0xf1); //???????????仯?????
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_REG(0xb1);  // ????????????
	LCD_WR_DATA(0x00); // ??????????????? COM0 ???
	LCD_WR_REG(0x29);  // ???????DISPLAY ON
	
	LCD_LED_ON;     //????????	 
	LCD_Clear();   //????????
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


void LCD_Clear(void)
{
	int i,j;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	
	for(i=0;i<160;i++)
	{
		for(j=0;j<20;j++)
		{
			transfer_mono_data_18pixel(0x00,0x00,0x00); //????????? 8 ????????? 8*3=24 ??????
		}
	}
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
	HAL_Delay(100);	
	LCD_RST_SET;
	HAL_Delay(50);
}

/*****************************************************************************
 * @name       :uint8_t SPI_WriteByte(SPI_HandleTypeDef *hspi, uint8_t Byte, uint8_t cmd)
 * @date       :2025-08-25
 * @function   :Write a byte of data using STM32's hardware SPI (HAL library)
 * @parameters :hspi: SPI handle structure pointer (e.g. &hspi1, &hspi2)
                Byte: Data to be written
                cmd: 0-write command, 1-write data
 * @retvalue   :Data received by the bus
******************************************************************************/
uint8_t SPI_WriteByte(SPI_HandleTypeDef *hspi, uint8_t Byte, uint8_t cmd)
{
    uint16_t tx_data;  // 16位发送数据缓冲区
    uint16_t rx_data;  // 16位接收数据缓冲区
    
    // 构建16位发送数据（与原代码位操作逻辑一致）
    // cmd左移15位，Byte左移7位，组合成16位数据
    tx_data = ((cmd << 15) | (Byte << 7));
    
    // 使用HAL库函数进行SPI发送和接收
    // 参数说明：
    // hspi: SPI句柄
    // (uint8_t*)&tx_data: 发送数据缓冲区地址
    // (uint8_t*)&rx_data: 接收数据缓冲区地址
    // 1: 数据长度（1个16位数据）
    // HAL_MAX_DELAY: 超时时间（无限等待）
    HAL_SPI_TransmitReceive(hspi, (uint8_t*)&tx_data, (uint8_t*)&rx_data, 1, 50);
    
    // 返回接收的16位数据（与原代码行为一致）
    return rx_data;
}

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

void transfer_mono_data_3pixel(u8 mono_data)
{
	u8 gray_data=0;
	
	if(mono_data&0x80)
	{
		gray_data=0xe0; //?????? 11100000??????? D7??D6??D5 ???
	}
	else
	{
		gray_data=0;
	}
	mono_data<<=1;
	if(mono_data&0x80)
	{
		gray_data+=0x1c; //?????? 00011100??????? D4??D3??D2 ???
	}
	else;
	mono_data<<=1;
	if(mono_data&0x80)
	{
		gray_data+=0x03; //?????? 00000011??????? D1??D0 ???
	}
	else;
	LCD_WR_DATA(gray_data); //display 3 dots (seg_N,seg_N+1,seg_N+2)
}	

void transfer_mono_data_6pixel(u8 dat1)
{
	transfer_mono_data_3pixel(dat1);
	transfer_mono_data_3pixel(dat1<<3);
}

void transfer_mono_data_9pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_6pixel(dat1);
	transfer_mono_data_3pixel((dat1<<6)|(dat2>>2));
}

void transfer_mono_data_12pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_9pixel(dat1,dat2);
	transfer_mono_data_3pixel(dat2<<1);
}

void transfer_mono_data_15pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_12pixel(dat1,dat2);
	transfer_mono_data_3pixel(dat2<<4);
}

void transfer_mono_data_18pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_15pixel(dat1,dat2);
	transfer_mono_data_3pixel((dat2<<7)|(dat3>>1));
}

void transfer_mono_data_21pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_18pixel(dat1,dat2,dat3);
	transfer_mono_data_3pixel(dat3<<2);
}

void transfer_mono_data_24pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_21pixel(dat1,dat2,dat3);
	transfer_mono_data_3pixel(dat3<<5);
}

void disp_240x160(u8 *dp)
{
	u16 i,j;
	u8 dat1,dat2,dat3;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	for(i=0;i<160;i++)
	{
		for(j=0;j<10;j++)//??? 10 ?Σ??????? 24 ???????? 240 ??????
		{
			dat1=*dp;dp++;
			dat2=*dp;dp++;
			dat3=*dp;dp++;
			transfer_mono_data_24pixel(dat1,dat2,dat3); //????????? 8 ????????? 8*3=24 ??????
		}
	}
}