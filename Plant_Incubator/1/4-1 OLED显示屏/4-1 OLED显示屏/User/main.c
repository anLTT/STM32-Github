#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "SCD30.h"
#include "MyI2C.h"
#include "sys.h"
//#include "usart.h"
 
int main(void)
{
	SystemInit();
	
	uint8_t i=0;
//	uint16_t res;
//	uint8_t crc_statu;
//	uint8_t write_statu;
	uint8_t read_statu;
	uint16_t CO2 = 0; //ppm
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	uart_init(9600);	 //串口初始化为115200
	OLED_Init();
	IIC_Init();
	OLED_ShowString(1,1,"Loading...");
	for(i=0;i<3;i++)//预热计时十秒
	{
		OLED_ShowNum(1,12,i,2);
		Delay_s(1);
	}
	// 设置测量间隔
//		write_statu = SCD30_Write_with_arguments(0x4600,0x000A);
//		OLED_ShowNum(2,1,write_statu,1);
//	
		//显示间隔
//		crc_statu=SCD30_ReadRegister(0x4600,&res,10);
//		OLED_ShowNum(3,1,crc_statu,1);
//		OLED_ShowHexNum(3,3,res,4);
 
		SCD30_stopPeriodicMeasurement(1);
  	//SCD30_reInit(20);
  	SCD30_StartPeriodicMeasurement();
	
	while (1)
	{
		//读取数据准备状态
//		read_statu = SCD30_getDataReadyStatus();
//		OLED_ShowNum(4,1,read_statu,1);
		
		read_statu = SCD30_readMeasurement(&CO2);
		if(read_statu)
		{
			OLED_ShowString(4,1,"CO2:");
			OLED_ShowNum(4,5,CO2,4);
//			printf("  %d ppm\r\n",CO2);
			i=0;
		}
		OLED_ShowNum(1,12,i++,2);
		Delay_ms(100);
 
	}
}

//#include "stm32f10x.h"                  // Device header
//#include "Delay.h"
//#include "OLED.h"

//int main(void)
//{
//	/*模块初始化*/
//	OLED_Init();		//OLED初始化
//	
//	/*OLED显示*/
//	OLED_ShowChar(1, 1, 'A');				//1行1列显示字符A
//	
//	OLED_ShowString(1, 3, "HelloWorld!");	//1行3列显示字符串HelloWorld!
//	
//	OLED_ShowNum(2, 1, 12345, 5);			//2行1列显示十进制数字12345，长度为5
//	
//	OLED_ShowSignedNum(2, 7, -66, 2);		//2行7列显示有符号十进制数字-66，长度为2
//	
//	OLED_ShowHexNum(3, 1, 0xAA55, 4);		//3行1列显示十六进制数字0xA5A5，长度为4
//	
//	OLED_ShowBinNum(4, 1, 0xAA55, 16);		//4行1列显示二进制数字0xA5A5，长度为16
//											//C语言无法直接写出二进制数字，故需要用十六进制表示
//	
//	while (1)
//	{
//		
//	}
//}
