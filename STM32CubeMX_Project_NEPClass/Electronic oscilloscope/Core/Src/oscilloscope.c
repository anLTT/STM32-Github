#include "oscilloscope.h"
#include "struct_typedef.h"
#include "main.h"



char usart_readbuff[30] = {0};//串口接收缓存数组
u8 usart_readok = 0;//一帧数据处理标志

extern fp32 cmd_P;
extern fp32 cmd_I;
extern fp32 cmd_D;


//波形发送
//使用UART6进行


////下位机给上位机发送波形
//void send_wave(void)
//{
//    //定义通道名帧头帧尾
//    u8 frameNameHead[] = "AABBCC";
//    u8 frameNameEnd[] = "CCBBAA";

//    //定义数据帧头帧尾
//    u8 frameDataHead[] = "DDEEFF";
//    u8 frameDataEnd[] = "FFEEDD";

//    //定义通道名
//    u8 name[] = {"target_pos,ecd_pos,P,I,D"};

//    //赋值数据
//    float channels[5];
//    channels[0] = cmd_target_pos;
//    channels[1] = cmd_ecd_pos;
//    channels[2] = cmd_P;
//    channels[3] = cmd_I;
//    channels[4] = cmd_D;

//    //通过串口6,给上位机发送数据
//    //下面的sizeof(frameNameHead)-1中的减1,就是忽略掉frameNameHead字符串后面的'\0'字符
//    usart_senddatas(USART6,frameNameHead,sizeof(frameNameHead)-1); 
//    usart_senddatas(USART6,name,sizeof(name)-1);
//    usart_senddatas(USART6,frameNameEnd,sizeof(frameNameEnd)-1);

//    usart_senddatas(USART6,frameDataHead,sizeof(frameDataHead)-1);
//    usart_senddatas(USART6,(u8*)channels,sizeof(channels));
//    usart_senddatas(USART6,frameDataEnd,sizeof(frameDataEnd)-1);


//}

////波形发送驱动
//void usart_senddatas(USART_HandleTypeDef* huart,u8* addr,int size)
//{
//    while(size--)//判断数据发送完没有
//    {
//        while(USART_GetFlagStatus(USARTx,USART_FLAG_TC) == RESET);//等待上一个byte的数据发送结束
//        USART_SendData(USARTx,*addr);//调用STM32标准库函数发送数据
//        addr++;//地址偏移
//    }
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











