#include "My_SCD4x-CO2.h"

uint8_t periodic_Measurements_AreRunning = 0;
scd4x_SensorData_t SCD4x_SensorData;

//Start periodic measurements. See "SCD40_SCD41_Datasheet.pdf" 3.5.1
//signal update interval is 5 seconds.
bool SCD4x_StartPeriodicMeasurement(void)
{
	bool success = 0;
	
  if (periodic_Measurements_AreRunning)
  {
    #if SCD4x_ENABLE_DEBUGLOG

     printf("SCD4x_StartPeriodicMeasurement: periodic measurements are already running");
		
    #endif // if SCD4x_ENABLE_DEBUGLOG
    return (true);  //Maybe this should be false?
  }

  success = SCD4x_sendCommand(SCD4x_COMMAND_START_PERIODIC_MEASUREMENT);
  if (success)
    periodic_Measurements_AreRunning = true;
  return (success);
}

// Stop periodic measurements. See "SCD40_SCD41_Datasheet.pdf" 3.5.3
// 传感器在发出 STOP_PERIONAL_MEASURATION 命令后等待500毫秒后才会对其他命令作出响应。
bool SCD4x_stopPeriodicMeasurement(uint16_t delayTime)
{
	
//	if (periodic_Measurements_AreRunning == 0)
//  {
//    return (false);
//  }

  bool success = SCD4x_sendCommand(SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT);
	
	if(success)
	{
		periodic_Measurements_AreRunning = false;
		if (delayTime > 0)
    delay_ms(delayTime);
		return true;
	}
  if (delayTime > 0)
    delay_ms(delayTime);
	
  return (false);
}

