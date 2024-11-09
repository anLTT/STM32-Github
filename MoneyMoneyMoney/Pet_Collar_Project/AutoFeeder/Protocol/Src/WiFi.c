#include "WiFi.h"
// #include "tcp.h"
#include "usart.h"
// #include ""          引入WB2-12F的头文件
//#include "delay.h"    引入延时函数
//#include "key.h"      引入按键处理函数
#include "string.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"



//安信可联网模块

//************************************************************

//Function:
//设备通过网络连接到阿里云平台,并进行MQTT通信




//************************************************************

volatile u8 TcpClosedFlag = 0;//标记TCP连接的状态

void Aliyun_AT(const char* topic, const char* message)
{//用于执行阿里云平台的AT命令

    Aliyun_AT_Init();//初始化MQTT连接参数

    // 发布消息到指定主题
    MQTTPublish(topic,message);
}



void Aliyun_AT_Init(void)
{//初始化MQTT连接参数
    WB2_12F_Send_AT_Cmd("AT+MQTT=1,ningbowanchong.com",1000);//设置MQTT服务器的域名
    WB2_12F_Send_AT_Cmd("AT+MQTT=2,1883",1000);//设置MQTT服务器的端口
    WB2_12F_Send_AT_Cmd("AT+MQTT=3,1",1000);//设置连接方式
    WB2_12F_Send_AT_Cmd("AT+MQTT=4,1234",1000);//设置用户ID
    WB2_12F_Send_AT_Cmd("AT+MQTT=5,admin",1000);//设置MQTT用户名
    WB2_12F_Send_AT_Cmd("AT+MQTT=6,public",1000);//设置MQTT密码
    WB2_12F_Send_AT_Cmd("AT+MQTT",1000);//连接MQTT服务器
    WB2_12F_Send_AT_Cmd("AT+MQTTSUB=testtopic0,0",1000);//订阅一个名为testtopic0的主题
}

void MQTTPublish(const char* topic, const char* message)
{
    char cmd[256];
    // 构建AT命令字符串
    sprintf(cmd, "AT+MQTTPUB=%s,1,0,%s", topic, message);
    // 发送AT命令
    WB2_12F_Send_AT_Cmd(cmd, 1000);
}





















































































































// //********************************************* */
//         if(key)  // 如果检测到按键被按下。
//         {  
//             switch(key)  // 根据按键值执行不同的操作。
//             {  
//                 case WKUP_PRES:  // 如果按下的是WKUP_PRES键（可能是一个特定的按键标识符）。
//                     // LED_ON();  // 打开LED灯，此行代码被注释掉了。
//                     ESP8266_Net_Mode_Choose(STA);  // 将ESP8266设置为STA（Station）模式，即客户端模式。

//                     ESP8266_JoinAP(Direct_Connection);  // 连接到Wi-Fi接入点，使用直连方式，方便调试。
//                     // 如果需要一键配网或AP配网，需要更改参数值。
//                     break;
//                 case KEY0_PRES:  // 如果按下的是KEY0_PRES键。
//                     ESP8266_restore();  // 恢复ESP8266模块的设置到出厂默认状态。
//                     break;
//                 case KEY1_PRES:  // 如果按下的是KEY1_PRES键。
//                     // 目前没有定义任何操作。
//                     break;
//             }
//         }
//         else  // 如果没有按键被按下。
//             delay_ms(10);  // 延时10毫秒，减少CPU占用。
//     }  // 配网步骤结束。

//     ESP8266_Set_Device_Information();  // 设置ESP8266模块的设备信息。
//     ESP8266_Configure_MQTT_Connection_Parameters();  // 配置MQTT连接参数。

//     while(ESP8266_Scan1())  // 继续循环执行Wi-Fi网络扫描，直到找到网络。
//     {
//         ESP8266_Subscribe_To_MQTT_Topic_Information();  // 订阅MQTT主题。
//     };
// }

// void Aliyun_AT2(void)
// {//用于执行阿里云平台的AT命令
//     uint8_t key = 0;//存储按键扫描的结果
//     WB2_12F_Get_LinkStatus();//获取模块的网络连接状态
//     WB2_12F_Get_MQTTStatus();//获取模块的MQTT连接状态

//     while (WB2_12F_Scan())
//     {//循环执行WiFi网络扫描,直到找到网络
//         key = KEY_Scan(0);//扫描按键,参数0可能是按键的编号/类型

