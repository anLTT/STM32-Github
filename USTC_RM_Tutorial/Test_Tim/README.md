让我想想,就是定时器中断吗,可以在it.c中找TIM3_IRQHandler,
然后看到专属于TIM3的定时器中断函数中调用了通用的定时器中断函数HAL_TIM_IRQHandler,并且指明是&htim3,
在往里面看,发现这个通用的定时器中断函数里面有非常多的回调函数,
这些回调函数都在特定事件发生时各自出发,比如说有
        HAL_TIM_IC_CaptureCallback(htim);
        HAL_TIM_OC_DelayElapsedCallback(htim);
        HAL_TIM_IC_CaptureCallback(htim);
        HAL_TIM_PWM_PulseFinishedCallback(htim);
    之类的,并且能看到用之前都在进行if判断,
    然后本次用到的是HAL_TIM_PeriodElapsedCallback
    跳转到定义的话也能看到是个虚函数,
    我们要进行的操作就是将其在main.c中写成实函数并进行使用