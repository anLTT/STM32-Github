#ifndef __OSCILLOSCOPE_H
#define __OSCILLOSCOPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "struct_typedef.h"
#include "stm32f4xx_hal_usart.h"
	
//��λ������λ�����Ͳ���
void send_wave(void);

//���η�������
void usart_senddatas(USART_HandleTypeDef* huart,uint8_t* addr,int size);

uint8_t usart_get_data(char *cmd,float *d1,float *d2,float *d3);

//���յĴ���
 void get_cmd(void);

 //�������ɺ���
 void getdatas(void);








#ifdef __cplusplus
}
#endif

#endif // !__OSCILLOSCOPE_H




