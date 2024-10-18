#include "BlueTooth.h"
#include "usart.h"
#include "app.h"
#include "AT24C32.h"
#include "blood.h"
#include "Addr_Def.h"
#include "gps.h"

#define SOFT_WARE_VERSION  "24-38-01" // * 24年 38周 01版本

#define READ_CMD    0x00
#define SET_CMD     0x01

#define BOARD_ADDR 0xFE // * 通用设备地址

extern ST_SERIAL g_stSerial1;
extern uint32_t g_ulInitPsw;
extern ST_SAVEINFO g_stSaveInfo;
extern uint32_t g_ucDevAddr;

static uint8_t s_ucRstFlag = FALSE;


typedef enum
{
    E_DEVICE_NUMBER  = 0x0001,
    E_GPS_LATITUDE   = 0x0002,
    E_GPS_NS         = 0x0003,
    E_GPS_LONGITUDE  = 0x0004,
    E_GPS_EW         = 0x0005,
    E_BLOOD_HEART    = 0x0006,
    E_BLOOD_OXYGEN   = 0x0007,
    E_SECOND_INDEX   = 0x0008,
    E_DEV_ACTIVATE   = 0x0009,
    E_DEV_INIT_PSW   = 0x000A,
    E_DEV_VERSION    = 0x000B,
    E_REST           = 0x000C,
    E_MAX            = 0x000D,
    
    E_REC_SAVE_PT    = 0x1000,
    E_REC_INFO_0     = 0x1001,
    E_REC_INFO_71    = 0x1048,
}ENUM_DATA_TYPE;

/*
    蓝牙数据读取项处理
*/
static uint16_t BlueTooth_READ(uint16_t usData, uint8_t *pucSndBuf)
{
    uint16_t usLen = 0;

    switch(usData)
    {
        case E_DEVICE_NUMBER:
            At24c02_Read_Amount_Byte(ADDR_DEVICE_NUMBER, pucSndBuf, 8);
            usLen = 8;
            break;
        case E_GPS_LATITUDE:
            memcpy(pucSndBuf, (uint8_t *)&g_stGPS.pucLatitude, 11);
            usLen = 11;
            break;
        case E_GPS_NS:
            memcpy(pucSndBuf, (uint8_t *)&g_stGPS.pucN_S, 2);
            usLen = 2;
            break;
        case E_GPS_LONGITUDE:
            memcpy(pucSndBuf, (uint8_t *)&g_stGPS.pucLongitude, 12);
            usLen = 12;
            break;
        case E_GPS_EW:
            memcpy(pucSndBuf, (uint8_t *)&g_stGPS.pucE_W, 2);
            usLen = 2;
            break;
        case E_BLOOD_HEART:
            memcpy(pucSndBuf, (uint8_t *)&g_blooddata.heart, 2);
            usLen = 2;
            break;
        case E_BLOOD_OXYGEN:
            memcpy(pucSndBuf, (uint8_t *)&g_blooddata.SpO2, 4);
            usLen = 4;
            break;
        case E_SECOND_INDEX:
            memcpy(pucSndBuf, (uint8_t *)&g_stTime.ulSecondIdx, 4);
            usLen = 4;
            break;
        case E_DEV_ACTIVATE:
            At24c02_Read_Amount_Byte(ADDR_DEVICE_ACTIVATE, pucSndBuf, 1);
            usLen = 1;
            break;
        case E_DEV_VERSION:
            memcpy(pucSndBuf, (uint8_t *)&SOFT_WARE_VERSION, 8);
            usLen = 8;
            break;
        default:
            ;
    }

    // * 调换大小端
    if(usLen)
    {
        app_ExchangeBytes(pucSndBuf, usLen);
    }
    return usLen;
}

