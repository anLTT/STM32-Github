#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "stdint.h"
#include "Delay.h"
//#include "OLED.h"
#include "SCD30.h"
#define SCD30_ADDRESS		0x61
uint8_t periodic_Measurements_AreRunning=0;
#define SCD30_ENABLE_DEBUGLOG 0 


 
static const unsigned char crc_table[] =
{
0x00,0x31,0x62,0x53,0xc4,0xf5,0xa6,0x97,0xb9,0x88,0xdb,0xea,0x7d,0x4c,0x1f,0x2e,
0x43,0x72,0x21,0x10,0x87,0xb6,0xe5,0xd4,0xfa,0xcb,0x98,0xa9,0x3e,0x0f,0x5c,0x6d,
0x86,0xb7,0xe4,0xd5,0x42,0x73,0x20,0x11,0x3f,0x0e,0x5d,0x6c,0xfb,0xca,0x99,0xa8,
0xc5,0xf4,0xa7,0x96,0x01,0x30,0x63,0x52,0x7c,0x4d,0x1e,0x2f,0xb8,0x89,0xda,0xeb,
0x3d,0x0c,0x5f,0x6e,0xf9,0xc8,0x9b,0xaa,0x84,0xb5,0xe6,0xd7,0x40,0x71,0x22,0x13,
0x7e,0x4f,0x1c,0x2d,0xba,0x8b,0xd8,0xe9,0xc7,0xf6,0xa5,0x94,0x03,0x32,0x61,0x50,
0xbb,0x8a,0xd9,0xe8,0x7f,0x4e,0x1d,0x2c,0x02,0x33,0x60,0x51,0xc6,0xf7,0xa4,0x95,
0xf8,0xc9,0x9a,0xab,0x3c,0x0d,0x5e,0x6f,0x41,0x70,0x23,0x12,0x85,0xb4,0xe7,0xd6,
0x7a,0x4b,0x18,0x29,0xbe,0x8f,0xdc,0xed,0xc3,0xf2,0xa1,0x90,0x07,0x36,0x65,0x54,
0x39,0x08,0x5b,0x6a,0xfd,0xcc,0x9f,0xae,0x80,0xb1,0xe2,0xd3,0x44,0x75,0x26,0x17,
0xfc,0xcd,0x9e,0xaf,0x38,0x09,0x5a,0x6b,0x45,0x74,0x27,0x16,0x81,0xb0,0xe3,0xd2,
0xbf,0x8e,0xdd,0xec,0x7b,0x4a,0x19,0x28,0x06,0x37,0x64,0x55,0xc2,0xf3,0xa0,0x91,
0x47,0x76,0x25,0x14,0x83,0xb2,0xe1,0xd0,0xfe,0xcf,0x9c,0xad,0x3a,0x0b,0x58,0x69,
0x04,0x35,0x66,0x57,0xc0,0xf1,0xa2,0x93,0xbd,0x8c,0xdf,0xee,0x79,0x48,0x1b,0x2a,
0xc1,0xf0,0xa3,0x92,0x05,0x34,0x67,0x56,0x78,0x49,0x1a,0x2b,0xbc,0x8d,0xde,0xef,
0x82,0xb3,0xe0,0xd1,0x46,0x77,0x24,0x15,0x3b,0x0a,0x59,0x68,0xff,0xce,0x9d,0xac
};
 
unsigned char crc8scd30(unsigned char *ptr, unsigned char len)
{
		unsigned char crc = 0xFF;
 
		while (len--)
		{
				crc = crc_table[crc ^ *ptr++];
		}
		return (crc);
}

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
    Delay_ms(delayTime);
		return 1;
	}
	if (delayTime > 0)
    Delay_s(delayTime);
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
    Delay_ms(delayTime);
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
//		OLED_ShowString(2,1,"Auto success");
	}
	return 1;
}
 
