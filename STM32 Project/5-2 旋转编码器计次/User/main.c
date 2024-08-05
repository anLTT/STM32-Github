#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Encoder.h"


int main()
{
	int Num = 0;
	OLED_Init();
	Encoder_Init();
	OLED_ShowString(1,1,"Num:");
	
	while(1)
	{
		Num += Encoder_Get();//因为这个函数范围有正负
		OLED_ShowSignedNum(1,5,Num,5);//OLED显示
	}

}



