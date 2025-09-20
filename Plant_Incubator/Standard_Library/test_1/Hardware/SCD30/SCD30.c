#include "stm32f10x.h"                  // Device header
#include "SCD30.h"
#include "Delay.h"
#include "stm32f10x_i2c.h"
#define SCD30_ADDRESS       0x61

/**
 * 初始化I2C2接口（PB10=SCL, PB11=SDA）
 */
void SCD30_I2C2_Init(void)
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed =	50000;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2,&I2C_InitStructure);
	I2C_Cmd(I2C2,ENABLE);
}

/**
 * @brief  CRC-8计算（严格遵循文档1.1.3规则：多项式0x31，初始值0xFF，无反射）
 * @param  data：待校验数据（2字节，文档1.1.2要求）
 * @param  len：数据长度（固定为2）
 * @retval 计算得到的CRC值
 */
uint8_t SCD30_CalcCRC8(uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;  // 初始值0xFF（文档1.1.3）
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;  // 多项式0x31（文档1.1.3）
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;  // 最终异或0x00（文档1.1.3）
}

/**
 * 发送I2C命令（内部使用）
 * @param cmd 命令码
 * @param param 参数值
 * @param has_param 是否带参数
 * @return 0=成功, 1=失败
 */
static uint8_t SCD30_I2C2_WriteCmd(uint16_t cmd, uint16_t param, uint8_t has_param) {
    uint8_t tx_buf[5] = {0};  // 有参数时长度为5字节
    uint8_t tx_len = 2;       // 无参数时长度为2字节

    // 填充命令（高8位在前）
    tx_buf[0] = (cmd >> 8) & 0xFF;
    tx_buf[1] = cmd & 0xFF;

    // 带参数时填充参数和CRC（只对带参数的两字节进行校验和）
    if (has_param) {
        tx_buf[2] = (param >> 8) & 0xFF;
        tx_buf[3] = param & 0xFF;
        tx_buf[4] = SCD30_CalcCRC8(&tx_buf[2], 2);
        tx_len = 5;
    }

    // 发送START信号
    I2C_GenerateSTART(I2C2, ENABLE);
    uint32_t timeout = SCD30_I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (--timeout == 0) return 1;
    }

    // 发送写地址
    I2C_Send7bitAddress(I2C2, SCD30_I2C_ADDR_WRITE, I2C_Direction_Transmitter);
    timeout = SCD30_I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (--timeout == 0) return 1;
    }

    // 发送数据
    for (uint8_t i = 0; i < tx_len; i++) {
        I2C_SendData(I2C2, tx_buf[i]);
        timeout = SCD30_I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
            if (--timeout == 0) return 1;
        }
    }

    // 发送STOP信号
    I2C_GenerateSTOP(I2C2, ENABLE);
    return 0;
}

/**
 * 启动连续测量
 * @return 0=成功, 1=失败
 */
uint8_t SCD30_StartContinuousMeas(void) {
    // 启动测量，参数0表示禁用气压补偿
    return SCD30_I2C2_WriteCmd(SCD30_CMD_START_MEAS, 0x0000, 1);
}

/**
 * 检查数据是否就绪（内部使用）
 * @return 0=就绪, 1=未就绪, 2=通信失败
 */