/*
    蓝牙数据写入项处理
*/
static uint8_t BlueTooth_SET(uint16_t usData, uint8_t *pucBuf)
{
    uint8_t ucRet = FALSE;

    switch(usData)
    {
        case E_DEVICE_NUMBER:
            app_ExchangeBytes(pucBuf, 8);
            At24c02_Write_Amount_Byte(ADDR_DEVICE_NUMBER, pucBuf, 8);
            ucRet = TRUE;
            break;
        case E_SECOND_INDEX:
            // * 低位在后 高位在前
            g_stTime.ulSecondIdx = pucBuf[3] | (pucBuf[2] << 8) | (pucBuf[1] << 16) | (pucBuf[0] << 24);
            ucRet = TRUE;
            break;
        case E_DEV_ACTIVATE:
            ucRet = app_Activate_Sta_Change(pucBuf[0]);
            if(ucRet == E_DEVICE_DEACTIVATE)
            {
                // * 取消激活状态需要重启设备
                s_ucRstFlag = TRUE;
                ucRet = TRUE;
            }
            break;
        case E_DEV_INIT_PSW:
            g_ulInitPsw = pucBuf[3] | (pucBuf[2] << 8) | (pucBuf[1] << 16) | (pucBuf[0] << 24);
            ucRet = TRUE;
            break;
        case E_REST:
            s_ucRstFlag = TRUE;
            ucRet = TRUE;
            break;
        default:
            ;
    }
    return ucRet;
}
    uint8_t pucGpsData[UTCTime_Length + latitude_Length + N_S_Length + longitude_Length + E_W_Length];
    uint8_t pucBloodData[6];
    uint16_t usCRC;
/*
    Info日志读取
*/
static uint16_t BlueTooth_INFO_READ(uint16_t usData, uint8_t *pucSndBuf)
{
    uint16_t usLen = 0;
    ST_SAVEINFO stSaveInfo;
    uint8_t ucIndex = 0;
    
    if(usData == E_REC_SAVE_PT)
    {
        At24c02_Read_Amount_Byte(ADDR_GPS_BLOOD_DATA_PT, pucSndBuf, 2);
        app_ExchangeBytes(pucSndBuf, 2);
        usLen = 2;
    }
    else if((usData >= E_REC_INFO_0) && (usData <= E_REC_INFO_71))
    {
        At24c02_Read_Amount_Byte(ADDR_GPS_BLOOD_DATA_BUF + (usData - E_REC_INFO_0) * sizeof(stSaveInfo), (uint8_t *)&stSaveInfo, sizeof(stSaveInfo));
        
        app_ExchangeBytes((uint8_t *)&stSaveInfo.pucGpsData, UTCTime_Length);
        ucIndex += UTCTime_Length;
        app_ExchangeBytes((uint8_t *)&stSaveInfo.pucGpsData + ucIndex, latitude_Length);
        ucIndex += latitude_Length;
        app_ExchangeBytes((uint8_t *)&stSaveInfo.pucGpsData + ucIndex, N_S_Length);
        ucIndex += N_S_Length;
        app_ExchangeBytes((uint8_t *)&stSaveInfo.pucGpsData + ucIndex, longitude_Length);
        ucIndex += longitude_Length;
        app_ExchangeBytes((uint8_t *)&stSaveInfo.pucGpsData + ucIndex, E_W_Length);
        ucIndex += E_W_Length;
        app_ExchangeBytes((uint8_t *)&stSaveInfo.pucBloodData, sizeof(stSaveInfo.pucBloodData));
        app_ExchangeBytes((uint8_t *)&stSaveInfo.ulSecondIdx, sizeof(stSaveInfo.ulSecondIdx));
        app_ExchangeBytes((uint8_t *)&stSaveInfo.usCRC, sizeof(stSaveInfo.usCRC));
        memcpy(pucSndBuf, (uint8_t *)&stSaveInfo, sizeof(stSaveInfo));

        usLen = sizeof(g_stSaveInfo);
    }
    else
    {
        return FALSE;
    }
    return usLen;
}

