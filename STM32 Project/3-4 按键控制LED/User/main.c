#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"                       //包含LED模块的头文件
#include "Key.h"
int main()//main()中可不写void,但自定义函数一定要写
{
	//对于驱动代码,全写主函数里杂,所以会单独封装,放到其他.c和.h文件里
	//所以新建Hardware文件
	LED_Init();//包含后就可以用了,刚开始有警告,因为还没更新,不知道有这个东西,可以编译一下
	//LOAD后两个灯都亮说明端口配置和模块化编程没问题,因为GPIO配置好后默认是低电平,所以即亮
	
	Key_Init();//主循环前初始化按键
	
	uint8_t KeyNum = 0;//定义存按键值的变量
	
	while(1)
	{
		//LED1_ON();
		//LED2_OFF();
		//Delay_ms(500);
		//LED1_OFF();
		//LED2_ON();
		//Delay_ms(500);
		
		//KeyNum = Key_GetNum();//不断读取键码值
		//if(KeyNum == 1)
		//{
		//	LED1_ON();
		//}
		//if(KeyNum == 2)
		//{
		//	LED1_OFF();
		//}
		//为了实现按两下亮并灭 要用GPIO_ReadOutputDataBit,在LED.c中写
		
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			LED1_Turn();//哇哦,Turn中把ON和OFF都包含了,还能切换
		}
		if(KeyNum == 2)
		{
			LED2_Turn();
		}
	}

}



