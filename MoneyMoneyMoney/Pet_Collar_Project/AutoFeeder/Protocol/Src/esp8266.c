#include "esp8266.h"
#include "usart.h"
#include "stm32f4xx_hal_uart.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "core_json.h"

/*
*************************************
宏定义
*************************************
*/
#define WIFI_SSID        "CHINE_PDCN"
#define WIFI_PASSWD      "xlmqwa1006"

#define MQTT_CLIENT_ID   "mqtt_stm32|securemode=2\\,signmethod=hmacsha1\\,timestamp=1687594902069|"   
#define MQTT_USER_NAME   "mqtt_stm32&a1TGt6tIcAE"
#define MQTT_PASSWD      "556483AFA86B8FF534E3DB0A14EE7A36D2910B2D"
#define BROKER_ASDDRESS  "a1TGt6tIcAE.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SUB_TOPIC        "/sys/a1TGt6tIcAE/mqtt_stm32/thing/service/property/set"
#define PUB_TOPIC        "/sys/a1TGt6tIcAE/mqtt_stm32/thing/event/property/post"
#define JSON_FORMAT      "{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}"

/*
*************************************
变量定义
*************************************
*/
unsigned char receive_buf[512];	  //接收缓冲区
unsigned char receive_start = 0;	//接收开始标志位
uint16_t receive_count = 0;	      //接收计数
uint16_t receive_finish = 0;	    //接收完成标志位 

/**
  * @brief          解析json消息
  * @param[in]      json_msg:json消息,json_len:json长度
  * @retval         返回0表示json解析成功,否则返回1
  */
uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len)
{
  uint8_t retval =0;
  
  JSONStatus_t result;
  char query[] = "params.light";
  size_t queryLength = sizeof( query ) - 1;
  char * value;
  size_t valueLength;
  result = JSON_Validate((const char *)json_msg, json_len);
  if( result == JSONSuccess)
  {
    result = JSON_Search((char *)json_msg, json_len, query, queryLength,&value, &valueLength );
    if( result == JSONSuccess)
    {
      char save = value[valueLength];
      value[valueLength] = '\0';
      printf("Found: %s %d-> %s\n", query, valueLength,value);
      value[valueLength] = save;
      retval = 0;
    }
    else
    {
      retval = 1;
    }
  }
  else
  {
    retval = 1;
  }  
  return retval;
}
/**
  * @brief          处理UART2接收
  * @param[in]      none
  * @retval         none
  */
void uart2_receiver_handle(void)
{
  unsigned char receive_data = 0;   
  if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
  {
    HAL_UART_Receive(&huart2, &receive_data, 1, 1000);//接收1字节数据
    receive_buf[receive_count++] = receive_data;
    receive_start = 1;	                              //接收开始标志位1
    receive_finish = 0;	                              //接收完成标志位0
  }
}
/**
  * @brief          清空UART2接收缓冲区
  * @param[in]      len:清空长度
  * @retval         none
  */
void uart2_receiver_clear(uint16_t len)	
{
  memset(receive_buf,0x00,len);
  receive_count = 0;
  receive_start = 0;
  receive_finish = 0;
}
/**
  * @brief          发送安信可命令
  * @param[in]      cmd:命令,len:命令长度,rec_data:接收数据
  * @retval         none
  */
uint8_t WB2_12F_Send_AT_Cmd(unsigned char *cmd,unsigned char len,char *rec_data)	
{
  unsigned char retval =0;
  unsigned int count = 0;

  HAL_UART_Transmit(&huart2, cmd, len, 1000);	                                   
  while((receive_start == 0)&&(count<1000))
  {
    count++;
    HAL_Delay(1);
  }

  if(count >= 1000)	
  {
    retval = 1;	
  }
  else	
  {
    do
    {
      receive_finish++;
      HAL_Delay(1);
    }
    while(receive_finish < 500);
    retval = 2;
    if(strstr((const char*)receive_buf, rec_data))	
    {
      retval = 0;	
    }
  }
  uart2_receiver_clear(receive_count);
  return retval;
}
/**
  * @brief          配置安信可网络
  * @param[in]      none
  * @retval         成功返回0,否则返回1
  */
uint8_t WB2_12F_Config_Network(void)
{
  uint8_t retval =0;
  uint16_t count = 0;
  
  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n",strlen("AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n"), 1000);
  
  while((receive_start == 0)&&(count<1000))
  {
    count++;
    HAL_Delay(1);
  }
  
  if(count >= 1000)	
  {
    retval = 1;	
  }
  else
  {
    HAL_Delay(8000);
    if(strstr((const char*)receive_buf, "OK"))	
    {
      retval = 0;	
    }
    else
    {
      retval = 1;
    }
  }
  uart2_receiver_clear(receive_count);
  return retval;
}
/**
  * @brief          连接MQTT服务器
  * @param[in]      none
  * @retval         成功返回0,否则返回1
  */
uint8_t WB2_12F_Connect_Server(void)
{
  uint8_t retval=0;
  uint16_t count = 0;

  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n",strlen("AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n"), 1000);	
  while((receive_start == 0)&&(count<1000))	
  {
    count++;
    HAL_Delay(1);
  }
  
  if(count >= 1000)	
  {
    retval = 1;	
  }
  else	
  {
    HAL_Delay(5000);
    if(strstr((const char*)receive_buf, "OK"))	
    {
      retval = 0;	
    }
    else
    {
      retval = 1;	
    }
  }
  uart2_receiver_clear(receive_count);	
  return retval;
}
/**
  * @brief          复位ESP8266
  * @param[in]      none
  * @retval         成功返回0,否则返回1
  */
