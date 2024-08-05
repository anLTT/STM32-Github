#include "stm32f10x.h"                  // Device header

//PWM初始化函数
void PWM_Init(void)
{
	//打开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	//开AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	//初始化GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//此时要用复用推挽输出,不然的话引脚控制权就不是片上外设(此处是TIM2_CH1),而是输出数据寄存器
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	//选择内部时钟
	TIM_InternalClockConfig(TIM2);
	//初始化时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;  //ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;//PCS
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//给结构体赋初始值

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置输出比较的模式
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;//设置输出比较的极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;//设置CCR
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	
	
	
	
	//启动定时器
	TIM_Cmd(TIM2,ENABLE);

}

void PWM_SetCompare3(uint16_t Compare)
{
	TIM_SetCompare3(TIM2,Compare);//单独更改通道3的CCR的值

}





















