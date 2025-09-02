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
#include "lcd.h"
#include "stdlib.h"
//#include "delay.h"	 
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
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{  
								 
 	LCD_RESET(); //LCD ��λ
	
	//************* ST7586S��ʼ��**********//		
	LCD_WR_REG(0x11); //�˳�˯��ģʽ
	LCD_WR_REG(0xC0); // ���� VOP
	LCD_WR_DATA(0xf1); // ���� VOP ��ֵ�ĵ� 8 λ���ܹ� 9 λ��,ÿ��һ���� 0.03667V
	LCD_WR_DATA(0x00); // ���� VOP ��ֵ�ĵ� 9 λ��Ҳ�����һλ
	LCD_WR_REG(0xC3); // ���� BIAS
	LCD_WR_DATA(0x02); // 00��BIAS = 1/14 02 = 1/12
	LCD_WR_REG(0xC4); // ������ѹ����
	LCD_WR_DATA(0x07); // 07��8 ��ѹ
	LCD_WR_REG(0xD0); // ����ģ���·
	LCD_WR_DATA(0x1D); // ����ģ���·
	LCD_WR_REG(0xB5); // N-Line = 13
	LCD_WR_DATA(0x00); // 8d
	LCD_WR_REG(0x38); // 0x38������Ϊ�Ҷ�ģʽ�� 0x39: ����Ϊ�ڰ�ģʽ��
	LCD_WR_REG(0x3A); // ���� DDRAM �ӿڣ���ɫģʽ��4 �Ҷȼ���16 �Ҷȼ���
	LCD_WR_DATA(0x02); // 0x03:16 �Ҷȼ���0x02:4 �Ҷȼ���ɫģʽ��

	LCD_WR_REG(0x36); // ɨ��˳������
	LCD_WR_DATA(0x00); // ɨ��˳������:MX=1,MY=1: �����ң����ϵ��µ�ɨ��˳��
	LCD_WR_REG(0xB0); // Duty ����
	LCD_WR_DATA(0x9f); // Duty ����:1/160
	LCD_WR_REG(0x20); // �������ã�OFF
	LCD_WR_REG(0xf1); //�¶Ȳ������¶ȱ仯�ı�֡Ƶ
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_DATA(0x15);
	LCD_WR_REG(0xb1);  // ɨ����ʼ������
	LCD_WR_DATA(0x00); // ɨ����ʼ�����ã��� COM0 ��ʼ
	LCD_WR_REG(0x29);  // ����ʾ��DISPLAY ON
	
	LCD_LED=1;     //��������	 
	LCD_Clear();   //��ȫ����ɫ
}
 

/*д LCD ���е�ַ��X Ϊ��ʼ���е�ַ��Y Ϊ��ʼ���е�ַ��x_total,y_total �ֱ�Ϊ�е�ַ���е�ַ����㵽�յ�Ĳ�ֵ */
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
	
	
//����ͬһ����ַ�� 3 ������ĺڰ׵����ݣ����� SEG0��SEG1��SEG2���� 3 ��������ͬһ���е�ַ���޷��ֿ���
//������ʱ����� 1 �е������ǡ�D7 D6 D5 D4 D3 D2 D1 D0�� �еĸ� 3 λ---D7 D6 D5, �� 2 ������ 3 λ---D4 D3 D2,�� 3 ���ǵ���λ---D1 D0��
void transfer_mono_data_3pixel(u8 mono_data)
{
	u8 gray_data=0;
	
	if(mono_data&0x80)
	{
		gray_data=0xe0; //������ 11100000�����Ǹ� D7��D6��D5 ��ֵ
	}
	else
	{
		gray_data=0;
	}
	mono_data<<=1;
	if(mono_data&0x80)
	{
		gray_data+=0x1c; //������ 00011100�����Ǹ� D4��D3��D2 ��ֵ
	}
	else;
	mono_data<<=1;
	if(mono_data&0x80)
	{
		gray_data+=0x03; //������ 00000011�����Ǹ� D1��D0 ��ֵ
	}
	else;
	LCD_WR_DATA(gray_data); //display 3 dots (seg_N,seg_N+1,seg_N+2)
}	

