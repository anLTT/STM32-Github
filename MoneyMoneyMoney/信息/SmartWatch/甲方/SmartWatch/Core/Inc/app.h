#ifndef __APP_H
#define __APP_H
#include "GPS.h"
#include "main.h"

typedef struct
{
    int32_t ulHour;
    int32_t ulMin;
    uint32_t ulSecondIdx;// * Unix时间戳
}ST_TIME;

__packed typedef struct
{
    uint8_t pucGpsData[UTCTime_Length + latitude_Length + N_S_Length + longitude_Length + E_W_Length];
    uint8_t pucBloodData[6];
    uint32_t ulSecondIdx;
    uint16_t usCRC;
}ST_SAVEINFO;

typedef enum
{
    E_DEVICE_ACTIVATE = 0x01,
    E_DEVICE_DEACTIVATE = 0xFF,
}ENUM_ACTIVATE_STA;

extern ST_TIME g_stTime;

void app_init(void);
void app_main(void);
extern uint16_t app_Crc16(const uint8_t *pucData, uint16_t usLength);
extern void app_ExchangeBytes(uint8_t *pucData, uint16_t usLen);
extern void app_1msTimerCallback(void);
extern uint8_t app_Activate_Sta_Change(uint8_t ucSta);
#endif
