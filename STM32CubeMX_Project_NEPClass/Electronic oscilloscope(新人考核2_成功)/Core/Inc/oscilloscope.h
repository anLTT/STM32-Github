#ifndef __OSCILLOSCOPE_H
#define __OSCILLOSCOPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "struct_typedef.h"
	
//下位机给上位机发送波形
void send_wave(void);

//波形发送驱动
void usart_senddatas(USART_HandleTypeDef* huart,u8* addr,int size);

u8 usart_get_data(char *cmd,float *d1,float *d2,float *d3);

//接收的代码
 void get_cmd(void);

 //波形生成函数
 void getdatas(void);








#ifdef __cplusplus
}
#endif

#endif // !__OSCILLOSCOPE_H




