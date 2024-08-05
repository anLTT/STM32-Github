#include "stm32f10x.h"                  // Device header
#include "Delay.h"

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//开启GPIOA时钟 - ENABLE对应的是开启
	//时钟会产生周期性的方波，寄存器会在上升沿或者下降沿工作（移位，锁存），没有时钟，寄存器无法工作（数电会讲）
	//STM32为了低功耗省电，默认关闭时钟，需要用到的才打开。
	
	GPIO_InitTypeDef GPIO_InitStructure;//GPIO初始化
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置成推挽模式
	//高低电平都高驱动,LED正反接时都能亮
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//换成开漏输出模式
	//当LED为高电平点亮的时候,LED不亮,说明开漏模式高电平无驱动能力
	//LED改回低电平点亮,LED亮,说明开楼模式低电平有驱动能力
	//再改为推挽输出,一般用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//端口模式 - 推免输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//0号端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIOA初始化
	
	//使用相关函数 - 推免模式,设置低电平亮高电平灭
	//GPIO_ResetBits(GPIOA,GPIO_Pin_0);//  -  设置低电平
	//GPIO_SetBits(GPIOA,GPIO_Pin_0);//  -  设置高电平
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);//Bit_RESET  -  设置低电平
	//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);//Bit_SET  -  设置高电平
	while(1)
	{
		//好像第一节有关程序的说了,要while(1){}因为单片机工作不能停
		
		
		
		
		
		
		
		
		
		
		//用前两种介绍的函数
		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
		Delay_ms(500);
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
		Delay_ms(500);//延时  -  毫秒
		//用第三种介绍的函数
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
		Delay_ms(500);
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
		Delay_ms(500);
		//如果不想用给定的,想让1-高电平,0-低电平
		//直接用回警告枚举类型里用了别的类型,所以用强制类型转换为枚举里的BitAction
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)0);
		Delay_ms(500);
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,(BitAction)1);
		Delay_ms(500);
	}

}



