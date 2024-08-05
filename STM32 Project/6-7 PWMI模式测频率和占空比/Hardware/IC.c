#include "stm32f10x.h"                  // Device header

void IC_Init(void)
{//输入捕获初始化,步骤对照基本结构来
	//开时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//我们的TIM2要输出PWM,所以输入捕获换用PWM3
													   //TIM3也是APB1的外设
	
	//配GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//用TIM3CH1(CH-通道),所以PA6
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//要上拉输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//配时基单元
	//选择内部时钟
	TIM_InternalClockConfig(TIM3);
	//初始化时基单元(设时基单元结构体参数)
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;  //ARR --- 由上节,要设置大,防止计数溢出
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;//PCS --- 根据信号频率分布范围调整
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	
	
	//4.初始化输入捕获单元
		//结构体起名
	TIM_ICInitTypeDef TIM_ICInitSturcture;
		//引出参数
	TIM_ICInitSturcture.TIM_Channel = TIM_Channel_1;//4个通道中决定一个
	TIM_ICInitSturcture.TIM_ICFilter = 0x0F;//配置滤波器,增大参数可以避免毛刺和噪声的干扰
	TIM_ICInitSturcture.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿触发
	TIM_ICInitSturcture.TIM_ICPrescaler =TIM_ICPSC_DIV1;//预分频器
	TIM_ICInitSturcture.TIM_ICSelection = TIM_ICSelection_DirectTI;//触发信号从哪个引脚输入
	
	TIM_PWMIConfig(TIM3,&TIM_ICInitSturcture);

	//5.触发源选择
	TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
	//6.配置从模式为Reset
	TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_Reset);
	//7.最后,启动定时器
	TIM_Cmd(TIM3,ENABLE);
}

uint32_t IC_GetFreq(void)
{
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);//返回最新周期的频率值,单位是Hz
}

uint32_t IC_GetDuty(void)
{
	return (TIM_GetCapture2(TIM3) + 1)*100 / (TIM_GetCapture1(TIM3) + 1); //返回占空比,范围为0~100,对应0%~100%
}








