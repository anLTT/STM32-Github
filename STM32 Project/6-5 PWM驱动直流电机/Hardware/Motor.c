#include "stm32f10x.h"                  // Device header
#include "PWM.h"		//继承PWM的模块
 
void Motor_Init(void)//初始化模块
{
	//调用底层的PWM_Init,初始化PWM  --- 套娃,主函数中到时候好像也要PWM_Init();
	PWM_Init();
	
	//同时电机模块中,还多了电机方向控制的AN1和AN2
	//所以要额外初始化方向控制的两个脚
	//从LED.c中偷GPIO初始化
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//这样就初始化好了电机方向引脚
}


//设置速度函数
void Motor_SetSpeed(int8_t Speed)//参数为带符号的速度变量,负数表示反转,范围给-100~100
{
	//正反转用if分开
	if(Speed >= 0)
	{//正转逻辑
		//方向
		GPIO_SetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);//方向控制两引脚为高低电平,哪个高哪个低不重要,主要是要极性不一样
		//速度
		PWM_SetCompare3(Speed);//改通道3CCR值
	}
	else//Speed小于0,代表反转
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
		GPIO_SetBits(GPIOA,GPIO_Pin_5);//Set和Reset反过来就能反转了
		PWM_SetCompare3(-Speed);//这个函数只能传正值,所以带负号
	}
}















