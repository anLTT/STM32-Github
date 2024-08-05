#ifndef __PID_H
#define __PID_H

typedef struct 
{
    float now;
    float last;
    float integral;
}Error;

typedef struct 
{
    // kp ki kd 
    float kp, ki, kd;

    //TIO   三个和外界交互的量
    float target, input, output;

    //ERROR
    Error error;

    //limit
    float MAX_OUTPUT;
    float MAX_ERROR_INTEGRAL;


}PID; 



//下面的PID* pid 指的是给哪个PID设置参数

//初始化参数
void initPID(PID* pid,float MAX_OUTPUT,float MAX_ERROR_INTEGRAL);
//更新[计算输出]PID
void updatePID(PID* pid,float input);
//设置PID参数
void setPIDParam(PID* pid,float kp,float ki,float kd);
//设置目标
void setPIDTarget(PID* pid,float target);



float limit(float input,float MAX_OUTPUT_ABS);


#endif // ! __PID_H