uint8_t SCD30_readMeasurement(uint16_t *co2,float *temperature,uint16_t *humidity)
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
	
	if(SCD30_getDataReadyStatus() == 0)
	{//OLED_ShowString(3,1,"Data not ready");
	return 0;}
	else;
		//OLED_ShowString(3,1,"Data     ready");
	SCD30_SensorData.tempCO2.unsigned16 = 0;
	SCD30_SensorData.tempHumidity.unsigned16 = 0;
	SCD30_SensorData.tempTemperature.unsigned16 = 0;
	
	IIC_Start();
  IIC_Send_Byte( (SCD30_ADDRESS<<1)&(~0x01) ); //I2C Address + write
	Delay_ms(50);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x0300 >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(0x0300 & 0xFF); //LSB
	error = IIC_Wait_Ack();
	IIC_Stop();
	
	if ( error )
    return 0; //Sensor did not ACK
	
	Delay_ms(10);
 
	IIC_Start();
  IIC_Send_Byte( (SCD30_ADDRESS<<1)|(0x01) ); //I2C Address + read
	Delay_ms(50);
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
	
	 
	if(SCD30_getDataReadyStatus() == 1) 
	{
		//OLED_ShowString(4,10,"success");
	}
	if(SCD30_getDataReadyStatus() == 0) 
	{
		//OLED_ShowString(4,10,"error");
	}
	 uint8_t dataM[2];
	 uint8_t dataL[2];
	 
	 dataM[0]=buffer[0];
	 dataM[1]=buffer[1];
	 dataL[0]=buffer[2];
	 dataL[1]=buffer[3];
	 

	 uint8_t tempM[2] = {buffer[4], buffer[5]};
uint8_t tempL[2] = {buffer[6], buffer[7]};

uint8_t humM[2] = {buffer[8], buffer[9]};
uint8_t humL[2] = {buffer[10], buffer[11]};

//	 OLED_ShowHexNum(4,1,buffer[0],2);
//	 OLED_ShowHexNum(4,3,buffer[1],2);
//	 OLED_ShowHexNum(4,5,CRC1,2);
//	 OLED_ShowHexNum(4,7,buffer[2],2);
//	 OLED_ShowHexNum(4,9,buffer[3],2);
//	 OLED_ShowHexNum(4,11,CRC2,2);
//	 
	 //B0  B0  DD  C9(B0  B0  DD  F9)
	 if((CRC1 == calculate_crc(dataM,2) && CRC2 == calculate_crc(dataL,2))
		&&(CRC3 == calculate_crc(tempM,2) && CRC4 == calculate_crc(tempL,2))
		&&(CRC5 == calculate_crc(humM,2) && CRC6 == calculate_crc(humL,2)))	 
	 {
		 tempU32  = (unsigned int)((((unsigned int)dataM[0]) << 24) |
																						(((unsigned int)dataM[1]) << 16) |
																						(((unsigned int)dataL[0]) << 8) |
																						((unsigned int)dataL[1]));
			*co2 = *(float*)&tempU32;

			    tempU32  = (unsigned int)((((unsigned int)tempM[0]) << 24) |
                              (((unsigned int)tempM[1]) << 16) |
                              (((unsigned int)tempL[0]) << 8) |
                              ((unsigned int)tempL[1]));
    *temperature = *(float*)&tempU32;

	    tempU32  = (unsigned int)((((unsigned int)humM[0]) << 24) |
                              (((unsigned int)humM[1]) << 16) |
                              (((unsigned int)humL[0]) << 8) |
                              ((unsigned int)humL[1]));
    *humidity = *(float*)&tempU32;

			return 1;
	 }
	 else
	 {
		 return 0;
	 }
		 
}
 
 
//写入不带命令
uint8_t SCD30_Write_without_arguments(uint16_t command)
{
	uint8_t error = 0;
	IIC_Start();
	IIC_Send_Byte((SCD30_ADDRESS<<1)&(~0x01));
	Delay_ms(50);
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
	Delay_ms(50);
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
	Delay_ms(30);
	IIC_Wait_Ack();
	IIC_Send_Byte(registerAddress >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(registerAddress & 0xFF); //LSB
	error = IIC_Wait_Ack();
	IIC_Stop();
	
	if ( error )
    return 0; //Sensor did not ACK
	
	Delay_ms(delayTime);
 
	IIC_Start();
  IIC_Send_Byte( (SCD30_ADDRESS<<1)|(0x01) ); //I2C Address + read
	Delay_ms(30);
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
	
 
 