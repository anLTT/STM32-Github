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
	
	//初始化GPIO(从LED.c中偷的)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	//此时要用复用推挽输出,不然的话引脚控制权就不是片上外设(此处是TIM2_CH1),而是输出数据寄存器
	
	// 重映射  --- GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	
	
	
	
	
	//选择内部时钟
	TIM_InternalClockConfig(TIM2);
	//初始化时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;  //ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;//PCS
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	//定时器中断里是初始化NVIC
	//此处是初始化输出比较单元/通道
	//有void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
		//void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
		//void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
	    //void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
	//四个通道,不同通道对应GPIO口不同
	//PA0用第一个
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//给结构体赋初始值
	//作用:此时,由于用TIM2加上实际需求,下面的结构体变量赋值并没有所有都复制,因此未赋值的结构体变量的值是奇怪的
	//当之后程序其他因实际需求地方改变或者发生了奇怪的事情,就可能会用到我们没赋值的结构体变量,然后就会变得很怪
	//所以用此函数将所有变量赋初始值,然后再改想改的变量
	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置输出比较的模式
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;//设置输出比较的极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//设置输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;//设置CCR
	//Pulse直译是脉冲,实际上用来设计CCR寄存器的值
	//上面的ARR,PSC和这里的CCR共同决定输出PWM的周期和占空比
	
	 TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	 //因为还需要一个GPIO口输出PWM,所以到上面初始化GPIO口
	
	
	
	
	
	//启动定时器
	TIM_Cmd(TIM2,ENABLE);

}

void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2,Compare);//单独更改通道1的CCR的值

}





















