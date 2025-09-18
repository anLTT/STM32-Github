#ifndef __SCD30_H
#define __SCD30_H
#include "stm32f10x.h" 
 
uint8_t calculate_crc(uint8_t *data, uint8_t length);
char SCD30_convertHexToASCII(uint8_t digit);
uint8_t SCD30_Write_without_arguments(uint16_t command);
uint8_t SCD30_Write_with_arguments(uint16_t command, uint16_t arguments);
uint8_t SCD30_ReadRegister(uint16_t registerAddress, uint16_t *response, uint16_t delayTime);
uint8_t SCD30_getDataReadyStatus(void);
uint8_t SCD30_stopPeriodicMeasurement(uint16_t delayTime);
uint8_t SCD30_reInit(uint16_t delayTime);
uint8_t SCD30_StartPeriodicMeasurement(void);
uint8_t SCD30_readMeasurement(uint16_t *co2);
 
#define  Trigger_continuous_measurement   0x0010 //触发连续测量
#define  Stop_continuous_measurement      0x0104 //停止连续测量
#define  Set_measurement_interval         0x4600 //设置测量间隔
#define  Get_data_ready_status            0x0202 //获得数据准备状态
#define  Read_measurement                 0x0300 //读取数值
#define  Activate_ASC                     0x5306 //激活自动校准
#define  Activate_FRC                     0x5204 //激活强制校准
#define  Set_Temperature_Offset	    	  0x5403 //温度补偿
#define  Altitude_Compensation			  0x5102 //高度补偿
#define  Read_firmware_version			  0xD100 //读取固件版本
#define  Soft_reset						  0xD304 //软复位
 
 
typedef union
{
  int16_t signed16;
  uint16_t unsigned16;
} scd30_int16_uint16_t; // Avoid any ambiguity casting int16_t to uint16_t
 
typedef union
{
  uint16_t unsigned16;
  uint8_t bytes[2];
} scd30_uint16_Bytes_t; // Make it easy to convert 2 x uint8_t to uint16_t
 
 
typedef struct
{
	scd30_uint16_Bytes_t tempCO2;
	scd30_uint16_Bytes_t tempHumidity;
	scd30_uint16_Bytes_t tempTemperature;
}scd30_SensorData_t;
 
extern scd30_SensorData_t SCD30_SensorData;

static const unsigned char crc_table[];

unsigned char crc8scd30(unsigned char *ptr, unsigned char len);

#endif