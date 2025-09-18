#ifndef MY_SCD4x_CO2_H
#define MY_SCD4x_CO2_H

#include "myiic.h"
#include "delay.h"
#include <stdbool.h>

#define SCD4x_ENABLE_DEBUGLOG 0 //debug SCD4x

//The default I2C address for the SCD4x is 0x62.
#define SCD4x_ADDRESS 0x62

//Available commands

//Basic Commands
#define SCD4x_COMMAND_START_PERIODIC_MEASUREMENT              0x21b1
#define SCD4x_COMMAND_READ_MEASUREMENT                        0xec05 // execution time: 1ms
#define SCD4x_COMMAND_STOP_PERIODIC_MEASUREMENT               0x3f86 // execution time: 500ms

//On-chip output signal compensation
#define SCD4x_COMMAND_SET_TEMPERATURE_OFFSET                  0x241d // execution time: 1ms
#define SCD4x_COMMAND_GET_TEMPERATURE_OFFSET                  0x2318 // execution time: 1ms
#define SCD4x_COMMAND_SET_SENSOR_ALTITUDE                     0x2427 // execution time: 1ms
#define SCD4x_COMMAND_GET_SENSOR_ALTITUDE                     0x2322 // execution time: 1ms
#define SCD4x_COMMAND_SET_AMBIENT_PRESSURE                    0xe000 // execution time: 1ms

//Field calibration
#define SCD4x_COMMAND_PERFORM_FORCED_CALIBRATION              0x362f // execution time: 400ms
#define SCD4x_COMMAND_SET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2416 // execution time: 1ms
#define SCD4x_COMMAND_GET_AUTOMATIC_SELF_CALIBRATION_ENABLED  0x2313 // execution time: 1ms

//Low power
#define SCD4x_COMMAND_START_LOW_POWER_PERIODIC_MEASUREMENT    0x21ac
#define SCD4x_COMMAND_GET_DATA_READY_STATUS                   0xe4b8 // execution time: 1ms

//Advanced features
#define SCD4x_COMMAND_PERSIST_SETTINGS                        0x3615 // execution time: 800ms
#define SCD4x_COMMAND_GET_SERIAL_NUMBER                       0x3682 // execution time: 1ms
#define SCD4x_COMMAND_PERFORM_SELF_TEST                       0x3639 // execution time: 10000ms
#define SCD4x_COMMAND_PERFORM_FACTORY_RESET                   0x3632 // execution time: 1200ms
#define SCD4x_COMMAND_REINIT                                  0x3646 // execution time: 20ms

//Low power single shot - SCD41 only
#define SCD4x_COMMAND_MEASURE_SINGLE_SHOT                     0x219d // execution time: 5000ms
#define SCD4x_COMMAND_MEASURE_SINGLE_SHOT_RHT_ONLY            0x2196 // execution time: 50ms

typedef union
{
  int16_t signed16;
  uint16_t unsigned16;
} scd4x_int16_uint16_t; // Avoid any ambiguity casting int16_t to uint16_t

typedef union
{
  uint16_t unsigned16;
  uint8_t bytes[2];
} scd4x_uint16_Bytes_t; // Make it easy to convert 2 x uint8_t to uint16_t

typedef struct
{
	scd4x_uint16_Bytes_t tempCO2;
	scd4x_uint16_Bytes_t tempHumidity;
	scd4x_uint16_Bytes_t tempTemperature;
}scd4x_SensorData_t;

extern scd4x_SensorData_t SCD4x_SensorData; //save SCD4x sensor data

bool SCD4x_StartPeriodicMeasurement(void);
bool SCD4x_stopPeriodicMeasurement(uint16_t delayTime); // execution time: 500ms
bool SCD4x_readMeasurement(uint16_t *co2, float *temperature, float *humidity, uint8_t *Error);
bool SCD4x_getSerialNumber(char *serialNumber); //return 13 byte string

bool SCD4x_performSelfTest(void);  // executed time 10000 ms
bool SCD4x_performFactoryReset(uint16_t delayTime); // executed time 1200 ms
bool SCD4x_reInit(uint16_t delayTime);   // executed time 20 ms
bool SCD4x_getDataReadyStatus(void);  // executed time 1 ms

bool SCD4x_sendCommand(uint16_t command);
bool SCD4x_sendCommand_CmdData_CRC(uint16_t command, uint16_t arguments);
bool SCD4x_readRegister(uint16_t registerAddress, uint16_t *response, uint16_t delayTime);

char SCD4x_convertHexToASCII(uint8_t digit);
uint8_t SCD4x_computeCRC8(uint8_t data[], uint8_t len); //CRC8ะฃั้

#endif
