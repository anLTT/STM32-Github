#ifndef __GPS_H__
#define __GPS_H__

#include "stdint.h"

#define GPS_Buffer_Length   80
#define UTCTime_Length      11
#define latitude_Length     11
#define N_S_Length          2
#define longitude_Length    12
#define E_W_Length          2 

typedef struct 
{
	uint8_t pucGPS_Buffer[GPS_Buffer_Length];
	uint8_t ucIsGetData;		//是否获取到GPS数据
	uint8_t ucIsParseData;	//是否解析完成
	uint8_t pucUTCTime[UTCTime_Length];		//UTC时间
	uint8_t pucLatitude[latitude_Length];		//纬度
	uint8_t pucN_S[N_S_Length];		//N/S
	uint8_t pucLongitude[longitude_Length];		//经度
	uint8_t pucE_W[E_W_Length];		//E/W
	uint8_t ucIsUsefull;		//定位信息是否有效
	uint16_t usErrTimeOut;	//GPS数据获取超时时间
}ST_GPS_DATA;

extern ST_GPS_DATA g_stGPS;// * stm32f1xx_it.c 外部只允许查询 g_stGPS 状态，不允许修改

extern void GPS_Ram_Init(void); //GPS数据初始化函数
extern void GPS_Get_Data(void); //GPS数据获取函数
extern void GPS_Decode_DATA(void); //GPS数据解析函数
extern uint8_t GPS_GetInfoSave(uint8_t *pucGPSInfo);
extern void GPS_ErrCheck(void);
#endif
