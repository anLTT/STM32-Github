#ifndef __LED_H      //添加防止头文件重复包含的代码      //#ifndef __LED_H  : 如果没定义LED这个字符串
#define __LED_h										 	 //那么定义这个字符串

void LED_Init(void);//因为要外部引用,所以申明
void LED1_ON(void);
void LED1_Turn(void);
void LED1_OFF(void);
void LED2_ON(void);
void LED2_Turn(void);
void LED2_OFF(void);

#endif													 //和#ifndef组成的括号,函数和变量声明放在里面
//.h存放驱动程序可以对外提供的函数or变量的声明
//文件以空行结尾
