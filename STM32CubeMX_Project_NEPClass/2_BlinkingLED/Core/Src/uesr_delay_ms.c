#include "stm32f4xx.h"                  // Device header

void user_delay_us(uint16_t us)
{
    for(;us > 0;us--)
    {
        for(uint8_t i = 50;i > 0;i--)
        {
            ;
        }
    }
}




void user_delay_ms(uint16_t ms)
{
    for(;ms > 0;ms--)
    {
        user_delay_us(1000);
    }
}























