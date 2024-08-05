#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)//按键初始化函数
{
	//将按键两个端口都初始化为上拉输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//开时钟
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
	//配置成上拉输入，按下低电平，松手高电平。
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//因为读取按键,所以上拉输入模式
	//GPIO_Mode_IPU（上拉输入）能确保在按键未被按下时，IO引脚安全地读取到高电平状态,而不是悬空
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//这里的速度是输出速度,在输入模式下没用,但写了也没关系
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);//调函数
}

uint8_t Key_GetNum(void)//读取按键值函数
{//调用函数,返回按下按键的键码
	//返回值是uint8_t 即 unsigned char
	//此处char是整形
	uint8_t KeyNum = 0;//定义变量

	//读取GPIO端口
	//在gpio.h中有四个GPIO读取函数
	
	//uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	//读取输入数据寄存器某一个端口的输入值
	//用两个参数指定某一个端口
	//返回值uint8_t代表端口的高低电平
	
	//uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
	//比上个函数少了一个Bit,用来读取整个输入寄存器
	//参数只有一个,用来指定外设
	//返回值uint16_t 是个16位的数据,每一位代表一个端口值
	
	//uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	//读取输出数据寄存器的某一位,不是用来读取端口输入数据
	//一般用于输出模式,查看输出的内容
	
	///uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
	//相比上一个也少了Bit,用来读取整个输出寄存器


	//此处需读取外部输入的一个端口值
	//GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);
	//返回值为1/0  为0说明按下按钮
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0)//代表按键按下
	{
		Delay_ms(20);//要#include
		//用途:因为按键按下和松开都会有抖动,所以按下的一瞬间先延时20ms用来消抖
		
		//接着检测按键松手的情况,因为一般是松手后才工作
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);//如果按键一直按着就卡在这里
		Delay_ms(20);//消松手时抖动
		KeyNum = 1;
	}//这是PinB1的,B11也这样
	
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0)//代表按键按下
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0);
		Delay_ms(20);
		KeyNum = 2;//定义为2
	}//这是PinB11的
	
	return KeyNum;
}




