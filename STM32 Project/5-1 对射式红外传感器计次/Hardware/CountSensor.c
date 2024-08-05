#include "stm32f10x.h"                  // Device header

//计次
uint16_t CountSensor_Count;//全局变量默认初始化为0

//返回变量的函数
uint16_t CountSensor_Get(void)
{
	return CountSensor_Count;

}


void CountSensor_Init(void)//初始化函数
{
	//进行外部中断的配置

	//1.配置RCC,把涉及的外设的时钟都打开
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//开启AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//也是在APB2总线上的,不知道的话可以看函数定义中参数有没有
	//EXTI和NVIC的时钟一直开的,所以不用自己打开

	
	
	//2.配置GPIO,选择端口为输入模式
	//2.1配置结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	//2.2引出结构体成员
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//对外部中断来说,要选择浮空输入or上拉输入or下拉输入   - 具体看手册
	//在此处给上拉输入---默认高电平输入

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//2.3调用函数,初始GPIOX的外设
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	
	//3.配置AFIO,选择用到的GPIO,连接后面的EXTI
	//AFIO没自己的库函数,是跟GPIO并在一起的
	
	//GPIO_EXTILineConfig();//配置AFIO数据选择寄存器,来选择想要的中断引脚
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);
	//跳到定义中可看到虽然名称是GPIO,但用的是AFIO寄存器,所以实际上是AFIO函数
	//第一个参数,选择某个GPIO外设作为外部中断源
	//第二个参数指定配置的外设中断线
	
	
	
	//4.配置EXTI,
	//选择边沿触发方式,如上升沿/下降沿/双边沿
	//并选择触发响应方式,可选择中断响应或事件响应---一般为中断响应
	//(EXTI有库函数能用)
	
	//初始化EXTI
	//因为只有一个EXTI,所以不用像GPIO一样指定配置哪个EXTI
	//跳转定义看说明
	
	//定义结构体
	EXTI_InitTypeDef EXTI_InitStructure;
	//结构体成员赋值
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;//指定要配置的中断线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//指定选择中断线的新状态--ENABLE或DISABLE---开启中断,所以ENABLE
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//指定外部中断线的模式--- EXTI_Mode_Interrupt--中断模式 或者 EXTI_Mode_Event--事件模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//指定触发信号的有效边沿,根据实际选,此处选下降沿
	//注:定义后面写错了,不是EXTIMode_TypeDef,而是EXTITrigger_TypeDef
	//里面有 EXTI_Trigger_Rising--上升沿触发  
	//EXTI_Trigger_Falling--下降沿触发 
	//EXTI_Trigger_Rising_Falling--双边沿触发
 
	//因为用的下降沿触发,所以移开挡光片时触发中断
	//若是上升沿触发,则遮挡的时候才触发
	//若是双边沿触发,则遮挡和移开时都触发
	EXTI_Init(&EXTI_InitStructure);



	//5.配置NVIC,给中断选择合适的优先级
	//因为NVIC是内核外设,所以其库函数被分配到杂项里面了(misc.h)
	
	//配置中断之前,先用NVIC_PriorityGroupConfig指定中断分组,再用NVIC_Init初始化NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//根据实际选分组,此处选NVIC_PriorityGroup_2,较平均
	//pre-emption priority--抢占优先级    subpriority--响应优先级
	//因为分组整个芯片只能用一种,所以整个代码中只要执行一次就行了
	//此处是在模块中分组,所以要保证在其他模块也是这个分组-----也可以在main()函数开始就分组,模块中就不用分了
	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//指定中断通道开启或关闭
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//指定通道使能(ENABLE)或失能(DISABLE)
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//指定所选通道的抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//指定所选通道的响应优先级
	//因为此程序只有一个中断,所以优先级设置可随意|优先级在多个中断源同时申请,产生拥挤时才有用
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	//最后,通过NVIC,外部中断信号进入CPU,使得CPU从主函数跳到中断函数执行
	//中断程序需要自己写中断函数
	//STM32中,中断函数名固定,参考启动文件,每个中断通道对应一个中断函数
	//Start的startup_stm32f10x_md.s中的中断向量表中以IRQHandler结尾的字符串是中断函数的名字
	//EXTI15_10_IRQHandler是EXTI15到10的中断函数
}



void EXTI15_10_IRQHandler(void)//中断函数名称不能写错,否则进不了中断!!!!
{
	//一般先进行中断标志位的判断,确保是想要的中断源触发的函数(因为这函数EXTI10到15都能进来)
	 if(EXTI_GetITStatus(EXTI_Line14) == SET)//看EXTI14的中断标志位是不是1,因为返回值是SET或RESET,所以if
	 {//若是,执行中断程序
		 
	//中断程序结束后一定要清除中断函数标志位
	//因为只要中断标志位置1,程序就会跳到中断函数.不清除就会一直中断

		 //如果此处数据乱跳可以对GPIO14口进行再次判断：	
		 if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		 {CountSensor_Count ++;
		 
		 
		EXTI_ClearITPendingBit(EXTI_Line14);}//将中断标志位清除
	//接着测试能不能进入中断函数
	//中断函数不用声明,因为不需要调用,到时候自动执行
	 }

}














