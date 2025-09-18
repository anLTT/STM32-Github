#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "delay_1.h"
#include "Key.h"
#include "Timer.h"
#include "lcd.h"

float Temp = 60.0;
int8_t Humi = 60;
int16_t CO2 = 1056;


int main(void)
{
	Key_Init();
	Timer_Init();
	

	SystemInit();//?????RCC 72MHZ
	delay_init(72);	     
	LCD_Init();	   


	disp_240x160(jiemian);
	delay_ms(1000);

	
	while (1)
	{

		if (Key_Check(KEY_1, KEY_SINGLE) || Key_Check(KEY_1, KEY_REPEAT))
		{
			//Num1 ++;
			Temp += 0.1;
			CO2 += 1;
			Humi += 1;
		}
		if (Key_Check(KEY_2, KEY_SINGLE) || Key_Check(KEY_2, KEY_REPEAT))
		{
			Temp -= 0.1;
			CO2 -= 1;
			Humi -= 1;
		}

	 Temp = clamp_float(Temp, 0.0f, 70.0f);
	
	 LCD_ShowFloat_70_Large(15, 10, Temp);
	 			//delay_ms(200);
	
	 CO2 = clamp_int16(CO2, 0, 3000);

	LCD_ShowInt_3000_Large(15,95,CO2);
					//delay_ms(200);
	Humi = clamp_int8(Humi, 0, 100);

	LCD_ShowInt_3000_Large(15,53,Humi);
					delay_ms(200);


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


//	
//	while (1)
//	{
////		LCD_ShowChar(0, 0, ' ');//不用这个.自己写的时候会直接将数据进行覆盖
////		LCD_ShowChar(0, 0, num + '0');
////		delay_ms(1000);
////		num++;
////		if(num > 9) num = 0;
////		LCD_ShowChar(0, 0, ' ');
////		LCD_ShowChar_Large(0, 0, num + '0');
////		delay_ms(200);
//		
//		/*示例1*/
////		if (Key_Check(KEY_1, KEY_HOLD))
////		{
////			Num1 = 1;
////		}
////		else
////		{
////			Num1 = 0;
////		}
////		if (Key_Check(KEY_2, KEY_HOLD))
////		{
////			Num2 = 1;
////		}
////		else
////		{
////			Num2 = 0;
////		}
//		
//		/*示例2*/
////		if (Key_Check(KEY_1, KEY_DOWN))
////		{
////			Num1 ++;
////		}
////		if (Key_Check(KEY_2, KEY_UP))
////		{
////			Num2 ++;
////		}
//		
//		/*示例3*/
////		if (Key_Check(KEY_1, KEY_SINGLE))
////		{
////			Num1 ++;
////		}
////		if (Key_Check(KEY_1, KEY_DOUBLE))
////		{
////			Num1 += 100;
////		}
////		if (Key_Check(KEY_2, KEY_SINGLE))
////		{
////			Num1 --;
////		}
////		if (Key_Check(KEY_2, KEY_DOUBLE))
////		{
////			Num1 -= 100;
////		}
////		if (Key_Check(KEY_1, KEY_LONG) || Key_Check(KEY_2, KEY_LONG))
////		{
////			Num1 = 0;
////		}
//		
//		/*示例4*/
//		if (Key_Check(KEY_1, KEY_SINGLE) || Key_Check(KEY_1, KEY_REPEAT))
//		{
//			//Num1 ++;
//			Num1 += 0.1;
//			CO2 += 1;
//			Humi += 1;
//		}
//		if (Key_Check(KEY_2, KEY_SINGLE) || Key_Check(KEY_2, KEY_REPEAT))
//		{
//			Num1 -= 0.1;
//			CO2 -= 1;
//			Humi -= 1;
//		}
////		if (Key_Check(KEY_3, KEY_SINGLE))
////		{
////			Num1 = 0;
////		}
////		if (Key_Check(KEY_3, KEY_LONG))
////		{
////			Num1 = 9999;
////		}
//		
//		/*示例5*/
////		uint8_t K1_UP = Key_Check(KEY_1, KEY_UP);
////		uint8_t K2_UP = Key_Check(KEY_2, KEY_UP);
////		
////		if (K1_UP && Key_Check(KEY_3, KEY_HOLD))
////		{
////			Num1 ++;
////		}
////		if (K2_UP && Key_Check(KEY_3, KEY_HOLD))
////		{
////			Num1 --;
////		}
////		if (K1_UP && Key_Check(KEY_4, KEY_HOLD))
////		{
////			Num2 ++;
////		}
////		if (K2_UP && Key_Check(KEY_4, KEY_HOLD))
////		{
////			Num2 --;
////		}
//		
//				//LCD_ShowChar_Large(0, 0, Num1 + '0');
////				LCD_ShowFloat_70(0,0,Num1);

//	 Num1 = clamp_float(Num1, 0.0f, 70.0f);
//	
//	 LCD_ShowFloat_70_Large(14, 0, Num1);
//	// 			delay_ms(1000);
//	// LCD_ShowFloat_70_Large(14, 50, Num1);
//	
//	// 			delay_ms(1000);	
//				
//	// 			LCD_ShowFloat_70_Large(14, 97, Num1);
//	
//	 			delay_ms(1000);
//	
//	 CO2 = clamp_int8(CO2, 0, 100);

//	LCD_ShowInt_3000_Large(14,40,CO2);
////					delay_ms(200);
//Humi = clamp_int16(Humi, 0, 3000);

//	LCD_ShowInt_3000_Large(14,80,Humi);
//					delay_ms(200);


//			}
//}


