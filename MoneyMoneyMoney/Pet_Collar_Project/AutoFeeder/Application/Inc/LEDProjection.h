#ifndef __LEDPROJECTION_H
#define __LEDPROJECTION_H

#include "main.h"

// 切换LED状态
void Toggle_LED(void) ;
// 检查按钮是否被按下
uint8_t Is_Button_Pressed(void);

//上两个函数的整合
void Check_And_Toggle_LED(void);





#endif

