#include "app.h"
#include "string.h"
#include "stdint.h"
#include "usart.h"
#include "tim.h"
#include "GPS.h"
#include "MAX30100.h"
#include "blood.h"
#include "AT24C32.h"
#include "IIC.h"
#include "Addr_Def.h"
#include "time.h"
#include "BlueTooth.h"
#include "iwdg.h"

//#define SOFT_IIC_EN

extern uint8_t g_ucComRxBuf[SERIAL_SND_BUF_LEN];
uint32_t g_ulInitPsw = 0;
ST_TIME g_stTime;
ST_SAVEINFO g_stSaveInfo;
uint32_t g_ucDevAddr = 0;

static void app_Loop(void);
static void app_InfoGet(void);
static void app_SoftRTC(void);
static void app_InfoSaveLoop(void);
static void app_InfoClear(void);

void app_Ram_init(void)
{
    // * 清空缓存
    GPS_Ram_Init();

    // * 获取设备地址
    At24c02_Read_Amount_Byte(ADDR_DEVICE_NUMBER + 7, (uint8_t*)&g_ucDevAddr, 1);
}

void app_init(void)
{
    Uart_Rcv_Rdy(1);// * 触发串口1接收
    Uart_Rcv_Rdy(2);// * 触发串口2接收
    HAL_TIM_Base_Start_IT(&htim1);// * 100ms定时中断使能
    app_Ram_init();// * 清空缓存
    max30100_init();
#ifdef SOFT_IIC_EN
    HAL_I2C_MspDeInit(&hi2c2);
    I2C_GPIO_Init();// * 模拟IIC初始化
#endif
    HAL_UART_Transmit_IT(&huart1, "Init_Ok", strlen("Init_Ok"));
}

void app_main(void)
{
    static uint8_t s_ucTaskCnt = 0; // * 任务切换序号
    // static uint16_t s_usTestWrite16Bit = 0x5AA5;
	// static uint16_t s_usTestRead16bit = 0;
    // static uint8_t s_ucTestWrite8Bit = 0xA5;
    // static uint8_t s_ucTestRead8bit = 0;
	

    app_Loop();

    if(TIM_TIMER_STA(E_TIMER_100MS) == TRUE)
    {
        // * 100ms任务

        // * 1s任务
        s_ucTaskCnt++;

        if(s_ucTaskCnt >= E_100MS_IN_10MS_CNT)
        {
            s_ucTaskCnt = 0;
        }

        switch(s_ucTaskCnt)
        {
            case 0:
            {
                HAL_IWDG_Refresh(&hiwdg);
                GPS_Decode_DATA();// * 每秒解析GPS数据
            }break;
            case 1:
            {
                
            }break;
//             case 2:
//             {
// #ifdef SOFT_IIC_EN
//                 AT24C32_WriteByte(0x00000002, (uint8_t*)&s_ucTestWrite8Bit);// * 模拟IIC
// #else
//                 At24c02_Write_Amount_Byte(0x00000003, (uint8_t*)&s_usTestWrite16Bit, 2);
//                 //At24c02_Write_Byte(0x00000002, (uint8_t*)&s_ucTestWrite8Bit);
// #endif
//             }break;
//             case 3:
//             {
// #ifdef SOFT_IIC_EN
//                 AT24C32_ReadByte(0x00000002, (uint8_t*)&s_ucTestRead8bit);// * 模拟IIC
// #else
//                 At24c02_Read_Amount_Byte(0x00000003, (uint8_t*)&s_usTestRead16bit, 2);
//                 //At24c02_Read_Byte(0x00000002, (uint8_t*)&s_ucTestRead8bit);
// #endif
//             }break;
            case 4:
            {
                app_SoftRTC();
            }break;
            case 5:
            {
                HAL_IWDG_Refresh(&hiwdg);
                app_InfoGet();
            }break;
            case 6:
            {
                HAL_IWDG_Refresh(&hiwdg);
            }break;
            case 7:
            {
                GPS_ErrCheck();
                blood_Err_Check();
                Uart_ErrCheck();
            }break;
            default:
            {
                
            }
        }
    }
}

static void app_Loop(void)
{
    //血液信息获取
	blood_data_update();
	//血液信息转换
	blood_data_translate();
    //GPS信息获取
    GPS_Get_Data();
    //蓝牙信息处理
    BlueTooth_Process();
}

/*
    每秒轮询一次数据是否准备好，因GPS在室内不一定能获取到数据
    所以当心率获取到，且30s到时，更新内存中的数据，10min 到时，将数据保存到flash中
*/
static void app_InfoGet(void)
{
    static uint8_t s_ucTime30s = 0;
    static uint8_t s_ucBloodReady = FALSE;
    uint8_t ucDevActivate;

    if((s_ucBloodReady == FALSE) && blood_GetInfoGet(g_stSaveInfo.pucBloodData))
    {
        s_ucBloodReady = TRUE;
    }

    GPS_GetInfoSave(g_stSaveInfo.pucGpsData);

    s_ucTime30s++;
    if(s_ucTime30s >= 30)
    {
        s_ucTime30s = 0;
        if(s_ucBloodReady)
        {
            s_ucBloodReady = FALSE;
            // * 10min 保存一次 flash
            if((g_stTime.ulMin % 10) == 0)
            {
                At24c02_Read_Amount_Byte(ADDR_DEVICE_ACTIVATE, (uint8_t *)&ucDevActivate, sizeof(ucDevActivate));
                if(ucDevActivate == E_DEVICE_ACTIVATE)
                {
                    g_stSaveInfo.ulSecondIdx = g_stTime.ulSecondIdx;
                    g_stSaveInfo.usCRC = app_Crc16((uint8_t*)&g_stSaveInfo, sizeof(g_stSaveInfo) - 2);
                    app_InfoSaveLoop();
                }
            }
        }
    }
}