//��ʾ 6 ������
void transfer_mono_data_6pixel(u8 dat1)
{
	transfer_mono_data_3pixel(dat1);
	transfer_mono_data_3pixel(dat1<<3);
}
//��ʾ 8 ������
void transfer_mono_data_8pixel(u8 dat1)
{
	transfer_mono_data_3pixel(dat1);//���� dat1 �� D7\D6\D5 �� 3 λ����Ӧ 3 ������(�� 1��2��3 ��������ʾ�������е�ַ���Զ�+1 ��
	transfer_mono_data_3pixel(dat1<<3);//���� dat1 �� D4\D3\D2 �� 3 λ����Ӧ 3 ������(�� 4��5��6 ��������ʾ�������е�ַ���Զ�+1 ��
	transfer_mono_data_3pixel(dat1<<6);//���� dat1 �� D1\D0 �� 2 λ����Ӧ 3 ������(�� 7��8��9 ��������ʾ����
	//���Һ������ IC ��ÿ���е�ַ�� 3 �������޷��ֿ������Ե� 7��8 ����������۵��� 9 �����󣬽����ÿ����ʾ 9 ������ֻ������ 9 ������Ჹ��0��
	//����� 9 ������������ʾ���ݣ��ͻᱻ��������
}
//��ʾ 9 ������
void transfer_mono_data_9pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_6pixel(dat1);//����ʾ 6 ������
	transfer_mono_data_3pixel((dat1<<6)|(dat2>>2)); //��ʾ dat1 �� D1��D0 �� dat2 �� D7 λ����Ӧ 3 ������(�� 7��7��9 ��������ʾ�������е�ַ���Զ�+1 ��
}
//��ʾ 12 ������
void transfer_mono_data_12pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_9pixel(dat1,dat2);//����ʾ 9 ������
	transfer_mono_data_3pixel(dat2<<1); //���� dat2 �� D6\D5\D4 �� 3 λ����Ӧ�� 10��11��12 �����������ʾ�������е�ַ���Զ�+1��
}
//��ʾ 15 ������
void transfer_mono_data_15pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_12pixel(dat1,dat2); //����ʾ 12 ������
	transfer_mono_data_3pixel(dat2<<4); //���� dat2 �� D3\D2\D1 �� 3 λ����Ӧ�� 13��14��15 ���������ʾ�������е�ַ���Զ�+1��
}
//��ʾ 16 ������
void transfer_mono_data_16pixel(u8 dat1,u8 dat2)
{
	transfer_mono_data_15pixel(dat1,dat2); //����ʾ 15 ������
	transfer_mono_data_3pixel(dat2<<7); //��ʾ�� 16 ������,��Ӧ dat2 �� D0 λ��
//���Һ������ IC ��ÿ���е�ַ�� 3 �������޷��ֿ������Ե� 16 ����������۵��� 17��18 �����󣬽����ÿ����ʾ 18 ������ ֻ������17��18 ������Ჹ��0��
//����� 17��18 ������������ʾ���ݣ��ͻᱻ��������
}
//��ʾ 18 ������
void transfer_mono_data_18pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_15pixel(dat1,dat2); //����ʾ 15 ������
	transfer_mono_data_3pixel((dat2<<7)|(dat3>>1)); //���� dat2 �� D0 �� dat3 �� D7��D6 �� 3 λ����Ӧ�� 16��17��18 ���������ʾ�������е�ַ���Զ�+1 ��
}
//��ʾ 21 ������
void transfer_mono_data_21pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_18pixel(dat1,dat2,dat3); //����ʾ 18 ������
	transfer_mono_data_3pixel(dat3<<2); //���� dat3 �� D5��D4��D3 �� 3 λ����Ӧ�� 19��20��21 ���������ʾ�������е�ַ���Զ�+1��
}
//��ʾ 24 �����󡣷���һ��
void transfer_mono_data_24pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_mono_data_21pixel(dat1,dat2,dat3); //����ʾ 21 ������
	transfer_mono_data_3pixel(dat3<<5); //���� dat3 �� D2��D1��D0 �� 3 λ����Ӧ�� 22��23��24 ���������ʾ�������е�ַ���Զ�+1��
}
//��ʾ 27 ������
void transfer_mono_data_27pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //����ʾ 24 ������
	transfer_mono_data_3pixel(dat4); //���� dat4 �� D7��D6��D5 �� 3 λ����Ӧ�� 25�� 26��27 ���������ʾ�������е�ַ���Զ�+1��
}
//��ʾ 30 ������
void transfer_mono_data_30pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //����ʾ 24 ������
	transfer_mono_data_6pixel(dat4); //����ʾ 6 ������24+6=30
}
//��ʾ 32 ������
void transfer_mono_data_32pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //����ʾ 24 ������
	transfer_mono_data_8pixel(dat4); //����ʾ 8 ������24+8=32
