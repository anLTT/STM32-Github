#include "stm32f40x.h"                  // Device header
#include "LEDProjection.h"

// ***********************************************************
// 主要函数及其功能
// void Check_And_Toggle_LED(void)
// 检查按钮是否被按下，如果是，则切换LED状态

// ***********************************************************


// 切换LED状态
void Toggle_LED(void) {
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_PIN);
}

// 检查按钮是否被按下
uint8_t Is_Button_Pressed(void) {
    return HAL_GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_PIN) == GPIO_PIN_SET;
}


void Check_And_Toggle_LED(void){
    // 如果按钮被按下，则切换LED状态
    if (Is_Button_Pressed()) {
        Toggle_LED();
        // 延时去抖动
        HAL_Delay(300);
        while (Is_Button_Pressed()) {
            // 等待按钮释放
            HAL_Delay(10);
        }
    }
}
