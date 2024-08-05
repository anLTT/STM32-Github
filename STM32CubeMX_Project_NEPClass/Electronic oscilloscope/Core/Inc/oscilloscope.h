#ifndef __OSCILLOSCOPE_H
#define __OSCILLOSCOPE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "struct_typedef.h"
	
u8 usart_get_data(char *cmd,float *d1,float *d2,float *d3);
void get_cmd(void);










#ifdef __cplusplus
}
#endif

#endif // !__OSCILLOSCOPE_H




