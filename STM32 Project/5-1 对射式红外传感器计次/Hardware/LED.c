#include "stm32f10x.h"                  // Device header
//第一行include一个stm32的头文件

//LED.c存放驱动程序的主体代码

//哦哦,先主函数
//屁咧,什么主函数,主函数只能有一个,是自定义的函数

void LED_Init(void)//初始化LED的函数
	
//不显示代码提示的话,按快捷键Ctrl+Alt+空格 (也可能不按Alt,看电脑)//不是,为什么我两种都不行啊www

{	//里面打开时钟,配置端口初始化等
//初始化时钟    RCC+APB2外设(APB2Periph)+时钟初始化(ClockCmd)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//LED都在GPIOA上
//配置端口模式
	GPIO_InitTypeDef GPIO_InitStructure;//先定义一个结构体变量
//结构体成员赋值
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//选中1,2号引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//选速度
//调用GPIO_Init函数
	GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化GPIOA外设+传递结构体变量的地址
	//GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_2);//加上这行,配为高电平,LED灭      SetBits中的s表示可以弄多个
}

//创建LED开灭的函数
//自定义函数时,一定要写形参类型
void LED1_ON(void)//LED1亮
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
}
void LED1_Turn(void)//加上翻转功能
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1) == 0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	}
}
void LED1_OFF(void)//LED1灭
{
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
void LED2_ON(void)//LED2亮
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}
void LED2_Turn(void)//加上翻转功能
{
	if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2) == 0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
}
void LED2_OFF(void)//LED2灭
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);
}