uint8_t esp8266_reset(void)
{
  uint8_t retval =0;
  uint16_t count = 0;
  
  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+RST\r\n",8, 1000);
  while((receive_start == 0)&&(count<2000))	
  {
    count++;
    HAL_Delay(1);
  }
  if(count >= 2000)	
  {
    retval = 1;	
  }
  else	
  {
    HAL_Delay(5000);
    if(strstr((const char*)receive_buf, "OK"))	
    {
      retval = 0;	
    }
    else
    {
      retval = 1;	
    }
  }
  uart2_receiver_clear(receive_count);	
  return retval;
}
/**
  * @brief          发送ESP8266消息
  * @param[in]      none
  * @retval         成功返回0,否则返回1
  */
uint8_t esp8266_send_msg(void)	
{
  uint8_t retval =0;	
  uint16_t count = 0;			
  static uint8_t error_count=0;
  unsigned char msg_buf[256];
  
  //sprintf((char *)msg_buf,"AT+MQTTPUB=0,\""PUB_TOPIC"\",\""JSON_FORMAT"\",0,0\r\n",temp_value,humi_value);
  HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
  HAL_UART_Transmit(&huart1, (unsigned char *)msg_buf,strlen((const char *)msg_buf), 1000);	
  while((receive_start == 0)&&(count<500))	
  {
    count++;
    HAL_Delay(1);
  }
  if(count >= 500)	
  {
    retval = 1;	
  }
  else	
  {
    HAL_Delay(50);
    if(strstr((const char*)receive_buf, "OK"))	
    {
      retval = 0;	
      error_count=0;
    }
    else 
    {
      error_count++;
      if(error_count==5)
      {
        error_count=0;
        printf("RECONNECT MQTT BROKER!!!\r\n");
        esp8266_init();
      }
    }
  }
  uart2_receiver_clear(receive_count);	
  return retval;
}
/**
  * @brief          接收ESP8266消息
  * @param[in]      none
  * @retval         成功返回0,否则返回1
  */
uint8_t esp8266_receive_msg(void)	
{
  uint8_t retval =0;
  int msg_len=0;
  uint8_t msg_body[128] = {0};
  
  if(receive_start == 1)	
  {
    do
    {
      receive_finish++;
      HAL_Delay(1);
    }
    while(receive_finish < 5);	
    
    if(strstr((const char*)receive_buf,"+MQTTSUBRECV:"))
    {
      sscanf((const char *)receive_buf,"+MQTTSUBRECV:0,\""SUB_TOPIC"\",%d,%s",&msg_len,msg_body);
      printf("len:%d,msg:%s\r\n",msg_len,msg_body);
      if(strlen((const char*)msg_body)== msg_len)
      {
        retval = parse_json_msg(msg_body,msg_len);
      }
      else
      {
        retval = 1;
      }
    }
    else 
    {
      retval = 1;
    }
  }
  else
  {
    retval = 1;
  }
  uart2_receiver_clear(receive_count);	
  return retval;
}
/**
  * @brief          初始化ESP8266
  * @param[in]      none
  * @retval         none
  */
void esp8266_init(void)
{
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);           											//使能UART2接收中断
  
  printf("1.SETTING STATION MODE\r\n");
  while(esp8266_send_cmd((uint8_t *)"AT+CWMODE=1\r\n",strlen("AT+CWMODE=1\r\n"),"OK")!=0)
  {
    HAL_Delay(1000);
  }
  printf("2.CLOSE ESP8266 ECHO\r\n");
  while(esp8266_send_cmd((uint8_t *)"ATE0\r\n",strlen("ATE0\r\n"),"OK")!=0)
  {
    HAL_Delay(1000);
  }
  printf("3.NO AUTO CONNECT WIFI\r\n"); 
  while(esp8266_send_cmd((uint8_t *)"AT+CWAUTOCONN=0\r\n",strlen("AT+CWAUTOCONN=0\r\n"),"OK")!=0)
  {
    HAL_Delay(1000);
  }
  printf("4.RESET ESP8266\r\n");
  while(esp8266_reset() != 0)
  {
    HAL_Delay(5000);
  }
  printf("5.CONFIG WIFI NETWORK\r\n");
  while(esp8266_config_network() != 0)
  {
    HAL_Delay(8000);
  }
//	printf("6.MQTT USER CONFIG\r\n");
//	while(esp8266_send_cmd((uint8_t *)"AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n",
//                          strlen("AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n"),"OK")!=0)
//	{
//		HAL_Delay(2000);
//	}
//	printf("7.CONNECT MQTT BROKER\r\n");
//	while(esp8266_connect_server() != 0)
//	{
//		HAL_Delay(8000);
//	}
//	printf("8.SUBSCRIBE TOPIC\r\n");
//	while(esp8266_send_cmd((uint8_t *)"AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n",strlen("AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n"),"OK")!=0)
//	{
//		HAL_Delay(2000);
//	}
//	printf("9.ESP8266 INIT OK!!!\r\n");
}
