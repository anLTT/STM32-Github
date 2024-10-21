#ifndef __SERVOS_H
#define __SERVOS_H

#include "main.h"

//舵机打开,设置角度
void Servo_Open(uint16_t angle);
//舵机关闭
void Servo_Close(void);
//定时自动开关舵机
void Servo_Auto(uint16_t open_time_seconds, uint16_t wait_time_hours);




#endif // !__SERVOS_H

