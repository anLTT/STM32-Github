#include "stm32f4xx.h"                  // Device header

void Blinking(void)
{
    HAL_GPIO_TogglePin(GPIOH,GPIO_PIN_10);
    HAL_GPIO_TogglePin(GPIOH,GPIO_PIN_11);
    HAL_GPIO_TogglePin(GPIOH,GPIO_PIN_12);
}





















