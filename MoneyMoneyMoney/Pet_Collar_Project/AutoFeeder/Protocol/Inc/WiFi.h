#ifndef __WIFI_H
#define __WIFI_H

#include "main.h"

void Aliyun_AT(const char* topic, const char* message);//用于执行阿里云平台的AT命令
void Aliyun_AT_Init(void);//初始化MQTT连接参数
void MQTTPublish(const char* topic, const char* message);//发布消息
void WB2_12F_Send_AT_Cmd(char *cmd, u16 waittime);//发送AT命令到模块
void WB2_12F_restore(void);// 函数声明：发送恢复出厂默认指令初始化模块，清除WIFI模块的配网信息
void WB2_12F_Get_LinkStatus(void);// 函数声明：获取模块的网络连接状态
void WB2_12F_Get_MQTTStatus(void);// 函数声明：获取模块的MQTT连接状态











#endif 