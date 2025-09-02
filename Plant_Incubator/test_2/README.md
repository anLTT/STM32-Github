# 具体

这个工程目前来讲实现的功能就只有RX和TX

# 注

重映射pritnf一定要勾选Keil中魔术棒里面的Use MicroLIB

# 目标实现功能

1. FREERTOS
2. CAN+UART(和LCD的要看嘛?)
3. 按键-->状态机变成思路我爱你!!!【[编程技巧] 第2期 全功能按键非阻塞式实现 按键单击 双击 长按】 https://www.bilibili.com/video/BV13c7kzeEYk/?share_source=copy_web&vd_source=dbec21df08456504e47ed0c0559f1ff7
        -->这个函数在1ms的定时中断里面，for循环很耗时长 太久了程序会崩，up这里次数少影响不大
4. RTC
5. 食谱

# 附加小功能

1. ADC采集电池电压判断充放电(是不是到时候还有个Out)