//���Һ������ IC ��ÿ���е�ַ�� 3 �������޷��ֿ������Ե� 31��32 ����������۵��� 33 �����󣬽����ÿ����ʾ 33 ������ ֻ������33 ������Ჹ��0��
//����� 33 ������������ʾ���ݣ��ͻᱻ��������
}
//��ʾ 33 ������
void transfer_mono_data_33pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4,u8 dat5)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //����ʾ 24 ������
	transfer_mono_data_9pixel(dat4,dat5); //����ʾ 9 ������
}
//��ʾ 48 ������
void transfer_mono_data_48pixel(u8 dat1,u8 dat2,u8 dat3,u8 dat4,u8 dat5,u8 dat6)
{
	transfer_mono_data_24pixel(dat1,dat2,dat3); //����ʾ 24 ������
	transfer_mono_data_24pixel(dat4,dat5,dat6); //����ʾ 24 ������
}	
	

//����ͬһ����ַ�� 3 ������� 4 �Ҷȼ������ݣ����� SEG0�� SEG1��SEG2,�� 3 ��������ͬһ���е�ַ�� �޷��ֿ�
//�ͻҶ�����(gray_data)ʱ��SEG0 ��Ӧ�� 3 λ��D7�� D6��D5��,SEG1 ��Ӧ�� 3 λ��D4��D3��D2),SEG2 ��Ӧ����λ��D1��D0����
void transfer_gray_data_3pixel(u8 dat1)
{
	u8 gray_data;
	
	gray_data=dat1&0xc0;; //�� gray_data �� D7��D6 ��ֵ(=dat1 �� D7��D6)
	if((dat1&0xc0)==0xc0)
	{
		gray_data|=0x20; //�� gray_data �� D5 ��ֵ���� dat1 �� D7��D6 ���� 1 ��ʱ��gray_data �� D5=1,�� dat1 �� D7\D6 ������ 1 ��ʱ��gray_data �� D5=0
	}
	gray_data|=((dat1>>1)&0x18); //�� gray_data �� D4��D3 ��ֵ��=dat1 �� D5��D4)
	if((dat1&0x30)==0x30)
	{
		gray_data|=0x04; //�� gray_data �� D2 ��ֵ���� dat1 �� D5��D4 ���� 1 ��ʱ��gray_data �� D2=1,�� dat1 �� D7��D6 ������ 1 ��ʱ��gray_data �� D2=0
	}
	gray_data|=((dat1>>2)&0x03); //�� gray_data �� D1��D0 ��ֵ(=dat1 �� D3��D2)
	LCD_WR_DATA(gray_data); //���� 1 ���ֽڻҶ����ݸ�Һ������ IC,��Ӧ�� 3 ���������ʾ(seg_N,seg_N+1,seg_N+2)
}


//����ͬһ����ַ�� 12 ������� 4 �Ҷȵ����ݣ����� SEG0��SEG1��SEG2......SEG9��SEG10��SEG11���� 12 �������� 4 ���е�ַ��
//ÿ 2 λ���ݶ�Ӧһ������12 �������ã�2*12=24 λ���� 3 ���ֽ�:dat1��dat2��dat3
void transfer_gray_data_12pixel(u8 dat1,u8 dat2,u8 dat3)
{
	transfer_gray_data_3pixel(dat1); //��ʾ 3 ������(seg_N,seg_N+1,SEG_N+2)
	transfer_gray_data_3pixel((dat1<<6)|(dat2>>2)); //��ʾ 3 ������(seg_N+3,seg_N+4,SEG_N+5)
	transfer_gray_data_3pixel((dat2<<4)|(dat3>>4)); //��ʾ 3 ������(seg_N+6,seg_N+7,SEG_N+8)
	transfer_gray_data_3pixel(dat3<<2); //��ʾ 3 ������(seg_N+9,seg_N+10,SEG_N+11)
}


/*����*/
void LCD_Clear(void)
{
	int i,j;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	
	for(i=0;i<160;i++)
	{
		for(j=0;j<20;j++)
		{
			transfer_mono_data_18pixel(0x00,0x00,0x00); //ÿ���ֽ���ʾ 8 ��������ʾ 8*3=24 ������
		}
	}
}


//��ʾ 240*160 �����ͼ��
void disp_240x160(u8 *dp)
{
	u16 i,j;
	u8 dat1,dat2,dat3;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	for(i=0;i<160;i++)
	{
		for(j=0;j<10;j++)//ѭ�� 10 �Σ�ÿ����ʾ 24 �����󣬺ϼ� 240 ������
		{
			dat1=*dp;dp++;
			dat2=*dp;dp++;
			dat3=*dp;dp++;
			transfer_mono_data_24pixel(dat1,dat2,dat3); //ÿ���ֽ���ʾ 8 ��������ʾ 8*3=24 ������
		}
	}
}


