#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"

uint16_t Num = 0;

int main(void)
{	
	//我们此时还是定时中断
	//不过用的是外部定时器
	OLED_Init();
	Timer_Init();
	OLED_ShowString(1,1,"Num:");
	OLED_ShowString(2,1,"CNT:");
	while(1)
	{
		OLED_ShowNum(1,5,Num,5);
		OLED_ShowNum(2,5,Timer_GetCounter(),5);
		
	}
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{
		Num++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);	
	}
}



