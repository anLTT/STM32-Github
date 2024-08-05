#include "stm32f10x.h"                  // Device header

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//配置端口模式
	GPIO_InitTypeDef    GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//设置端口高低电平进行点灯
	GPIO_SetBits(GPIOC, GPIO_Pin_13); //把指定端口(13号口)设置为高电平
//	GPIO_ResetBits(GPIOC, GPIO_Pin_13);//把PC13设置为低电平
	
	while(1)
	{

	}
	
}
