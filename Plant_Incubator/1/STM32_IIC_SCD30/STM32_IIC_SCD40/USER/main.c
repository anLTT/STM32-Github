#include "sys.h"
#include "delay.h"
#include "My_SCD4x-CO2.h"


uint16_t CO2 = 0; //ppm
float temperature = 0; //���϶�
float humidity = 0; //ʪ�� �ٷֱ�
uint8_t error = 0;


 int main(void)
 {	
	delay_init();	    //��ʱ������ʼ��	  
	IIC_Init();
	//ֹͣ���ڲ���
	SCD4x_stopPeriodicMeasurement(600);
	//SCD4x�������
	SCD4x_reInit(20);
	//��ʼ���ڲ���
	SCD4x_StartPeriodicMeasurement(); 

	while(1)
	{
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		SCD4x_readMeasurement(&CO2, &temperature, &humidity, &error); //ת������5��
	}
 }