static uint8_t SCD30_CheckDataReady(void) {
    uint8_t rx_buf[3] = {0};

    // 发送检查命令
    if (SCD30_I2C2_WriteCmd(SCD30_CMD_GET_DATA_RDY, 0, 0) != 0) {
        return 2;
    }

    Delay_ms(SCD30_DELAY_AFTER_CMD);

    // 发送START信号
    I2C_GenerateSTART(I2C2, ENABLE);
    uint32_t timeout = SCD30_I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (--timeout == 0) return 2;
    }

    // 发送读地址
    I2C_Send7bitAddress(I2C2, SCD30_I2C_ADDR_READ, I2C_Direction_Receiver);
    timeout = SCD30_I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (--timeout == 0) return 2;
    }

    // 读取3字节数据
    for (uint8_t i = 0; i < 3; i++) {
        if (i < 2) {
            I2C_AcknowledgeConfig(I2C2, ENABLE);
        } else {
            I2C_AcknowledgeConfig(I2C2, DISABLE);
            I2C_GenerateSTOP(I2C2, ENABLE);
        }

        timeout = SCD30_I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            if (--timeout == 0) return 2;
        }
        rx_buf[i] = I2C_ReceiveData(I2C2);
    }

    // CRC校验
    if (rx_buf[2] != SCD30_CalcCRC8(rx_buf, 2)) {
        return 2;
    }

    // 返回就绪状态
    return (rx_buf[1] == 0x01) ? 0 : 1;
}

/**
 * 读取测量数据
 * @param data 存储数据的结构体指针
 * @return 0=成功, 1=未就绪, 2=通信失败, 3=CRC错误
 */
uint8_t SCD30_ReadMeasData(SCD30_MeasData *data) {
    uint8_t rx_buf[18] = {0};
    uint32_t temp_u32 = 0;

    // 检查数据是否就绪
    uint8_t ready_status = SCD30_CheckDataReady();
    if (ready_status == 1) return 1;
    if (ready_status == 2) return 2;

    // 发送读取命令
    if (SCD30_I2C2_WriteCmd(SCD30_CMD_READ_MEAS, 0, 0) != 0) {
        return 2;
    }

    Delay_ms(SCD30_DELAY_AFTER_CMD);

    // 发送START信号
    I2C_GenerateSTART(I2C2, ENABLE);
    uint32_t timeout = SCD30_I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
        if (--timeout == 0) return 2;
    }

    // 发送读地址
    I2C_Send7bitAddress(I2C2, SCD30_I2C_ADDR_READ, I2C_Direction_Receiver);
    timeout = SCD30_I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (--timeout == 0) return 2;
    }

    // 读取18字节数据
    for (uint8_t i = 0; i < 18; i++) {
        if (i < 17) {
            I2C_AcknowledgeConfig(I2C2, ENABLE);
        } else {
            I2C_AcknowledgeConfig(I2C2, DISABLE);
            I2C_GenerateSTOP(I2C2, ENABLE);
        }

        timeout = SCD30_I2C_TIMEOUT;
        while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            if (--timeout == 0) return 2;
        }
        rx_buf[i] = I2C_ReceiveData(I2C2);
    }

    // CRC校验
    for (uint8_t i = 0; i < 18; i += 3) {
        if (rx_buf[i+2] != SCD30_CalcCRC8(&rx_buf[i], 2)) {
            data->is_valid = 0;
            return 3;
        }
    }

    // 转换CO₂数据
    temp_u32 = ((uint32_t)rx_buf[0] << 24) | ((uint32_t)rx_buf[1] << 16) |
               ((uint32_t)rx_buf[3] << 8) | rx_buf[4];
    data->co2_ppm = *(float*)&temp_u32;

    // 转换温度数据
    temp_u32 = ((uint32_t)rx_buf[6] << 24) | ((uint32_t)rx_buf[7] << 16) |
               ((uint32_t)rx_buf[9] << 8) | rx_buf[10];
    data->temperature_c = *(float*)&temp_u32;

    // 转换湿度数据
    temp_u32 = ((uint32_t)rx_buf[12] << 24) | ((uint32_t)rx_buf[13] << 16) |
               ((uint32_t)rx_buf[15] << 8) | rx_buf[16];
    data->humidity_rh = *(float*)&temp_u32;

    data->is_valid = 1;
    return 0;
}




































//void SCD30_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
//{
//	uint32_t TimeOut;
//	TimeOut = 10000;
//	while(I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
//	{
//		TimeOut --;
//		if(TimeOut ==0)
//		{
//			break;
//		}
//	}
//}

