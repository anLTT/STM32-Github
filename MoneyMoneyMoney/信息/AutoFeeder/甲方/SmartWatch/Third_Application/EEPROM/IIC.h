#ifndef __IIC_H
#define __IIC_H
 
/*GPIO初始化*/
void I2C_GPIO_Init(void);
 
/*GPIO输出*/
#define IIC_SDA_HIGH() GPIOB->ODR |=  (1 << 11)
#define IIC_SDA_LOW()  GPIOB->ODR &= ~(1 << 11)
#define IIC_SCL_HIGH() GPIOB->ODR |=  (1 << 10)
#define IIC_SCL_LOW()  GPIOB->ODR &= ~(1 << 10)
 
/*GPIO读取数据*/
#define IIC_SDA_READ() GPIOB->IDR &= (1<<11)

extern void AT24C32_WriteByte(uint16_t MemAddress, uint8_t* pData);
extern void AT24C32_ReadByte(uint16_t MemAddress, uint8_t* pData);

#endif