/*
    蓝牙数据处理
    1byte设备地址 + 1byte命令码 + 2byte数据类型 + 2byte数据长度 + 数据 + 2byte校验码
*/
void BlueTooth_Process(void)
{
    uint16_t usDataLen = 0;
    uint16_t usData = 0;
    uint16_t usSndLen = 0;
    uint16_t usCrc16 = 0;

    // * 串口发送完成
    if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == SET))
    {
        if(g_stSerial1.usRcvProcessLen)
        {
            usCrc16 = g_stSerial1.pucRcvBuf[g_stSerial1.usRcvProcessLen - 2] | (g_stSerial1.pucRcvBuf[g_stSerial1.usRcvProcessLen - 1] << 8);
            if(usCrc16 == app_Crc16(g_stSerial1.pucRcvBuf, g_stSerial1.usRcvProcessLen - 2))
            {
                // * 低位在前 高位在后
                usData = (g_stSerial1.pucRcvBuf[2] << 8) | g_stSerial1.pucRcvBuf[3];    // * 数据类型
                usDataLen = (g_stSerial1.pucRcvBuf[4] << 8) | g_stSerial1.pucRcvBuf[5]; // * 数据长度
                if(g_stSerial1.usRcvProcessLen < 8)// * 长度小于协议最短长度
                {
                    return ;
                }
                if((g_ucDevAddr != g_stSerial1.pucRcvBuf[0]) && (BOARD_ADDR != g_stSerial1.pucRcvBuf[0]))// * 设备地址不匹配
                {
                    return ;
                }
                if(g_stSerial1.pucRcvBuf[1] == READ_CMD)
                {
                    if(usData < E_MAX)
                    {
                        usSndLen = BlueTooth_READ(usData, g_stSerial1.pucSndBuf + 6);// * 读取普通数据
                    }
                    else if((usData >= E_REC_SAVE_PT) && (usData <= E_REC_INFO_71))  // * 读取日志
                    {
                        usSndLen = BlueTooth_INFO_READ(usData, g_stSerial1.pucSndBuf + 6);
                    }
                }
                else if (g_stSerial1.pucRcvBuf[1] == SET_CMD)
                {
                    if(BlueTooth_SET(usData, g_stSerial1.pucRcvBuf + 6))
                    {
                        *(g_stSerial1.pucSndBuf + 6) = TRUE;
                        usSndLen = 1;
                    }
                    else
                    {
                        *(g_stSerial1.pucSndBuf + 6) = FALSE;
                    }
                }

                if(usSndLen != 0)
                {
                    g_stSerial1.pucSndBuf[0] = g_ucDevAddr;              // * 设备地址
                    g_stSerial1.pucSndBuf[1] = g_stSerial1.pucRcvBuf[1]; // * 命令码
                    g_stSerial1.pucSndBuf[2] = g_stSerial1.pucRcvBuf[2]; // * 数据类型
                    g_stSerial1.pucSndBuf[3] = g_stSerial1.pucRcvBuf[3]; // * 数据类型
                    g_stSerial1.pucSndBuf[4] = (usSndLen >> 8) & 0xff;   // * 数据长度
                    g_stSerial1.pucSndBuf[5] = usSndLen & 0xff;          // * 数据长度
                    usSndLen += 6;
                    usCrc16 = app_Crc16(g_stSerial1.pucSndBuf, usSndLen);// * 计算校验码
                    g_stSerial1.pucSndBuf[usSndLen] = usCrc16 & 0xff; // * 校验码
                    g_stSerial1.pucSndBuf[usSndLen + 1] = (usCrc16 >> 8) & 0xff; // * 校验码
                    
                    usSndLen += 2;
                    HAL_UART_Transmit_IT(&huart1, g_stSerial1.pucSndBuf, usSndLen);
                }
            }
            g_stSerial1.usRcvProcessLen = 0;
        }

        // * 需要复位
        if(s_ucRstFlag == TRUE)
        {
            while (1);// * 等待看门狗复位
        }
    }
}
