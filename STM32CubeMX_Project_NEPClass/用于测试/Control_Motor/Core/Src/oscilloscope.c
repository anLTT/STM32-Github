#include "oscilloscope.h"
#include "struct_typedef.h"
#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

char usart_readbuff[30] = {0};//串口接收缓存数组
u8 usart_readok = 0;//一帧数据处理标志

extern fp32 cmd_P;
extern fp32 cmd_I;
extern fp32 cmd_D;
extern fp32 target_pos;
extern fp32 ecd_pos;




//波形发送
//使用UART6进行


//下位机给上位机发送波形
void send_wave(void)
{
    //定义通道名帧头帧尾
    u8 frameNameHead[] = "AABBCC";
    u8 frameNameEnd[] = "CCBBAA";

    //定义数据帧头帧尾
    u8 frameDataHead[] = "DDEEFF";
    u8 frameDataEnd[] = "FFEEDD";

    //定义通道名
    u8 name[] = {"target_pos,ecd_pos,P,I,D"};

    //赋值数据
    float channels[5];
    channels[0] = target_pos;
    channels[1] = ecd_pos;
    channels[2] = cmd_P;
    channels[3] = cmd_I;
    channels[4] = cmd_D;

    //通过串口6,给上位机发送数据
    //下面的sizeof(frameNameHead)-1中的减1,就是忽略掉frameNameHead字符串后面的'\0'字符
    HAL_UART_Transmit(&huart6,(uint8_t*)frameNameHead,sizeof(frameNameHead)-1,HAL_MAX_DELAY); 
    HAL_UART_Transmit(&huart6,(uint8_t*)name,sizeof(name)-1,HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart6,(uint8_t*)frameNameEnd,sizeof(frameNameEnd)-1,HAL_MAX_DELAY);

    HAL_UART_Transmit(&huart6,(uint8_t*)frameDataHead,sizeof(frameDataHead)-1,HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart6,(uint8_t*)channels,sizeof(channels),HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart6,(uint8_t*)frameDataEnd,sizeof(frameDataEnd)-1,HAL_MAX_DELAY);


}

////波形发送驱动
//void usart_senddatas(USART_HandleTypeDef* huart,u8* addr,int size)
//{
//    HAL_UART_Transmit(huart,addr,size,HAL_MAX_DELAY);
//}

u8 usart_get_data(char *cmd,float *d1,float *d2,float *d3)
{
    u8 flag = 0;
    if(usart_readok == 1)
    {
    if(sscanf(usart_readbuff,"%3s=%f,%f,%f#",cmd,d1,d2,d3)==4)
    {
        flag = 1;
    }
    //清除接收完成标志
    memset(usart_readbuff,0,sizeof(usart_readbuff));
    usart_readok = 0;
    }
    return flag;
}

 void get_cmd(void)//接收的代码
 {
     char u_buff[10];
     float u_d1,u_d2,u_d3;
     if(usart_get_data(u_buff,&u_d1,&u_d2,&u_d3))
     {
     if(strcmp(u_buff,"PID") == 0) //比较命令控制字符是否为PID
     {
         cmd_P = u_d1;
         cmd_I = u_d2;
         cmd_D = u_d3;
     }
     }
     memset(u_buff,0,sizeof(u_buff));
 }

// //波形生成函数
// void getdatas(void)
// {
//     static float x = 0.00f * PI;
//     datas[0] = sinf(x);
//     datas[1] = cosf(x);
//     datas[2] = cosf(2 * x);
//     datas[3] = 2 * cosf(x);
//    
//     x += 0.05f * PI;
// }











