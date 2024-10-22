#include "stm32f40x.h"                  // Device header
#include "Servos.h"
// ***********************************************************
// 主要函数及其功能
// void Servo_Auto(uint16_t open_time_seconds, uint16_t wait_time_hours)
// 定时自动开关舵机


// ***********************************************************
extern TIM_HandleTypeDef htim1;

//舵机打开,设置角度
void Servo_Open(uint16_t angle)
{
    //计算占空比
    uint16_t compare = angle / 9.0 + 5;
    //设置占空比
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, compare);
}

//舵机关闭
void Servo_Close(void)
{
    //设置占空比
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 5);
}

//定时自动开关舵机
void Servo_Auto(uint16_t open_time_seconds, uint16_t wait_time_hours)
{
    static uint8_t state = 0;
    static uint32_t last_tick = 0;

    switch (state)
    {
        case 0:
            // Open the servo to 180 degrees
            Servo_Open(180);
            // Record the current tick
            last_tick = HAL_GetTick();
            // Move to the next state
            state = 1;
            break;

        case 1:
            // Check if the specified time has passed
            if (HAL_GetTick() - last_tick >= open_time_seconds * 1000)
            {
                // Close the servo
                Servo_Close();
                // Record the current tick
                last_tick = HAL_GetTick();
                // Move to the next state
                state = 2;
            }
            break;

        case 2:
            // Check if the wait time has passed
            if (HAL_GetTick() - last_tick >= wait_time_hours * 3600 * 1000)
            {
                // Move to the initial state to repeat the cycle
                state = 0;
            }
            break;
    }
}






