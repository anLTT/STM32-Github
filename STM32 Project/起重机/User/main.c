#include "stm32f10x.h"                  // Device header

#include "my_head.h"
#include "stdio.h"
#include "led.h"
#include "stdint.h"
#include "delay.h"

//#include "usart.h"	
#include "motor_pwm.h"

#include "encoder.h"
 
 
 int main(void)
 {	
	u16 led0pwmval=0;    
	u8 dir=1;	
    uart_init(9600);	     //串口1初始化
	delay_init();	    	 //延时函数初始化	  
	LED_Init();		  	//初始化与LED连接的硬件接口
    NVIC_Configuration();	//中断优先级
    Motor_Init();       //
	Motor_PWM_Init(7199,0);//不分频,初始化PWM 10KHZ,驱动电机
   	Encoder_Init_TIM2();//TIM2初始化为编码器接口模式
    Encoder_Init_TIM3();//TIM3初始化为编码器接口模式
     
    while(1)
	{ 
		if(dir)led0pwmval++;
		else led0pwmval--;	 
 		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;
		TIM_SetCompare1(TIM1,led0pwmval);
        Set_Pwm(6000,7000);
        printf("%d\t",Read_Encoder(2));
        printf("%d",Read_Encoder(3));
        printf("\r\n");
        Delay_ms(1000);
        Set_Pwm(-5000,-4000);
        printf("%d\t",Read_Encoder(2));
        printf("%d",Read_Encoder(3));
        printf("\r\n");
        Delay_ms(1000);
        
	} 
}

