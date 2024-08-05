#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init(void)//舵机初始化函数
{
	PWM_Init();//初始化PWM底层
	
}

void Servo_SetAngle(float Angle)//参数定义成float方便之后计算
{
	PWM_SetCompare2(Angle / 180 * 2000 + 500);//参数要算
//0   ---   500
//180 ---  2500
//范围 180 --- 2000 所以进行缩放,然后再偏移500
}























