#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Motor.h"//不调用PWM.h了耶
#include "Key.h"

//定义两个变量
uint8_t KeyNum = 0;//按键键码
int8_t Speed = 0;//有符号的速度变量

int main()
{
	OLED_Init();
	Motor_Init();//初始化电机
	Key_Init();//初始化按键
	
	
	OLED_ShowString(1,1,"Speed:");
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			Speed += 20;
			if(Speed > 100)
			{
				Speed = -100;
			}//所以Speed从-100到100变化
		}
		Motor_SetSpeed(Speed);//把SetSpeed挪到这里,并把Speed放进去
		
		//最后OLED显示速度
		OLED_ShowNum(1,7,Speed,3);
	}

}



