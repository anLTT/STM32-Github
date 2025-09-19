#ifndef __SCD30_H
#define __SCD30_H

#include "stm32f10x.h"
#include <stdint.h>

// SCD30 I2C相关定义（源自文档1.1.1、1.1.2）
#define SCD30_I2C_ADDR_WRITE    0xC2    // 7位地址0x61左移1位（写操作，最低位0）
#define SCD30_I2C_ADDR_READ     0xC3    // 7位地址0x61左移1位（读操作，最低位1）
#define SCD30_I2C_TIMEOUT       5000     // I2C通信超时时间（ms）
#define SCD30_DELAY_AFTER_CMD   10       // 命令后等待时间（>3ms，文档1.1.2要求）

// SCD30核心命令（源自文档1.4传感器命令集）
#define SCD30_CMD_START_MEAS    0x0010  // 触发连续测量（参数0=禁用气压补偿，文档1.4.1）
#define SCD30_CMD_STOP_MEAS     0x0104  // 停止连续测量（文档1.4.2）
#define SCD30_CMD_GET_DATA_RDY  0x0202  // 获取数据就绪状态（文档1.4.4）
#define SCD30_CMD_READ_MEAS     0x0300  // 读取测量数据（文档1.4.5）

// 测量数据结构体（存储CO₂、温度、湿度，单位参考文档1.217）
typedef struct {
    float co2_ppm;       // CO₂浓度（单位：ppm）
    float temperature_c; // 温度（单位：℃）
    float humidity_rh;   // 相对湿度（单位：%RH）
    uint8_t is_valid;    // 数据有效性标志（1=有效，0=无效）
} SCD30_MeasData;

// 函数声明
void SCD30_I2C2_Init(void);                         // I2C2外设初始化（适配SCD30时序）
uint8_t SCD30_CalcCRC8(uint8_t *data, uint8_t len); // 计算CRC-8（文档1.1.3规则）
uint8_t SCD30_StartContinuousMeas(void);            // 启动连续测量（文档1.4.1）
uint8_t SCD30_CheckDataReady(void);                 // 检查数据是否就绪（文档1.4.4）
uint8_t SCD30_ReadMeasData(SCD30_MeasData *data);   // 读取测量数据（文档1.4.5、1.5）

#endif
