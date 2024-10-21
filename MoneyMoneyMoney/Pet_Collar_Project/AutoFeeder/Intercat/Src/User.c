#include "User.h"

static uint8_t interval_t = 6;//放粮间隔,单位小时
static uint8_t duration_t = 30;//放粮时长,单位秒

uint8_t *p_interval = &interval_t;
uint8_t *p_duration = &duration_t;

u8 User_Set_Feed_Interval(uint8_t interval)
{
    //设置放粮间隔
    *p_interval = interval;
    return 0;
}

u8 User_Set_Feed_Duration(uint8_t duration)
{
    //设置放粮时长
    *p_duration = duration;
    return 0;
}


//用户输入处理函数
void Process_User_Input(uint8_t input, uint8_t value)
{
    switch (input)
    {
    case '1'://用户设置了新的放粮间隔
        //更新放粮间隔
        User_Set_Feed_Interval(value);
        break;
    case '2'://用户设置了新的放粮时长
        //更新放粮时长
        User_Set_Feed_Duration(value);
        break;
    //其他情况
    default:
        break;
    }
}