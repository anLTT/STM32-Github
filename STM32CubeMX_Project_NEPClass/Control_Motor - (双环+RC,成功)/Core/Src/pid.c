#include "pid.h"
#include "main.h"

float limit(float input,float MAX_OUTPUT_ABS)
{
    input = input > +MAX_OUTPUT_ABS ? +MAX_OUTPUT_ABS : input;
    input = input < -MAX_OUTPUT_ABS ? -MAX_OUTPUT_ABS : input;
    return input;
}


//初始化参数   --->   每个参数都赋初始值
void initPID(PID* pid,float MAX_OUTPUT,float MAX_ERROR_INTEGRAL)
{
    //三个参数
    pid->kp = 0;
    pid->ki = 0;
    pid->kd = 0;
    //误差
    pid->error.now = 0;
    pid->error.last = 0;
    pid->error.integral = 0;
    //TIO
    pid->target = 0;
    pid->input = 0;
    pid->output = 0;
    //limit----等于传进来的参数
    pid->MAX_ERROR_INTEGRAL = MAX_ERROR_INTEGRAL;
    pid->MAX_OUTPUT = MAX_OUTPUT;

}
//更新[计算输出]PID
void updatePID(PID* pid,float input)
{
    //计算中用到的数值计算与继承, kp,ki,kd在setPIDParam中配
    pid->input = input;
    pid->error.now = pid->target - pid->input;
    pid->error.integral += pid->error.now;
    pid->error.integral = limit(pid->error.integral,pid->MAX_ERROR_INTEGRAL);//误差积分限制[积分上限PID]
    //计算
    pid->output = pid->kp * pid->error.now + pid->ki * pid->error.integral + pid->kd * (pid->error.now - pid->error.last);

    pid->output = limit(pid->output,pid->MAX_OUTPUT);//输出限制

    pid->error.last = pid->error.now;
}
//设置PID参数
void setPIDParam(PID* pid,float kp,float ki,float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

}
//设置目标
void setPIDTarget(PID* pid,float target)
{
    pid->target = target;
}

