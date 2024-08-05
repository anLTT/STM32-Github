#include "stm32f10x.h"                  // Device header

//初始化函数
void Timer_Init(void)
{//在此处初始化定时器(TIM2 通用计时器)
	
	//先看定时器库函数有哪些
	//找tim.h

	//1. 打开RCC时钟
	//打开后,定时器的基准时钟和整个外设的时钟会同时打开
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//TIM2时APB1总线外设
	
	//2.选择时基单元的时钟源
	//对于定时中断,选择内部时钟源
	//因为定时器上电后默认是内部时钟,所以这步可省略
	TIM_InternalClockConfig(TIM2);
	
	//3.配置时基单元(PSC:预分频器  CNT:计数器   ARP:自动重装器)
	//包括预分频器,自动重装器,计数模式等
	//这些参数可直接用一个结构体配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//1分频
	
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//计数器模式    有向上计数,向下计数和三种中央对其的计数
	//此处选向上计数
	
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;
	//ARR自动重装器的值
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;
	//PSC预分频器的值
	
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	//重复计时器的值
	//重复计时器只在高级定时器中有,我们不需要用,所以赋0
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化时基单元


	//4.配置中断输出控制,允许更新中断输出到NVIC
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);//手动清除更新中断标志位
	//不加的话每次都是从1开始
	//因为...中写了一个刚初始化就进中断
	TIM_ITConfig(TIM2,TIM_EventSource_Update,ENABLE);

	//5.配置NVIC,打通中断通道
	//在NVIC中打开定时器中断的通道,并分配一个优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;//中断通道    TIM2_IRQn为定时器2在NVIC的通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	//6?最后一步?启动定时器
	TIM_Cmd(TIM2,ENABLE);
	//定时器开始工作,产生更新时触发中断
	
	//写了这么久还只是定时器中高端初始化函数www
	//还要写中断函数
	
	
	/*//6.运行控制

	//7.整个模块配置完后,还需使能计数器,不然计数器不会运行
	//使能后,计数器开始计数,计数器更新时,触发中断

	//8.定时器中断函数
	*/
}


/*
void TIM2_IRQHandler(void)//中断函数
{//产生更新中断时,会自动执行该中断函数,所以不用写到.h中
	//1.检查中断标志位
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)//TIM_IT_Update是更新中断
	{	
		//最后清除标志位
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}
*/















