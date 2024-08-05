#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"


int main()
{

	OLED_Init();//此工程用不到OLED
	//在Hardware里加PWM模块
	PWM_Init();
	//由于CCR是控制占空比,所以想实现呼吸灯,就是不断改变CCR的值
	
	uint8_t i = 0;//先定义变量
	
	
	while(1)
	{
		//主循环中不断调用	PWM_SetCompare1();更改CCR的值就能实现呼吸灯的效果了
		for(i = 0;i<=100;i++)
			{
				PWM_SetCompare1(i);//是设置CCR的值,并不是直接设置占空比.占空比是CCR和ARR+1共同设置的
				Delay_ms(10);
			}
		for(i = 0;i<=100;i++)
			{
				PWM_SetCompare1(100 - i);
				Delay_ms(10);
			}
	}

}