// Get 9 bytes from SCD4x. See 3.5.2
// Read sensor output.
// 每个信号更新间隔只能读出一次测量数据，因为读数时缓冲区被清空
// 如果缓冲区中没有可用数据，传感器将返回 NACK。
bool SCD4x_readMeasurement(uint16_t *co2, float *temperature, float *humidity, uint8_t *Error)
{
	uint8_t error = 0;
	uint8_t buffer[10] = {0};

  if (SCD4x_getDataReadyStatus() == false) // get_data_ready_status. see "SCD40_SCD41_Datasheet.pdf" 3.8.2
    return (false);

	SCD4x_SensorData.tempCO2.unsigned16 = 0;
	SCD4x_SensorData.tempHumidity.unsigned16 = 0;
	SCD4x_SensorData.tempTemperature.unsigned16 = 0;

	IIC_Start();
  IIC_Send_Byte( (SCD4x_ADDRESS<<1)&(~0x01)); //I2C Address + write
	IIC_Wait_Ack();
  IIC_Send_Byte(SCD4x_COMMAND_READ_MEASUREMENT >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(SCD4x_COMMAND_READ_MEASUREMENT & 0xFF); //LSB
	error = IIC_Wait_Ack();
	
  if ( error )
    return (false); //Sensor did not ACK

	error = 0;
  delay_ms(1); //Datasheet specifies this,  command execution time

	IIC_Start();
  IIC_Send_Byte( (SCD4x_ADDRESS<<1)|(0x01)); //I2C Address + read
	IIC_Wait_Ack();
	buffer[0] = IIC_Read_Byte(1); //read data and ack
	buffer[1] = IIC_Read_Byte(1);
	buffer[2] = IIC_Read_Byte(1);
	buffer[3] = IIC_Read_Byte(1);
	buffer[4] = IIC_Read_Byte(1);
	buffer[5] = IIC_Read_Byte(1);
	buffer[6] = IIC_Read_Byte(1);
	buffer[7] = IIC_Read_Byte(1);
	buffer[8] = IIC_Read_Byte(0); //read data and no ack
	IIC_Stop();

	uint8_t foundCrc = 0;
  uint8_t bytesToCrc[2]={0};
    for (uint8_t x = 0; x < 9; x++)
    {
      switch (x)
      {
      case 0:
      case 1:
        SCD4x_SensorData.tempCO2.bytes[x == 0 ? 1 : 0] = buffer[x]; // Store the two CO2 bytes in little-endian format
        bytesToCrc[x] = buffer[x]; // Calculate the CRC on the two CO2 bytes in the order they arrive
        break;
      case 3:
      case 4:
        SCD4x_SensorData.tempTemperature.bytes[x == 3 ? 1 : 0] = buffer[x]; // Store the two T bytes in little-endian format
        bytesToCrc[x % 3] = buffer[x]; // Calculate the CRC on the two T bytes in the order they arrive
        break;
      case 6:
      case 7:
        SCD4x_SensorData.tempHumidity.bytes[x == 6 ? 1 : 0] = buffer[x]; // Store the two RH bytes in little-endian format
        bytesToCrc[x % 3] = buffer[x]; // Calculate the CRC on the two RH bytes in the order they arrive
        break;
      default: // x == 2, 5, 8
        //Validate CRC
        foundCrc = SCD4x_computeCRC8(bytesToCrc, 2); // Calculate what the CRC should be for these two bytes
        if (foundCrc != buffer[x]) // CRC check error
        {
          #if SCD4x_ENABLE_DEBUGLOG
						
          #endif // if SCD4x_ENABLE_DEBUGLOG
					if(x==2)
						{error |= 0x01;} // CO2
					if(x==5)
						{error |= 0x02;} // Temperature
					if(x==8)
						{error |= 0x04;} // Humidity
        }
        break;
      }
    }

  if (error)
  {
		*Error = error;
    #if SCD4x_ENABLE_DEBUGLOG
    
    printf("SCD4x_readMeasurement: encountered error reading SCD4x data.");
    #endif // if SCD4x_ENABLE_DEBUGLOG
    return (false);
  }
  // 返回解析后的数据
  *co2 = (uint16_t)SCD4x_SensorData.tempCO2.unsigned16; //ppm
  *temperature = -45 + (((float)SCD4x_SensorData.tempTemperature.unsigned16) * 175 / 65536);
  *humidity = ((float)SCD4x_SensorData.tempHumidity.unsigned16) * 100 / 65536;
	
  return (true); //Success! 
}



//Get 9 bytes from SCD4x. Convert 48-bit serial number to ASCII chars. See 3.9.2
//Returns true if serial number is read successfully
//Reading out the serial number can be used to identify the chip and to verify the presence of the sensor.
bool SCD4x_getSerialNumber(char *serialNumber)
{
	uint8_t error = 0;
	uint8_t Crc = 0;
	
  if (periodic_Measurements_AreRunning)
  {
    #if SCD4x_ENABLE_DEBUGLOG
      printf("SCD4x_getSerialNumber: periodic measurements are running. Aborting");
    #endif // if SCD4x_ENABLE_DEBUGLOG
    return (false);
  }

	IIC_Start();
  IIC_Send_Byte( (SCD4x_ADDRESS<<1)&(~0x01)); //I2C Address + write
	IIC_Wait_Ack();
  IIC_Send_Byte(SCD4x_COMMAND_GET_SERIAL_NUMBER >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(SCD4x_COMMAND_GET_SERIAL_NUMBER & 0xFF); //LSB
	error = IIC_Wait_Ack();
	
  if ( error )
    return (false); //Sensor did not ACK

  delay_ms(1); //Datasheet specifies this

  IIC_Start();
  IIC_Send_Byte( (SCD4x_ADDRESS<<1)|(0x01)); //I2C Address + read
	error = IIC_Wait_Ack();

  if (error==0)
  {
    uint8_t bytesToCrc[2];
    int digit = 0;
    for (uint8_t  x = 0; x < 9; x++)
    {
      uint8_t incoming = IIC_Read_Byte(1); //read data and ack
			if(x==8)
			{
				incoming = IIC_Read_Byte(0);  //read data and no ack
				IIC_Stop();
			}
      switch (x)
      {
      case 0: // The serial number arrives as: two bytes, CRC, two bytes, CRC, two bytes, CRC
      case 1:
      case 3:
      case 4:
      case 6:
      case 7:
        serialNumber[digit++] = SCD4x_convertHexToASCII(incoming >> 4); // Convert each nibble to ASCII
        serialNumber[digit++] = SCD4x_convertHexToASCII(incoming & 0x0F);
        bytesToCrc[x % 3] = incoming;
        break;
      default: // x == 2, 5, 8
        Crc = SCD4x_computeCRC8(bytesToCrc, 2); // Calculate what the CRC should be for these two bytes
        if (Crc != incoming) // Does this match the CRC byte from the sensor?
        {
          #if SCD4x_ENABLE_DEBUGLOG

          #endif // if SCD4x_ENABLE_DEBUGLOG
          error = true;
        }
        break;
      }
      serialNumber[digit] = 0; // NULL-terminate the string
    }
  }

  if (error)
  {
    #if SCD4x_ENABLE_DEBUGLOG
      printf("SCD4x_readSerialNumber: encountered error reading SCD4x data.");
    #endif // if SCD4x_ENABLE_DEBUGLOG
    return (false);
  }

  return (true); //Success!
}



//Perform self test. Takes 10 seconds to complete. See 3.9.3
//The perform_self_test feature can be used as an end-of-line test to check sensor functionality
//and the customer power supply to the sensor.
bool SCD4x_performSelfTest(void)
{
  if (periodic_Measurements_AreRunning)
  {
    #if SCD4x_ENABLE_DEBUGLOG
			
    #endif // if SCD4x_ENABLE_DEBUGLOG
    return (false);
  }

  uint16_t response;

  #if SCD4x_ENABLE_DEBUGLOG
    printf("SCD4x_performSelfTest: delaying for 10 seconds...");
  #endif // if SCD4x_ENABLE_DEBUGLOG

  bool success = SCD4x_readRegister(SCD4x_COMMAND_PERFORM_SELF_TEST, &response, 10000);

  return (success && (response == 0x0000)); // word[0] = 0 → no malfunction detected
}

//Peform factory reset. See 3.9.4
//The perform_factory_reset command resets all configuration settings stored in the EEPROM
//and erases the FRC and ASC algorithm history.
bool SCD4x_performFactoryReset(uint16_t delayTime)
{
  if (periodic_Measurements_AreRunning)
  {
    #if SCD4x_ENABLE_DEBUGLOG

    #endif // if SCD4x_ENABLE_DEBUGLOG
    return (false);
  }

  bool success = SCD4x_sendCommand(SCD4x_COMMAND_PERFORM_FACTORY_RESET);
  if (delayTime > 0)
    delay_ms(delayTime);
  return (success);
}

//Reinit. See 3.9.5
//The reinit command reinitializes the sensor by reloading user settings from EEPROM.
//Before sending the reinit command, the stop measurement command must be issued.
//If the reinit command does not trigger the desired re-initialization,a power-cycle should be applied to the SCD4x.
bool SCD4x_reInit(uint16_t delayTime)
{
  if (periodic_Measurements_AreRunning)
  {
    return (false); // 需停止周期测量
  }

  bool success = SCD4x_sendCommand(SCD4x_COMMAND_REINIT);
  if (delayTime > 0)
    delay_ms(delayTime);
  return (success);
}

//Sends just a command, no arguments, no CRC
// see "SCD40_SCD41_Datasheet.pdf 3.3", "send command sequence"
bool SCD4x_sendCommand(uint16_t command)
{
	uint8_t error = 0;
	
	IIC_Start();
  IIC_Send_Byte((SCD4x_ADDRESS<<1)&(~0x01));
	IIC_Wait_Ack();
  IIC_Send_Byte(command >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(command & 0xFF); //LSB
	error = IIC_Wait_Ack();
	IIC_Stop();
	
  if ( error )
    return (false); //Sensor did not ACK

  return (true);
}

// Sends a command along with arguments and CRC
// see "SCD40_SCD41_Datasheet.pdf 3.3", "write sequence"
bool SCD4x_sendCommand_CmdData_CRC(uint16_t command, uint16_t arguments)
{
  uint8_t data[2];
	uint8_t error = 0;
  data[0] = arguments >> 8;
  data[1] = arguments & 0xFF;
  uint8_t crc = SCD4x_computeCRC8(data, 2); //Calc CRC on the arguments only, not the command

	IIC_Start();
	IIC_Send_Byte( (SCD4x_ADDRESS<<1)&(~0x01) );
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
    return (false); //Sensor did not ACK

  return (true);
}



// Read one uint16 data from SCD4x plus CRC. see "SCD40_SCD41_Datasheet.pdf 3.3", "read sequence"
// Returns true if IIC_Wait_Ack returns 0 _and_ the CRC check is valid
bool SCD4x_readRegister(uint16_t registerAddress, uint16_t *response, uint16_t delayTime)
{
	uint8_t error = 0;
	uint8_t crc = 0;
	uint8_t data[2] = {0};
	
	IIC_Start();
  IIC_Send_Byte( (SCD4x_ADDRESS<<1)&(~0x01) ); //I2C Address + write
	IIC_Wait_Ack();
  IIC_Send_Byte(registerAddress >> 8);   //MSB
	IIC_Wait_Ack();
  IIC_Send_Byte(registerAddress & 0xFF); //LSB
	error = IIC_Wait_Ack();
	IIC_Stop();
	
  if ( error )
    return (false); //Sensor did not ACK

  delay_ms(delayTime);
	
	IIC_Start();
  IIC_Send_Byte( (SCD4x_ADDRESS<<1)|(0x01) ); //I2C Address + read
	IIC_Wait_Ack();
	data[0] = IIC_Read_Byte(1);
	data[1] = IIC_Read_Byte(1);
	crc = IIC_Read_Byte(0);
	IIC_Stop();
	
  *response = (uint16_t)data[0] << 8 | data[1];
  uint8_t expectedCRC = SCD4x_computeCRC8(data, 2);
    if (crc == expectedCRC) // Return true if CRC check is OK
      return (true);
  
  return (false);
}

//Returns true when data is available. See 3.8.2
bool SCD4x_getDataReadyStatus(void)
{
  uint16_t response;
  bool success = SCD4x_readRegister(SCD4x_COMMAND_GET_DATA_READY_STATUS, &response, 1);

  if (success == false)
    return (false);

  // 0 至 11 bit 是 0 --> data not ready
  //else → data ready for read-out
  if ((response & 0x07ff) == 0x0000)
    return (false);
  return (true);
}

//PRIVATE: Convert serial number digit to ASCII
char SCD4x_convertHexToASCII(uint8_t digit)
{
  if (digit <= 9)
    return ( (char)(digit + 0x30) );
  else
    return ( (char)(digit + 0x41 - 10) ); // Use upper case for A-F
}

//Given an array and a number of bytes, this calculate CRC8 for those bytes
//From: http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
//Tested with: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
//x^8+x^5+x^4+1 = 0x31
uint8_t SCD4x_computeCRC8(uint8_t data[], uint8_t len)
{
  uint8_t crc = 0xFF; //Init with 0xFF

  for (uint8_t x = 0; x < len; x++)
  {
    crc ^= data[x]; // XOR-in the next input byte

    for (uint8_t i = 0; i < 8; i++)
    {
      if ((crc & 0x80) != 0)
        crc = (uint8_t)((crc << 1) ^ 0x31);
      else
        crc <<= 1;
    }
  }
	
  return crc; //No output reflection
}


/*
			end
*/
