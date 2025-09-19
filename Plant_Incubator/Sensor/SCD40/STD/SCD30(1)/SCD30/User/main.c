#include "stm32f10x.h"
#include "scd30.h"
#include "oled.h"
#include "delay.h"
#include <stdio.h>
#include "LED.h"

int main(void) {
    // 初始化系统资源
    SCD30_I2C2_Init();         // 初始化I2C2通信接口
    OLED_Init();               // 初始化OLED显示屏
	LED_Init();				   // 初始化LED灯
    
    // 清屏并显示初始化信息
    OLED_Clear();
    OLED_ShowString(1, 1, "Initializing...");
    
    // 启动SCD30连续测量
    uint8_t init_status = SCD30_StartContinuousMeas();
    if (init_status != 0) {
        OLED_Clear();
        OLED_ShowString(1, 1, "SCD30 Init Fail");
        OLED_ShowString(2, 1, "Check Connection");
        while (1);  // 初始化失败则停留在此
    }
    
    // 初始化成功，准备进入主循环
    OLED_Clear();
    OLED_ShowString(1, 1, "System Ready");
    Delay_ms(1000);
    OLED_Clear();
    
    SCD30_MeasData data;       // 存储传感器数据
    char display_buf[32];      // 显示缓冲区
    
    // 主循环：持续读取并显示数据
    while (1) {
        // 读取传感器数据
        uint8_t read_status = SCD30_ReadMeasData(&data);
        
        // 根据读取状态显示不同内容
        if (read_status == 0 && data.is_valid) {
            //判断数据指标
            if(data.co2_ppm > 2000||data.temperature_c > 35)  
			{
				LED1_ON();
				LED2_OFF();
			}
			else 
			{	
				LED1_OFF();
				LED2_ON();
			}
			// 数据有效，显示测量值
            // 显示CO₂浓度
			sprintf(display_buf, "CO2: %.1f ppm", data.co2_ppm);
            OLED_ShowString(1, 1, display_buf);
            
            // 显示温度
            sprintf(display_buf, "Temp: %.1f C", data.temperature_c);
            OLED_ShowString(2, 1, display_buf);
            
            // 显示湿度
            sprintf(display_buf, "Humi: %.1f %%", data.humidity_rh);
            OLED_ShowString(3, 1, display_buf);
        } else {
            // 数据无效，显示错误信息
            OLED_Clear();
			OLED_ShowString(1, 1, "Data Error");
			sprintf(display_buf, "Code: %d", read_status);
            OLED_ShowString(2, 1, display_buf);
		}
        
        // 等待下一次测量（SCD30默认测量间隔为2秒）
        Delay_ms(2200);
    }
}


