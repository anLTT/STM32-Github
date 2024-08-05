#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Key.h"//加按键头文件

//定义两个变量
uint8_t KeyNum = 0;//按键键码
float Angle = 0;//角度变量


int main()
{

	OLED_Init();
	Servo_Init();
	Key_Init();//按键初始化

	OLED_ShowString(1,1,"Angle:");
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			Angle += 30;
			if (Angle > 180)
			{
				Angle = 0;
			}
		}
		Servo_SetAngle(Angle);
		OLED_ShowNum(1,7,Angle,3);
	}

}



