#include "stm32f10x.h"                  // Device header
#include "Delay.h"

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//开启GPIOB时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//端口模式 - 推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//12号端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	
	while(1)
	{
	
		//GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		//Delay_ms(500);
		//GPIO_SetBits(GPIOB,GPIO_Pin_12);
		//Delay_ms(500);
		//换个响的模式
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
		Delay_ms(700);
	}

}