//         if(key)//如果监测到按键被按下
//         {
//             switch(key)//根据按键值执行不同的操作
//             {
//                 case WKUP_PRES://如果按下的是WKUP_PRES键
//                     //LED_ON();//打开LED灯
//                     WB2_12F_Net_Mode_Choose(STA);//将模块设置为STA(Station)模式,即客户端模式
//                     WB2_12F_JoinAP(Direct_Connection);//连接到Wi-Fi接入点,使用直连方式,方便调试
//                     break;
//                 case KEY0_PRES://如果按下的是KEY0_PRES键
//                     WB2_12F_restore();//恢复模块的设置到出厂默认状态
//                     break;
//                 case KEY1_PRES://如果按下的是KEY1_PRES键
//                     //目前没有定义任何操作
//                     break;
//             }
//         }
//         else//如果没有按键被按下
//             delay_ms(10);//延时10毫秒,减少CPU占用
//         }// 配网步骤结束。
    
//     ESP8266_Set_Device_Information();  // 设置ESP8266模块的设备信息。
//     ESP8266_Configure_MQTT_Connection_Parameters();  // 配置MQTT连接参数。

//     while(ESP8266_Scan1())  // 继续循环执行Wi-Fi网络扫描，直到找到网络。
//     {
//         ESP8266_Subscribe_To_MQTT_Topic_Information();  // 订阅MQTT主题。
//     };
    
//     }

//******************************************************************************************** */
void WB2_12F_Send_AT_Cmd(char *cmd, u16 waittime)//等待最多waittime毫秒以期待响应
{//发送AT命令到模块
    UART1_SendString(cmd);//发送AT命令到模块
    while (UART1_ReceiveByte() != 'K');//等待模块的响应
    while (UART1_ReceiveByte() != '\r');
    while (UART1_ReceiveByte() != '\n');
}


void WB2_12F_restore(void)// 函数声明：发送恢复出厂默认指令初始化模块，清除WIFI模块的配网信息
{
    WB2_12F_Send_AT_Cmd("AT+RESTORE",1000);
}

void WB2_12F_Get_LinkStatus(void)// 函数声明：获取模块的网络连接状态
{
    WB2_12F_Send_AT_Cmd("AT+STAINFO?",1000);
}

void WB2_12F_Get_MQTTStatus(void)// 函数声明：获取模块的MQTT连接状态
{
    WB2_12F_Send_AT_Cmd("AT+MQTT?",1000);
}

// void WB2_12F_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)// 函数声明：设置WIFI工作模式
// {
//     switch ( enumMode )//根据传入的枚举值enumMode选择不同的网络模式
//     {
//         case STA://模式是STA
//             WB2_12F_Send_AT_Cmd("AT+WMODE=1,1",1000);//设置STA模式，保存到FLASH
//             break;
//         case AP://模式是AP
//             WB2_12F_Send_AT_Cmd("AT+WMODE=2,1",1000);//设置AP模式，保存到FLASH
//             break;
//         case STA_AP://模式是AP+STA
//             WB2_12F_Send_AT_Cmd("AT+WMODE=3,1",1000);//设置AP+STA模式，保存到FLASH
//             break;
//         default:
//             break;
//     }
// }

// void WB2_12F_JoinAP(Distribution_method method)// 函数声明：设置WIFI配网方式
// {
//     char cCmd[50];//定义一个字符数组cCmd，用于存储构建的AT命令
//     switch ( method )
//     {
//         case Direct_Connection://如果配网方式是直连路由
//             sprintf(cCmd, "AT+CWJAP=\"%s\",\"%s\"", STA_ssid, STA_passwd);//使用sprintf函数格式化AT+CWJAP命令，包含SSID和密码
//             WB2_12F_Send_AT_Cmd(cCmd, 3000);
//             break;
//         case SmartConfig://如果配网方式是一键配网
//             WB2_12F_Send_AT_Cmd("AT+TCSTARTSMART", 1000);
//             break;
//         case SoftAP://如果配网方式是AP配网
//             sprintf(cCmd, "AT+TCSAP=\"%s\",\"%s\"", AP_ssid, AP_passwd);//使用sprintf函数格式化AT+TCSAP命令，包含SSID和密码
//             WB2_12F_Send_AT_Cmd(cCmd, 3000);
//             break;
//     }
// }

// void WB2_12F_Get_LinkStatus(void)// 函数声明：获取模块的网络连接状态
// {
//     WB2_12F_Send_AT_Cmd("AT+CWJAP?",1000);//发送AT+CWJAP?命令到模块,等待最多1000毫秒以期待响应
// }






// void ESP8266_Disconnect_Ap(void) // 函数声明：断开与路由器的连接
// {
// 	 ESP8266_Send_AT_Cmd("AT+CWQAP",1000); // 发送AT+CWQAP命令到ESP8266模块，等待最多1000毫秒以期待响应，用于断开当前连接的AP
// }

// void ESP8266_Disconnect_MQTT(void) // 函数声明：断开腾讯云连接
// {
// 	 ESP8266_Send_AT_Cmd("AT+TCMQTTDISCONN",1000); // 发送AT+TCMQTTDISCONN命令到ESP8266模块，等待最多1000毫秒以期待响应，用于断开MQTT连接
// }






