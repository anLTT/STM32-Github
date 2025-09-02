#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"

uint16_t Num1;
uint16_t Num2;

int main(void)
{
	OLED_Init();
	Key_Init();
	Timer_Init();
	
	OLED_ShowString(1, 1, "Num1:");
	OLED_ShowString(2, 1, "Num2:");
	
	while (1)
	{
		/*示例1*/
//		if (Key_Check(KEY_1, KEY_HOLD))
//		{
//			Num1 = 1;
//		}
//		else
//		{
//			Num1 = 0;
//		}
//		if (Key_Check(KEY_2, KEY_HOLD))
//		{
//			Num2 = 1;
//		}
//		else
//		{
//			Num2 = 0;
//		}
		
		/*示例2*/
//		if (Key_Check(KEY_1, KEY_DOWN))
//		{
//			Num1 ++;
//		}
//		if (Key_Check(KEY_2, KEY_UP))
//		{
//			Num2 ++;
//		}
		
		/*示例3*/
//		if (Key_Check(KEY_1, KEY_SINGLE))
//		{
//			Num1 ++;
//		}
//		if (Key_Check(KEY_1, KEY_DOUBLE))
//		{
//			Num1 += 100;
//		}
//		if (Key_Check(KEY_2, KEY_SINGLE))
//		{
//			Num1 --;
//		}
//		if (Key_Check(KEY_2, KEY_DOUBLE))
//		{
//			Num1 -= 100;
//		}
//		if (Key_Check(KEY_1, KEY_LONG) || Key_Check(KEY_2, KEY_LONG))
//		{
//			Num1 = 0;
//		}
		
		/*示例4*/
//		if (Key_Check(KEY_1, KEY_SINGLE) || Key_Check(KEY_1, KEY_REPEAT))
//		{
//			Num1 ++;
//		}
//		if (Key_Check(KEY_2, KEY_SINGLE) || Key_Check(KEY_2, KEY_REPEAT))
//		{
//			Num1 --;
//		}
//		if (Key_Check(KEY_3, KEY_SINGLE))
//		{
//			Num1 = 0;
//		}
//		if (Key_Check(KEY_3, KEY_LONG))
//		{
//			Num1 = 9999;
//		}
		
		/*示例5*/
//		uint8_t K1_UP = Key_Check(KEY_1, KEY_UP);
//		uint8_t K2_UP = Key_Check(KEY_2, KEY_UP);
//		
//		if (K1_UP && Key_Check(KEY_3, KEY_HOLD))
//		{
//			Num1 ++;
//		}
//		if (K2_UP && Key_Check(KEY_3, KEY_HOLD))
//		{
//			Num1 --;
//		}
//		if (K1_UP && Key_Check(KEY_4, KEY_HOLD))
//		{
//			Num2 ++;
//		}
//		if (K2_UP && Key_Check(KEY_4, KEY_HOLD))
//		{
//			Num2 --;
//		}
		
		OLED_ShowNum(1, 6, Num1, 5);
		OLED_ShowNum(2, 6, Num2, 5);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
