
#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "stdint.h"
#include "Delay.h"
//#include "OLED.h"
#include "SCD30.h"
#define SCD30_ADDRESS		0x61
uint8_t periodic_Measurements_AreRunning=0;
#define SCD30_ENABLE_DEBUGLOG 0 
 
scd30_SensorData_t SCD30_SensorData;
//CRC验证
uint8_t calculate_crc(uint8_t *data, uint8_t length) {
    uint8_t crc = 0xFF;  // 初始化CRC寄存器
    uint8_t polynomial = 0x31;  // 使用的多项式
 
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];  // 将每个数据字节与当前CRC寄存器进行异或运算
 
        for (uint8_t j = 0; j < 8; j++) {  // 对每个结果的每一位进行处理
            if (crc & 0x80) {  // 检查最高位是否为1
                crc = (crc << 1) ^ polynomial;  // 左移并与多项式异或
            } else {
                crc <<= 1;  // 仅左移
            }
        }
    }
 
    return crc;
}
 
//数字转ASCII
char SCD30_convertHexToASCII(uint8_t digit)
{
  if (digit <= 9)
    return ( (char)(digit + 0x30) );
  else
    return ( (char)(digit + 0x41 - 10) ); // Use upper case for A-F
}
 
//当浓度数据可以被读取时返回1
uint8_t SCD30_getDataReadyStatus(void)
{
  uint16_t response;
  uint8_t success = SCD30_ReadRegister(Get_data_ready_status, &response, 10);
	//OLED_ShowHexNum(4,1,response,4);
  if (success == 0)
    return (0);
 
  // 0 至 11 bit 是 0 --> data not ready
  //else → data ready for read-out
  if ((response & 0x07ff) == 0x0000)
    return (0);
  return (1);
}
//停止连续测量
uint8_t SCD30_stopPeriodicMeasurement(uint16_t delayTime)
{
	uint8_t success = SCD30_Write_without_arguments(0x0104);
	if(success)
	{
		periodic_Measurements_AreRunning = 0;
		if (delayTime > 0)
    delay_ms(delayTime);
		return 1;
	}
	if (delayTime > 0)
    delay_ms(delayTime);
	return 0;
}
 
//软复位
uint8_t SCD30_reInit(uint16_t delayTime)
{
	if (periodic_Measurements_AreRunning)
  {
    return 0; // 需停止周期测量
  }
 
	uint8_t success = SCD30_Write_without_arguments(Soft_reset);
	if (delayTime > 0)
    delay_ms(delayTime);
	return 1;
}
 
//激活测量
uint8_t SCD30_StartPeriodicMeasurement(void)
{
	uint8_t success = 0;
	if (periodic_Measurements_AreRunning)
  {
		#if SCD4x_ENABLE_DEBUGLOG
		OLED_ShowString(4,1,"measurements running");
		#endif //
		return 1;
	}
	
	success = SCD30_Write_with_arguments(0x0010,0x03F5);
	if(success)
	{
		periodic_Measurements_AreRunning = 1;
		//OLED_ShowString(2,1,"Auto success");
	}
	return 1;
}
 