//===��ʾ���Ի��棺����ȫ��ʾ��������ʾ��������ʾ��ѩ����ʾ=====
void test_display(u8 dat1,u8 dat2,u8 dat3)
{
	u16 i,j;
	
	lcd_address(0,0,240,160);
	LCD_WriteRAM_Prepare();
	for(i=0;i<160;i++)
	{
		for(j=0;j<10;j++)//ѭ�� 10 �Σ�ÿ����ʾ 24 �����󣬺ϼ� 240 ������
		{
			transfer_mono_data_24pixel(dat1,dat2,dat3); //ÿ���ֽ���ʾ 8 ��������ʾ 8*3=24 ������
		}
	}
}


//��ʾ 240*160 ����� 4 �Ҷȼ�ͼ��
void disp_4gray_240x160(u8 *dp)
{
	u8 i,j;
	u8 dat1,dat2,dat3;
	
	lcd_address(0,0,240,160); //
	LCD_WriteRAM_Prepare();
	
	for(i=0;i<160;i++)
	{
		for(j=0;j<20;j++)//ѭ�� 20 �Σ�ÿ����ʾ 12 �����󣬺ϼ� 20*12=240 ������
		{
			dat1=*dp;dp++;
			dat2=*dp;dp++;
			dat3=*dp;dp++;
			transfer_gray_data_12pixel(dat1,dat2,dat3); //ÿ���ֽ���ʾ 4 �����󣬹���ʾ 4*3=12 ������
		}
	}
}


#define SPI_I2S_FLAG_RXNE               ((uint16_t)0x0001)
#define SPI_I2S_FLAG_TXE                ((uint16_t)0x0002)
#define I2S_FLAG_CHSIDE                 ((uint16_t)0x0004)
#define I2S_FLAG_UDR                    ((uint16_t)0x0008)
//#define SPI_FLAG_CRCERR                 ((uint16_t)0x0010)
//#define SPI_FLAG_MODF                   ((uint16_t)0x0020)
#define SPI_I2S_FLAG_OVR                ((uint16_t)0x0040)
#define SPI_I2S_FLAG_BSY                ((uint16_t)0x0080)
#define IS_SPI_I2S_CLEAR_FLAG(FLAG) (((FLAG) == SPI_FLAG_CRCERR))
#define IS_SPI_I2S_GET_FLAG(FLAG) (((FLAG) == SPI_I2S_FLAG_BSY) || ((FLAG) == SPI_I2S_FLAG_OVR) || \
                                   ((FLAG) == SPI_FLAG_MODF) || ((FLAG) == SPI_FLAG_CRCERR) || \
                                   ((FLAG) == I2S_FLAG_UDR) || ((FLAG) == I2S_FLAG_CHSIDE) || \
                                   ((FLAG) == SPI_I2S_FLAG_TXE) || ((FLAG) == SPI_I2S_FLAG_RXNE))

/*****************************************************************************
 * @name       :u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte,u8 cmd)
 * @date       :2018-08-27 
 * @function   :Write a byte of data using STM32's hardware SPI
 * @parameters :SPIx: SPI type,x for 1,2,3
                Byte:Data to be written
								cmd:0-write command
								    1-write data
 * @retvalue   :Data received by the bus
******************************************************************************/
u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte,u8 cmd)
{
//	while((SPIx->SR&SPI_I2S_FLAG_TXE)==RESET);		//�ȴ���������	  
	SPIx->DR=((cmd<<15)|(Byte<<7));	 	//��������byte   
//	while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);//�ȴ�����������byte  
	return SPIx->DR;          	     //�����յ�������			
} 

/*****************************************************************************
// * @name       :void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
// * @date       :2018-08-09 
// * @function   :Set hardware SPI Speed
// * @parameters :SPIx: SPI type,x for 1,2,3
//                SpeedSet:0-high speed
//												 1-low speed
// * @retvalue   :None
//******************************************************************************/
//void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
//{
//	SPIx->CR1&=0XFFC7;
//	if(SpeedSet==1)//����
//	{
//		SPIx->CR1|=SPI_BaudRatePrescaler_2;//Fsck=Fpclk/2	
//	}
//	else//����
//	{
//		SPIx->CR1|=SPI_BaudRatePrescaler_32; //Fsck=Fpclk/32
//	}
//	SPIx->CR1|=1<<6; //SPI�豸ʹ��
//} 


