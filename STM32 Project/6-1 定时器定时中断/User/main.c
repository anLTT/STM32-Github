#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"

uint16_t Num = 0;

int main(void)
{
	//给定时器建一个模块
	//因为定时器不涉及外部硬件.所以建在System中
	//当然也可以放在其他文件夹里,毕竟文件夹是自己建的,可根据个人喜好
	//不过要保持良好的分类习惯,便于查找和管理
	
	OLED_Init();
	Timer_Init();
	OLED_ShowString(1,1,"Num:");

	while(1)
	{
		OLED_ShowNum(1,5,Num,5);
		
		////还可以看cnt计数器变化值的情况----不在前面打印"Cnt:",并且变得很快
		//OLED_ShowNum(2,5,TIM_GetCounter(TIM2),5);
	}

}

//中断函数可以放在任意的地方，因为函数名代表着函数的地址，在编译的时候，中断向量表里已经定义好了函数名，
//就是函数的入口地址，所以只要在这个工程里面，在哪都是可以跳转进去的

//所以放main中就不用跨文件了(当然还可以在要垮的文件中考前部分写 extern uint16_t Num;)



void TIM2_IRQHandler(void)//中断函数
{//产生更新中断时,会自动执行该中断函数,所以不用写到.h中
	
	//1.检查中断标志位
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)//TIM_IT_Update是更新中断
	{
	
		Num++;
		//最后清除标志位
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	
	}
}
//此时Timer.c文件中中断函数注释掉,可当作以后中断函数的模板