uint8_t SCD30_readMeasurement(uint16_t *co2)
{
	uint8_t error = 0;
	uint8_t buffer[12] = {0};
	uint8_t CRC1;
	uint8_t CRC2;
	uint8_t CRC3;
	uint8_t CRC4;
	uint8_t CRC5;
	uint8_t CRC6;
	
	//float co2Concentration;
	unsigned int tempU32;
	
	//if(SCD30_getDataReadyStatus() == 0)
	//{OLED_ShowString(3,1,"Data not ready");return 0;}
	//else
		//OLED_ShowString(3,1,"Data     ready");
	SCD30_SensorData.tempCO2.unsigned16 = 0;
	SCD30_SensorData.tempHumidity.unsigned16 = 0;
	SCD30_SensorData.tempTemperature.unsigned16 = 0;
	
	IIC_Start();
  IIC_Send_Byte( (SCD30_ADDRESS<<1)&(~0x01) ); //I2C Address + write
	delay_ms(50);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x0300 >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(0x0300 & 0xFF); //LSB
	error = IIC_Wait_Ack();
	IIC_Stop();
	
	if ( error )
    return 0; //Sensor did not ACK
	
	delay_ms(10);
 
	IIC_Start();
  IIC_Send_Byte( (SCD30_ADDRESS<<1)|(0x01) ); //I2C Address + read
	delay_ms(50);
	IIC_Wait_Ack();
	buffer[0] = IIC_Read_Byte(1);	//CO2
	buffer[1] = IIC_Read_Byte(1);
	CRC1 = IIC_Read_Byte(1);
	buffer[2] = IIC_Read_Byte(1);
	buffer[3] = IIC_Read_Byte(1);
	CRC2 = IIC_Read_Byte(1);			//CO2
	buffer[4] = IIC_Read_Byte(1); //T
	buffer[5] = IIC_Read_Byte(1);
	CRC3 = IIC_Read_Byte(1);
	buffer[6] = IIC_Read_Byte(1);
	buffer[7] = IIC_Read_Byte(1);
	CRC4 = IIC_Read_Byte(1);			//T
	buffer[8] = IIC_Read_Byte(1);	//RH
	buffer[9] = IIC_Read_Byte(1);
	CRC5 = IIC_Read_Byte(1);
	buffer[10] = IIC_Read_Byte(1);
	buffer[11] = IIC_Read_Byte(1);
	CRC6 = IIC_Read_Byte(0);			//RH
	IIC_Stop();
	
	 
	if(SCD30_getDataReadyStatus() == 0) 
	{
		//OLED_ShowString(4,10,"success");
	}
	if(SCD30_getDataReadyStatus() == 1) 
	{
		//OLED_ShowString(4,10,"error");
	}
	 uint8_t dataM[2];
	 uint8_t dataL[2];
	 
	 dataM[0]=buffer[0];
	 dataM[1]=buffer[1];
	 dataL[0]=buffer[2];
	 dataL[1]=buffer[3];
	 
//	 OLED_ShowHexNum(4,1,buffer[0],2);
//	 OLED_ShowHexNum(4,3,buffer[1],2);
//	 OLED_ShowHexNum(4,5,CRC1,2);
//	 OLED_ShowHexNum(4,7,buffer[2],2);
//	 OLED_ShowHexNum(4,9,buffer[3],2);
//	 OLED_ShowHexNum(4,11,CRC2,2);
	 
	 
	 if(CRC1 == calculate_crc(dataM,2) && CRC2 == calculate_crc(dataL,2))
	 {
		 tempU32  = (unsigned int)((((unsigned int)dataM[0]) << 24) |
																						(((unsigned int)dataM[1]) << 16) |
																						(((unsigned int)dataL[0]) << 8) |
																						((unsigned int)dataL[1]));
			*co2 = *(float*)&tempU32;
			return 1;
	 }
	 else
		 return 0;
		 
}
 
 
//写入不带命令
uint8_t SCD30_Write_without_arguments(uint16_t command)
{
	uint8_t error = 0;
	IIC_Start();
	IIC_Send_Byte((SCD30_ADDRESS<<1)&(~0x01));
	delay_ms(50);
	IIC_Wait_Ack();
	IIC_Send_Byte(command >> 8);   //MSB
	IIC_Wait_Ack();
	IIC_Send_Byte(command & 0xFF); //LSB
	error = IIC_Wait_Ack();
	IIC_Stop();
	if ( error )
    return 0; //Sensor did not ACK
 
  return 1;
}
//写入带命令
uint8_t SCD30_Write_with_arguments(uint16_t command, uint16_t arguments)
{
	uint8_t data[2];
	uint8_t error = 0;
  data[0] = arguments >> 8;
  data[1] = arguments & 0xFF;
 
	uint8_t crc = calculate_crc(data,2);
	IIC_Start();
	IIC_Send_Byte( (SCD30_ADDRESS<<1)&(~0x01) );
	delay_ms(50);
	IIC_Wait_Ack();
  IIC_Send_Byte(command >> 8);     //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(command & 0xFF);   //LSB
	IIC_Wait_Ack();
  IIC_Send_Byte(arguments >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(arguments & 0xFF); //LSB
	IIC_Wait_Ack();
  IIC_Send_Byte(crc); //CRC
	error = IIC_Wait_Ack();
	IIC_Stop();
	
  if ( error )
    return (0); //Sensor did not ACK
 
  return (1);
}
//读取
uint8_t SCD30_ReadRegister(uint16_t registerAddress, uint16_t *response, uint16_t delayTime)
{
	uint8_t error = 0;
	uint8_t crc = 0;
	uint8_t data[2] = {0};
	
	IIC_Start();
  IIC_Send_Byte( (SCD30_ADDRESS<<1)&(~0x01) ); //I2C Address + write
	delay_ms(30);
	IIC_Wait_Ack();
	IIC_Send_Byte(registerAddress >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(registerAddress & 0xFF); //LSB
	error = IIC_Wait_Ack();
	IIC_Stop();
	
	if ( error )
    return 0; //Sensor did not ACK
	
	delay_ms(delayTime);
 
	IIC_Start();
  IIC_Send_Byte( (SCD30_ADDRESS<<1)|(0x01) ); //I2C Address + read
	delay_ms(30);
	IIC_Wait_Ack();
	
	data[0] = IIC_Read_Byte(1);
	data[1] = IIC_Read_Byte(1);
	crc = IIC_Read_Byte(0);
	IIC_Stop();
	*response = (uint16_t)data[0] << 8 | data[1];
	uint8_t expectedCRC = calculate_crc(data,2);
	
	if (crc == expectedCRC) // Return true if CRC check is OK	
      return 1;
	return 0;
}