/*
    循环保存函数
*/
static void app_InfoSaveLoop(void)
{
    uint8_t ucSavePoint = 0;
    ST_SAVEINFO st_TmpInfo;

    ucSavePoint = ((g_stTime.ulHour * 60) + g_stTime.ulMin) / 10;// * 得出当前时间点，是一天中的第几个10分钟，等效于第几个保存点
    At24c02_Write_Amount_Byte(ADDR_GPS_BLOOD_DATA_PT, (uint8_t*)&ucSavePoint, sizeof(ucSavePoint));
    At24c02_Write_Amount_Byte(ADDR_GPS_BLOOD_DATA_BUF + ucSavePoint * sizeof(g_stSaveInfo), (uint8_t*)&g_stSaveInfo, sizeof(g_stSaveInfo));

    // * 读取当前保存点，判断是否保存成功，如果失败，则尝试一次
    At24c02_Read_Amount_Byte(ADDR_GPS_BLOOD_DATA_BUF + ucSavePoint * sizeof(st_TmpInfo), (uint8_t*)&st_TmpInfo, sizeof(st_TmpInfo));
    if(st_TmpInfo.usCRC != g_stSaveInfo.usCRC)
    {
        At24c02_Write_Amount_Byte(ADDR_GPS_BLOOD_DATA_PT, (uint8_t*)&ucSavePoint, sizeof(ucSavePoint));
        At24c02_Write_Amount_Byte(ADDR_GPS_BLOOD_DATA_BUF + ucSavePoint * sizeof(g_stSaveInfo), (uint8_t*)&g_stSaveInfo, sizeof(g_stSaveInfo));
    }

    memset(g_stSaveInfo.pucGpsData, 0, sizeof(g_stSaveInfo.pucGpsData));
    memset(g_stSaveInfo.pucBloodData, 0, sizeof(g_stSaveInfo.pucBloodData));
}

/*
    将时间戳转换为分钟与小时
*/
static void app_SoftRTC(void)
{
    struct tm *st_Time;

    g_stTime.ulSecondIdx++;

    st_Time = localtime(&g_stTime.ulSecondIdx);
    g_stTime.ulHour = st_Time->tm_hour;
    g_stTime.ulMin = st_Time->tm_min;
}

/*
    CRC 校验
*/
uint16_t app_Crc16(const uint8_t *pucData, uint16_t usLength)
{
    uint16_t usCrc = 0xFFFF;
    uint8_t ucCnt = 0;

    for (uint16_t usPos = 0; usPos < usLength; usPos++)
    {
        usCrc ^= (uint16_t)pucData[usPos];
        for (ucCnt = 8; ucCnt != 0; ucCnt--)
        {
            if ((usCrc & 0x0001) != 0)
            {
                usCrc >>= 1;
                usCrc ^= 0xA001;
            }
            else
                usCrc >>= 1;
        }
    }
    return usCrc;
}

/*
    设备激活状态修改
*/
uint8_t app_Activate_Sta_Change(uint8_t ucSta)
{
    uint32_t ulTmp = 0;

    At24c02_Read_Amount_Byte(ADDR_DEVICE_NUMBER + 4, (uint8_t *)&ulTmp, sizeof(ulTmp));
    if(g_ulInitPsw == ulTmp)
    {
        if(ucSta == E_DEVICE_DEACTIVATE)// * 取消激活，则清除全部Info数据
        {
            g_stTime.ulSecondIdx = 0;
            app_InfoClear();
        }
        At24c02_Write_Amount_Byte(ADDR_DEVICE_ACTIVATE, (uint8_t *)&ucSta, sizeof(ucSta));
        return ucSta;
    }
    else
    {
        return FALSE;
    }
}

/*
    清除全部Info数据，并关闭Info保存功能
*/
static void app_InfoClear(void)
{
    uint8_t ucSavePoint = 0;
    ST_SAVEINFO st_TmpInfo;

    At24c02_Write_Amount_Byte(ADDR_GPS_BLOOD_DATA_PT, (uint8_t*)&ucSavePoint, sizeof(ucSavePoint));

    memset((uint8_t*)&st_TmpInfo, 0, sizeof(st_TmpInfo));
    for(uint8_t ucCnt = 0; ucCnt < 72; ucCnt++)
    {
        HAL_IWDG_Refresh(&hiwdg);
        At24c02_Write_Amount_Byte(ADDR_GPS_BLOOD_DATA_BUF + ucCnt * sizeof(st_TmpInfo), (uint8_t*)&st_TmpInfo, sizeof(st_TmpInfo));
    }
}

/*
    调换数组高低顺序
*/
void app_ExchangeBytes(uint8_t *pucData, uint16_t usLen)
{
    uint8_t ucTmp = 0;

    if((pucData == NULL) || (usLen == 0))
    {
        return;
    }

    for(uint16_t usCnt = 0; usCnt < usLen / 2; usCnt++)
    {
        ucTmp = pucData[usCnt];
        pucData[usCnt] = pucData[usLen - usCnt - 1];
        pucData[usLen - usCnt - 1] = ucTmp;
    }
}

/*
    1ms定时器回调函数
*/
void app_1msTimerCallback(void)
{
    blood_Interrupt();
    Uart_TimeOut();
}
