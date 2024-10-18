#include "main.h"
#include "IIC.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_rcc.h"
#include "stm32f1xx_hal.h"

#define Delay() delay_us(2)
#define AT24C32_ADDRESS 0xA0

void IIC_Start(void);
void IIC_Stop(void);
void IIC_ACK(uint8_t ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Write_Byte(uint8_t Data);
uint8_t IIC_Read_Byte(void);

void I2C_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(IO_EE_SCL_GPIO_Port, IO_EE_SCL_Pin | IO_EE_SDA_Pin, GPIO_PIN_SET);

    /*Configure GPIO pins : PB10 PB11 */
    GPIO_InitStruct.Pin = IO_EE_SCL_Pin | IO_EE_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(IO_EE_SCL_GPIO_Port, &GPIO_InitStruct);
}
/*设置相应数据的引脚为输出*/
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // PB11-SDA
    GPIO_InitStructure.Pin = IO_EE_SDA_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IO_EE_SCL_GPIO_Port, &GPIO_InitStructure);
}
/*设置相应数据的引脚为输入*/
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    // PB11-SDA
    GPIO_InitStructure.Pin = IO_EE_SDA_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(IO_EE_SCL_GPIO_Port, &GPIO_InitStructure);
}

/*us级别延迟函数*/
void delay_us(uint32_t us) // 主频72M
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--)
    {
        ;
    }
}
/**********************************************************************************/
/*                                      END                                       */
/**********************************************************************************/

/**********************************************************************************/
/*                                iic协议部分                                     */
/**********************************************************************************/
/*iic起始信号*/
void IIC_Start(void)
{
    SDA_OUT(); // sda线输出
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
    Delay();
    IIC_SDA_LOW(); // START:when CLK is high,DATA change form high to low
    Delay();
    IIC_SCL_LOW(); // 钳住I2C总线，准备发送或接收数据
}

/*iic停止信号*/
void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SDA_LOW(); // STOP:when CLK is high DATA change form low to high
    IIC_SCL_HIGH();
    Delay();
    IIC_SDA_HIGH();
    Delay();
}

/*iic产生应答信号*/
// 1:nack 0:ack
void IIC_ACK(uint8_t ack)
{
    SDA_OUT();
    IIC_SCL_LOW(); // 时钟线拉低，以便写数据
    if (ack)
        IIC_SDA_HIGH(); // 写应答信号
    else
        IIC_SDA_LOW();
    IIC_SCL_HIGH(); // 拉高时钟线
    Delay();        // 延时
    IIC_SCL_LOW();  // 拉低时钟线
    Delay();        // 延时
}

/*iic等待应答信号*/
uint8_t IIC_Wait_Ack(void)
{
    uint16_t wait_time = 0;
    SDA_IN();
    IIC_SCL_HIGH();
    Delay();
    while (IIC_SDA_READ())
    {
        wait_time++;
        if (wait_time >= 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_LOW();
    return 0;
}

/*iic写一个字节数据*/
void IIC_Write_Byte(uint8_t Data)
{
    uint8_t t = 0, bit;
    SDA_OUT();
    IIC_SCL_LOW();
    for (t = 0; t < 8; t++)
    {
        bit = Data & 0x80;
        if (bit)
            IIC_SDA_HIGH();
        else
            IIC_SDA_LOW();
        Data <<= 1;
        IIC_SCL_HIGH(); // 拉高时钟线
        Delay();        // 延时
        IIC_SCL_LOW();  // 拉低时钟线
        Delay();        // 延时
    }
    IIC_Wait_Ack();
}

/*iic读一个字节数据*/
uint8_t IIC_Read_Byte(void)
{
    uint8_t i;
    uint8_t dat = 0;
    SDA_IN();               // SDA设置为输入
    IIC_SDA_HIGH();         // 使能内部上拉,准备读取数据,
    for (i = 0; i < 8; i++) // 8位计数器
    {
        dat <<= 1;
        IIC_SCL_HIGH(); // 拉高时钟线
        Delay();        // 延时
        if (IIC_SDA_READ())
            dat += 1;
        IIC_SCL_LOW(); // 拉低时钟线
        Delay();       // 延时
    }
    return dat;
}

void AT24C32_WriteByte(uint16_t MemAddress, uint8_t* pData)
{
    IIC_Start();                      // 启动信号
    IIC_Write_Byte(AT24C32_ADDRESS);  // 发送 AT24C32 的地址（带写位）
    IIC_Write_Byte((uint8_t)(MemAddress >> 8)); // 发送高 8 位地址
    IIC_Write_Byte((uint8_t)MemAddress);        // 发送低 8 位地址
    IIC_Write_Byte(*pData);            // 发送数据
    IIC_Stop();                       // 停止信号
}

void AT24C32_ReadByte(uint16_t MemAddress, uint8_t* pData)
{
    IIC_Start();                      // 启动信号
    IIC_Write_Byte(AT24C32_ADDRESS);  // 发送 AT24C32 的地址（带写位）
    IIC_Write_Byte((uint8_t)(MemAddress >> 8)); // 发送高 8 位地址
    IIC_Write_Byte((uint8_t)MemAddress);        // 发送低 8 位地址
    
    IIC_Start();                      // 重复启动信号
    IIC_Write_Byte(AT24C32_ADDRESS | 0x01);  // 发送 AT24C32 的地址（带读位）
    *pData = IIC_Read_Byte();         // 读取数据
    IIC_Stop();                       // 停止信号
}
/**********************************************************************************/
/*                                      END                                       */
/**********************************************************************************/
