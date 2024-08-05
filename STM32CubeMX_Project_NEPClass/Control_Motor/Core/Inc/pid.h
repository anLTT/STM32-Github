#ifndef __pid_H
#define __pid_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
 
enum{
    LLAST	= 0,
    LAST 	= 1,
    NOW 	= 2,
    
    POSITION_PID,
    DELTA_PID,//PID相关枚举类型
};
typedef struct __pid_t
{
    float p;
    float i;
    float d;
    
    float set[3];				//目标值,包含NOW， LAST， LLAST上上次
    float get[3];				//测量值
    float err[3];				//误差
	
    
    float pout;					//p输出
    float iout;					//i输出
    float dout;					//d输出
    
    float pos_out;				//本次位置式输出
    float last_pos_out;			//上次输出
    float delta_u;				//本次增量值
    float delta_out;			//本次增量式输出 = last_delta_out + delta_u
    float last_delta_out;
    
	float max_err;
	float deadband;				//err < deadband return
    uint32_t pid_mode;
    uint32_t MaxOutput;			//输出限幅
    uint32_t IntegralLimit;		//积分限幅
    
    void (*f_param_init)(struct __pid_t *pid,  //PID参数初始化
                        uint32_t pid_mode,
                        uint32_t maxOutput,
                        uint32_t integralLimit,
                        float p,
                        float i,
                        float d);
    void (*f_pid_reset)(struct __pid_t *pid, float p, float i, float d);//pid三个参数修改
	float (*f_cal_pid)(struct __pid_t*, float, float); // 函数指针成员
    float (*f_cal_sp_pid)(struct __pid_t*, float, float, float); // 函数指针成员
						
}pid_t;//PID相关结构体定义
 
void PID_struct_init(
    pid_t* pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    float 	kp, 
    float 	ki, 
    float 	kd);//初始化函数申明
    
float pid_calc(pid_t* pid, float fdb, float ref);//PID输出值计算
    
extern pid_t pid_rol;
extern pid_t pid_pit;
extern pid_t pid_yaw;
extern pid_t pid_pit_omg;
extern pid_t pid_yaw_omg;	
extern pid_t pid_spd[4];
extern pid_t pid_yaw_alfa;
extern pid_t pid_chassis_angle;
extern pid_t pid_poke;
extern pid_t pid_poke_omg;
extern pid_t pid_imu_tmp;		//imu_temperature
extern pid_t pid_cali_bby;	//big buff yaw
extern pid_t pid_cali_bbp;
extern pid_t pid_omg;
extern pid_t pid_pos;
#endif


