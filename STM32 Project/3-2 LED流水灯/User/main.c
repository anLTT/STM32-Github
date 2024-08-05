#include "stm32f10x.h"                  // Device header
#include "Delay.h"

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启GPIOA时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置成推挽模式
	//我们用到的是A0~7端口
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |...;//一次性初始化多个端口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;//All把16个端口全部配置成推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	while(1)
	{
		GPIO_Write(GPIOA, ~0x0001); //0000 0000 0000 0001 
		//C中不能直接二进制,所以使用十六进制
		//对应PA0到PA15共16个端口,最右边是PA0,最左边是PA15
		//因为低电平才点亮,所以用~
		Delay_ms(500);
		GPIO_Write(GPIOA, ~0x0002); //0000 0000 0000 0010
		Delay_ms(500);
		GPIO_Write(GPIOA, ~0x0004); //0000 0000 0000 0100
		Delay_ms(500);
		GPIO_Write(GPIOA, ~0x0008); //0000 0000 0000 1000
		Delay_ms(500);
		GPIO_Write(GPIOA, ~0x0010); //0000 0000 0001 0000
		Delay_ms(500);
		GPIO_Write(GPIOA, ~0x0020); //0000 0000 0010 0000
		Delay_ms(500);
		GPIO_Write(GPIOA, ~0x0040); //0000 0000 0100 0000
		Delay_ms(500);
		GPIO_Write(GPIOA, ~0x0080); //0000 0000 1000 0000
		Delay_ms(500);
	}

}



