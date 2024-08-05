#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "PWM.h"
#include "IC.h"


int main()
{
	OLED_Init();
	PWM_Init();
	IC_Init();//初始化电路
	//显示频率
	OLED_ShowString(1,1,"Freq:00000Hz");
	
	PWM_SetPrescaler(720-1);
	PWM_SetCompare1(50);
	//目前ARR = 1000 - 1   所以频率1kHz,占空比50%
	//上两句将PWM输到PA0,PA0又由导线输出到PA6
	
	
	while(1)
	{//不断刷新显示频率
		OLED_ShowNum(1,6,IC_GetFreq(),5);		
	}

}



