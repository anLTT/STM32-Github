#include "GPS.h"
#include "stdbool.h"
#include "string.h"
#include "main.h"
#include "app.h"
#include "usart.h"

#define GPS_RESUME_TIME (60 * 15)

ST_GPS_DATA g_stGPS;
uint8_t g_ucComRxBuf[SERIAL_SND_BUF_LEN];     //接收缓冲,最大SERIAL_BUF_LEN个字节.

/*
	GPS所需变量内存初始化
*/
void GPS_Ram_Init(void)
{
    memset(g_ucComRxBuf, 0, sizeof(g_ucComRxBuf));
    memset(&g_stGPS, 0, sizeof(g_stGPS));
	memset(&g_stSerial2, 0, sizeof(g_stSerial2));
}

void GPS_Get_Data(void)
{
	uint16_t usSerilBufIdx = 0;// * 串口接收缓冲区索引
	uint16_t s_usGPSBufIdx = 0;// * GPS数据接收缓冲区索引

    if(g_stSerial2.usRcvProcessLen) 
    {
		g_stSerial2.usRcvProcessLen = 0;
		for(usSerilBufIdx = 0; usSerilBufIdx < SERIAL_SND_BUF_LEN; usSerilBufIdx++)
		{
			if(g_stSerial2.pucRcvBuf[usSerilBufIdx] == '$')
			{
				s_usGPSBufIdx = 0;
			}

			g_ucComRxBuf[s_usGPSBufIdx++] = g_stSerial2.pucRcvBuf[usSerilBufIdx];

			if((g_ucComRxBuf[0] == '$' ) && (g_ucComRxBuf[4] == 'M' ) && 
				(g_ucComRxBuf[5] == 'C'))     // * 确定是否收到"GPRMC/GNRMC"这一帧数据
			{
				if(g_ucComRxBuf[s_usGPSBufIdx] == '\n')									   
				{
					memset(g_stGPS.pucGPS_Buffer, 0, GPS_Buffer_Length);    // * 清空
					memcpy(g_stGPS.pucGPS_Buffer, g_ucComRxBuf, s_usGPSBufIdx); // * 保存数据
					g_stGPS.ucIsGetData = TRUE; // * 读取到数据标志
					s_usGPSBufIdx = 0;              // * 清空
					memset(g_ucComRxBuf, 0, SERIAL_SND_BUF_LEN);// * 清空				
				}	
			}
			
			if(s_usGPSBufIdx >= SERIAL_SND_BUF_LEN)
			{
				s_usGPSBufIdx = SERIAL_SND_BUF_LEN;
			}
		}
    }
}

void GPS_Decode_DATA(void)
{
	uint8_t *pucSubString;
	uint8_t *pucSubStringNext;
	uint8_t ucGPSInfoInx = 0;
	if(g_stGPS.ucIsGetData)
	{
		g_stGPS.ucIsGetData = false;
		g_stGPS.usErrTimeOut = 0;// 120s 内没有正常解析，则认为GPS模块异常
		for (ucGPSInfoInx = 0 ; ucGPSInfoInx <= 6 ; ucGPSInfoInx++)
		{
			if (ucGPSInfoInx == 0)
			{
				if ((pucSubString = strstr(g_stGPS.pucGPS_Buffer, ",")) == NULL)
                {
                    //解析错误
                }
			}
			else
			{
				pucSubString++;
				if ((pucSubStringNext = strstr(pucSubString, ",")) != NULL)
				{
					uint8_t usefullBuffer[2]; 
					switch(ucGPSInfoInx)
					{
						case 1:memcpy(g_stGPS.pucUTCTime, pucSubString, pucSubStringNext - pucSubString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, pucSubString, pucSubStringNext - pucSubString);break;	//获取UTC时间
						case 3:memcpy(g_stGPS.pucLatitude, pucSubString, pucSubStringNext - pucSubString);break;	//获取纬度信息
						case 4:memcpy(g_stGPS.pucN_S, pucSubString, pucSubStringNext - pucSubString);break;	//获取N/S
						case 5:memcpy(g_stGPS.pucLongitude, pucSubString, pucSubStringNext - pucSubString);break;	//获取经度信息
						case 6:memcpy(g_stGPS.pucE_W, pucSubString, pucSubStringNext - pucSubString);break;	//获取E/W

						default:break;
					}
					pucSubString = pucSubStringNext;
					g_stGPS.ucIsParseData = true;
					if(usefullBuffer[0] == 'A')
						g_stGPS.ucIsUsefull = true;
					else if(usefullBuffer[0] == 'V')
						g_stGPS.ucIsUsefull = false;

				}
				else
				{
					//解析错误
				}
			}
		}
	}
}

uint8_t GPS_GetInfoSave(uint8_t *pucGPSInfo)
{
	uint8_t ucBufIdx = 0;

	if(g_stGPS.ucIsParseData && g_stGPS.ucIsUsefull)// * 解析成功 且 数据有效
	{
		g_stGPS.ucIsParseData = false;
		g_stGPS.ucIsUsefull = false;
		memcpy(pucGPSInfo, g_stGPS.pucUTCTime, UTCTime_Length);
		ucBufIdx += UTCTime_Length;
		memcpy(pucGPSInfo + ucBufIdx, g_stGPS.pucLatitude, latitude_Length);
		ucBufIdx += latitude_Length;
		memcpy(pucGPSInfo + ucBufIdx, g_stGPS.pucN_S, N_S_Length);
		ucBufIdx += N_S_Length;
		memcpy(pucGPSInfo + ucBufIdx, g_stGPS.pucLongitude, longitude_Length);
		ucBufIdx += longitude_Length;
		memcpy(pucGPSInfo + ucBufIdx, g_stGPS.pucE_W, E_W_Length);
		ucBufIdx += E_W_Length;
	}
	return ucBufIdx;
}

/*
	GPS异常恢复:重新初始化串口2
	每秒调用一次
*/
void GPS_ErrCheck(void)
{
	if(g_stGPS.usErrTimeOut >= GPS_RESUME_TIME)
	{
		g_stGPS.usErrTimeOut = 0;
		MX_USART2_UART_Init();
		Uart_Rcv_Rdy(2);
		GPS_Ram_Init();
	}
	else
	{
		g_stGPS.usErrTimeOut++;
	}
}