//void SCD30_WriteReg(uint8_t RegAddress, uint8_t Data)     //指定地址写寄存器
//{
////	MyI2C_Start();										    //开始通信
////	MyI2C_SendByte(MPU6050_ADDRESS);					    //读取设备
////	MyI2C_ReceiveAck();									    //接收应答位
////	MyI2C_SendByte(RegAdddasaress);							    //读取设备中的寄存器（准备写）
////	MyI2C_ReceiveAck();										//接收应答位
////	MyI2C_SendByte(Data);									//向寄存器中写入数据
////	MyI2C_ReceiveAck();										//接收应答位
////	MyI2C_Stop();  											//停止通信

//	
//	
//	I2C_GenerateSTART(I2C2, ENABLE);
//	SCD30_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
//	
//	I2C_Send7bitAddress(I2C2,SCD30_ADDRESS,I2C_Direction_Transmitter);
//	SCD30_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
//	
//	I2C_SendData(I2C2, RegAddress);
//	SCD30_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING);
//	
//	I2C_SendData(I2C2, Data);
//	SCD30_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);
//	
//	I2C_GenerateSTOP(I2C2, ENABLE);

//}

//uint8_t MPU6050_ReadReg(uint8_t RegAddress)    //指定地址读寄存器
//{
//	uint8_t Data;
//	
////	MyI2C_Start();											//起始条件
////	MyI2C_SendByte(MPU6050_ADDRESS);						//读取设备
////	MyI2C_ReceiveAck();										//接收应答位
////	MyI2C_SendByte(RegAddress);								//读取设备中的寄存器
////	MyI2C_ReceiveAck();										//接收应答位
////	
////	MyI2C_Start();											//重复起始条件
////	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);					//读取设备（准备读）
////	MyI2C_ReceiveAck();										//接收应答位
////	Data = MyI2C_ReceiveByte();								//读取数据
////	MyI2C_SendAck(1);										//发送非应答位（告诉从机不准备继续读取）
////	MyI2C_Stop();  											//停止通信
//	
//	I2C_GenerateSTART(I2C2, ENABLE);
//	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
//	
//	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);
//	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
//	
//	I2C_SendData(I2C2, RegAddress);
//	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);
//	
//	I2C_GenerateSTART(I2C2, ENABLE);
//	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
//	
//	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Receiver);
//	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
//	
//	I2C_AcknowledgeConfig(I2C2,DISABLE);
//	I2C_GenerateSTOP(I2C2,ENABLE);
//	
//	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);
//	
//	Data = I2C_ReceiveData(I2C2);
//	
//	I2C_AcknowledgeConfig(I2C2,ENABLE);
//	
//	return Data;
//}

//void MPU6050_Init(void)
//{
////	MyI2C_Init();
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	I2C_InitTypeDef I2C_InitStructure;
//    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//	I2C_InitStructure.I2C_ClockSpeed =	100000;
//	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
//	I2C_Init(I2C2,&I2C_InitStructure);
//	
//	I2C_Cmd(I2C2,ENABLE);
//	
//	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);				//接触睡眠 陀螺仪时钟
//	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);      		//6轴不待机
//	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);		
//	MPU6050_WriteReg(MPU6050_CONFIG,0x06);	
//	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
//	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);	
//}

//uint8_t MPU6050_GetID(void)
//{
//	return MPU6050_ReadReg(MPU6050_WHO_AM_I);				//读取设备ID
//}

//void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
//	int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
//{
//	uint16_t DataH, DataL;
//	
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
//	*AccX = (DataH <<8 | DataL);
//	
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
//	*AccY = (DataH <<8 | DataL);
//	
//	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
//	*AccZ = (DataH <<8 | DataL);
//	
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
//	*GyroX = (DataH <<8 | DataL);
//	
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
//	*GyroY = (DataH <<8 | DataL);
//	
//	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
//	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
//	*GyroZ = (DataH <<8 | DataL);
//}
