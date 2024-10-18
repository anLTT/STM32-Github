#include "main.h"
#include "i2c.h"
 
#define AT24CXX_ADDR_WRITE  0xA0    // 写命令
#define AT24CXX_ADDR_READ   0xA1    // 读命令
 
#define AT24C1		127
#define AT24C2		255
#define AT24C4		511
#define AT24C8		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64		8189
#define AT24C128	16383
#define AT24C256	32767
 
#define BufferSize 	AT24C32//芯片型号
  
// 根据BufferSize宏的值来设置I2C_MEMADD_SIZE
#if BufferSize == AT24C64  
    #define I2C_MEMADD_SIZE I2C_MEMADD_SIZE_16BIT  
#elif BufferSize == AT24C02  
    #define I2C_MEMADD_SIZE I2C_MEMADD_SIZE_8BIT  
#elif BufferSize == AT24C32  
    #define I2C_MEMADD_SIZE I2C_MEMADD_SIZE_16BIT  
    
#endif
 
uint8_t At24c02_Write_Byte(uint16_t addr, uint8_t* dat);
uint8_t At24c02_Read_Byte(uint16_t addr, uint8_t* read_buf);
uint8_t At24c02_Write_Amount_Byte(uint16_t addr, uint8_t* dat, uint16_t size);
uint8_t At24c02_Read_Amount_Byte(uint16_t addr, uint8_t* recv_buf, uint16_t size);

