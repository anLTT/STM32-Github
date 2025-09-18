
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
//#include "OLED.h"
#include "SCD30.h"
#include "MyI2C.h"
#include "sys.h"
#include "usart.h"
 
int main(void)
{
	uint8_t i=0;
//	uint16_t res;
//	uint8_t crc_statu;
//	uint8_t write_statu;
	uint8_t read_statu;
	uint16_t CO2 = 0; //ppm
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为115200
	//OLED_Init();
	IIC_Init();
	//OLED_ShowString(1,1,"Loading...");
	for(i=0;i<10;i++)//预热计时十秒
	{
		//OLED_ShowNum(1,12,i,2);
		delay_ms(1000);
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
			//OLED_ShowString(4,1,"CO2:");
			//OLED_ShowNum(4,5,CO2,4);
			//printf("  %d ppm\r\n",CO2);
			i=0;
		}
		//OLED_ShowNum(1,12,i++,2);
		delay_ms(100);
 
	}
}
