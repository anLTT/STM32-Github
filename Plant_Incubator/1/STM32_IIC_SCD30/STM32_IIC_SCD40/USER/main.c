#include "sys.h"
#include "delay.h"
#include "My_SCD4x-CO2.h"


uint16_t CO2 = 0; //ppm
float temperature = 0; //摄氏度
float humidity = 0; //湿度 百分比
uint8_t error = 0;


 int main(void)
 {	
	delay_init();	    //延时函数初始化	  
	IIC_Init();
	//停止周期测量
	SCD4x_stopPeriodicMeasurement(600);
	//SCD4x软件重启
	SCD4x_reInit(20);
	//开始周期测量
	SCD4x_StartPeriodicMeasurement(); 

	while(1)
	{
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		SCD4x_readMeasurement(&CO2, &temperature, &humidity, &error); //转换周期5秒
	}
 }


