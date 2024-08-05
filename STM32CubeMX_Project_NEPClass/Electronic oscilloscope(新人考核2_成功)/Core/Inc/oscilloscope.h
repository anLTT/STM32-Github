#ifndef __OSCILLOSCOPE_H
#define __OSCILLOSCOPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "struct_typedef.h"
	
//��λ������λ�����Ͳ���
void send_wave(void);

//���η�������
void usart_senddatas(USART_HandleTypeDef* huart,u8* addr,int size);

u8 usart_get_data(char *cmd,float *d1,float *d2,float *d3);

//���յĴ���
 void get_cmd(void);

 //�������ɺ���
 void getdatas(void);








#ifdef __cplusplus
}
#endif

#endif // !__OSCILLOSCOPE_H