// void WiFi_Init(void)
// {
//     //初始化串口
//     UART1_Init(115200);
//     //初始化WIFI模块
//     WiFi_Reset();
//     //连接WIFI
//     WiFi_Connect();
//     //连接阿里云
//     WiFi_Connect_Aliyun();
// }

// void WiFi_Reset(void)
// {
//     //发送AT指令
//     UART1_SendString("AT+RST\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Connect(void)
// {
//     //发送AT指令
//     UART1_SendString("AT+CWMODE=1\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
//     //发送AT指令
//     UART1_SendString("AT+CWJAP=\"TP-LINK_2.4G\",\"12345678\"\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Connect_Aliyun(void)
// {
//     //发送AT指令
//     UART1_SendString("AT+CIPSTART=\"TCP\",\"a1Q5Q5Q5Q5Q.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Send_Data(u8 *data, u8 len)
// {
//     //发送AT指令
//     UART1_SendString("AT+CIPSEND=");
//     UART1_SendByte(len);
//     UART1_SendString("\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != '>');
//     //发送数据
//     UART1_SendData(data, len);
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Receive_Data(u8 *data, u8 len)
// {
//     u8 i;
//     //等待接收数据
//     while (UART1_ReceiveByte() != '+');
//     while (UART1_ReceiveByte() != 'I');
//     while (UART1_ReceiveByte() != 'P');
//     while (UART1_ReceiveByte() != 'D');
//     while (UART1_ReceiveByte() != 'A');
//     while (UART1_ReceiveByte() != 'T');
//     while (UART1_ReceiveByte() != 'A');
//     while (UART1_ReceiveByte() != ':');
//     while (UART1_ReceiveByte() != ' ');
//     //接收数据
//     for (i = 0; i < len; i++)
//     {
//         data[i] = UART1_ReceiveByte();
//     }
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Close(void)
// {
//     //发送AT指令
//     UART1_SendString("AT+CIPCLOSE\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Check(void)
// {
//     //发送AT指令
//     UART1_SendString("AT+CIPSTATUS\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'S');
//     while (UART1_ReceiveByte() != 'T');
//     while (UART1_ReceiveByte() != 'A');
//     while (UART1_ReceiveByte() != 'T');
//     while (UART1_ReceiveByte() != 'U');
//     while (UART1_ReceiveByte() != 'S');
//     while (UART1_ReceiveByte() != ':');
//     while (UART1_ReceiveByte() != ' ');
//     //接收数据
//     TcpClosedFlag = UART1_ReceiveByte();
//     //等待回复
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Send_Data_Aliyun(u8 *data, u8 len)
// {
//     //发送AT指令
//     UART1_SendString("AT+CIPSEND=");
//     UART1_SendByte(len + 2);
//     UART1_SendString("\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != '>');
//     //发送数据
//     UART1_SendByte(0x30 + len / 10);
//     UART1_SendByte(0x30 + len % 10);
//     UART1_SendData(data, len);
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Receive_Data_Aliyun(u8 *data, u8 len)
// {
//     u8 i;
//     //等待接收数据
//     while (UART1_ReceiveByte() != '+');
//     while (UART1_ReceiveByte() != 'I');
//     while (UART1_ReceiveByte() != 'P');
//     while (UART1_ReceiveByte() != 'D');
//     while (UART1_ReceiveByte() != 'A');
//     while (UART1_ReceiveByte() != 'T');
//     while (UART1_ReceiveByte() != 'A');
//     while (UART1_ReceiveByte() != ':');
//     while (UART1_ReceiveByte() != ' ');
//     //接收数据
//     for (i = 0; i < len; i++)
//     {
//         data[i] = UART1_ReceiveByte();
//     }
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Close_Aliyun(void)
// {
//     //发送AT指令
//     UART1_SendString("AT+CIPCLOSE=0\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'K');
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }

// void WiFi_Check_Aliyun(void)
// {
//     //发送AT指令
//     UART1_SendString("AT+CIPSTATUS=0\r\n");
//     //等待回复
//     while (UART1_ReceiveByte() != 'S');
//     while (UART1_ReceiveByte() != 'T');
//     while (UART1_ReceiveByte() != 'A');
//     while (UART1_ReceiveByte() != 'T');
//     while (UART1_ReceiveByte() != 'U');
//     while (UART1_ReceiveByte() != 'S');
//     while (UART1_ReceiveByte() != ':');
//     while (UART1_ReceiveByte() != ' ');
//     //接收数据
//     TcpClosedFlag = UART1_ReceiveByte();
//     //等待回复
//     while (UART1_ReceiveByte() != '\r');
//     while (UART1_ReceiveByte() != '\n');
// }


















