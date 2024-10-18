#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "ds1302.h"
#include "ds18b20.h"
#include "usart1.h"
#include "led.h"
#include "delay.h"
#include "max30102_read.h"
#include "myiic.h"
#include "key.h"
#include "iic.h"
#include "stdio.h"
#include "string.h"
#include "adxl345.h"
#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FLASH_SAVE_ADDR  ((u32)0x0800F000) 				//设置FLASH 保存地址(必须为偶数)

#define STM32_RX1_BUF       Usart1RecBuf 
#define STM32_Rx1Counter    RxCounter
#define STM32_RX1BUFF_SIZE  USART1_RXBUFF_SIZE

unsigned char setn=0;
unsigned char  p_r=0;		 						//平年/润年  =0表示平年，=1表示润年
float adx,ady,adz;
float acc;
float acc2,acc3;
u8 flag=0;
u16 bushu=0;
short temperature=0;
u16 xinlvMin=60,xinlvMax=120;//心率下限上限
u16 spo2Min=80;//血氧下限
u16 tempMax=373;//温度上限
u16 tempMin=150;//温度下限
char display[16];
u8 shanshuo=0;
u8 beepFlag=0x00; //蜂鸣器报警标志
u8 sendFlag = 1;
u8 tiltFlag=0;    //跌倒标志
u8 fallTime=5;    //跌倒持续的时间
u8 displayfall=0;

bool sendSetValueFlag=0;

int32_t hrAvg;//心率
int32_t spo2Avg;//血氧浓度

void DisplayTime(void)//显示时间函数
{
	  unsigned char i=0,x=0;
	  u16 nian_temp;
	
	  if(setn==0)DS1302_DateRead(&SysDate);//读时间
	  nian_temp=2000+SysDate.year;
		if((nian_temp%400==0)||((nian_temp%100!=0)&&(nian_temp%4==0)))  //判断是否为闰年
				p_r=1;
		else
				p_r=0;
	  if(setn<8)
		{
	  OLED_ShowChar((x++)*8,0,'2',2,setn+1-1);
	  OLED_ShowChar((x++)*8,0,'0',2,setn+1-1);
	  OLED_ShowChar((x++)*8,0,SysDate.year/10+'0',2,setn+1-1);
	  OLED_ShowChar((x++)*8,0,SysDate.year%10+'0',2,setn+1-1);
	  OLED_ShowChar((x++)*8,0,'-',2,0);
	  OLED_ShowChar((x++)*8,0,SysDate.mon/10+'0',2,setn+1-2);
	  OLED_ShowChar((x++)*8,0,SysDate.mon%10+'0',2,setn+1-2);
	  OLED_ShowChar((x++)*8,0,'-',2,0);
	  OLED_ShowChar((x++)*8,0,SysDate.day/10+'0',2,setn+1-3);
	  OLED_ShowChar((x++)*8,0,SysDate.day%10+'0',2,setn+1-3);
	  
	  OLED_ShowCN(i*16+88,0,0,setn+1-4);//测试显示中文：周
	  switch(SysDate.week)
    {
    case 1: 
			  OLED_ShowCN(i*16+104,0,1,setn+1-4);//测试显示中文：一
        break;

    case 2: 
			  OLED_ShowCN(i*16+104,0,2,setn+1-4);//测试显示中文：二
        break;

    case 3: 
			  OLED_ShowCN(i*16+104,0,3,setn+1-4);//测试显示中文：三
        break;

    case 4: 
			  OLED_ShowCN(i*16+104,0,4,setn+1-4);//测试显示中文：四
        break;

    case 5: 
			  OLED_ShowCN(i*16+104,0,i+5,setn+1-4);//测试显示中文：五
        break;

    case 6: 
			  OLED_ShowCN(i*16+104,0,6,setn+1-4);//测试显示中文：六
        break;

    case 7: 
			  OLED_ShowCN(i*16+104,0,7,setn+1-4);//测试显示中文：日
        break;
    }
    x=0;
		
	  OLED_ShowChar((x++)*8,2,SysDate.hour/10+'0',2,setn+1-5);
	  OLED_ShowChar((x++)*8,2,SysDate.hour%10+'0',2,setn+1-5);
	  OLED_ShowChar((x++)*8,2,':',2,0);
	  OLED_ShowChar((x++)*8,2,SysDate.min/10+'0',2,setn+1-6);
	  OLED_ShowChar((x++)*8,2,SysDate.min%10+'0',2,setn+1-6);
	  OLED_ShowChar((x++)*8,2,':',2,0);
	  OLED_ShowChar((x++)*8,2,SysDate.sec/10+'0',2,setn+1-7);
	  OLED_ShowChar((x++)*8,2,SysDate.sec%10+'0',2,setn+1-7);
	}
}

void GetHeartRateSpO2(void)
{
	unsigned char x=0;
	
	ReadHeartRateSpO2();  //读取心率血氧
	
	//显示相关的信息
	if(((hrAvg!=0)&&(hrAvg>=xinlvMax||hrAvg<=xinlvMin))&&shanshuo==1)
	{
			 OLED_ShowChar((x++)*8,6,' ',2,0);
			 OLED_ShowChar((x++)*8,6,' ',2,0);
			 OLED_ShowChar((x++)*8,6,' ',2,0);
	}
	else
	{
				//显示相关的信息
			OLED_ShowChar((x++)*8,6,hrAvg%1000/100+'0',2,0);
			OLED_ShowChar((x++)*8,6,hrAvg%100/10+'0',2,0);
			OLED_ShowChar((x++)*8,6,hrAvg%10+'0',2,0);
	}
	x=6;
	if(((spo2Avg!=0)&&(spo2Avg<=spo2Min))&&shanshuo==1)
	{
			 OLED_ShowChar((x++)*8,6,' ',2,0);
			 OLED_ShowChar((x++)*8,6,' ',2,0);
			 OLED_ShowChar((x++)*8,6,' ',2,0);
	}
	else
	{
				//显示相关的信息
			OLED_ShowChar((x++)*8,6,spo2Avg%1000/100+'0',2,0);
			OLED_ShowChar((x++)*8,6,spo2Avg%100/10+'0',2,0);
			OLED_ShowChar((x++)*8,6,spo2Avg%10+'0',2,0);
	}
}

void DisplayTemperature(void)//显示温度函数
{
	  unsigned char x=10;//显示的第几个字符
		
	  temperature=DS18B20_Get_Temp();

		if((temperature<=tempMin||temperature>=tempMax)&&shanshuo==1)
		{
				 OLED_ShowChar((x++)*8,2,' ',2,0);
				 OLED_ShowChar((x++)*8,2,' ',2,0);
				 OLED_ShowChar((x++)*8,2,' ',2,0);
				 OLED_ShowChar((x++)*8,2,' ',2,0);
		}
		else
		{
				OLED_ShowChar((x++)*8,2,temperature/100+'0',2,0);
				OLED_ShowChar((x++)*8,2,temperature%100/10+'0',2,0);
				OLED_ShowChar((x++)*8,2,'.',2,0);
				OLED_ShowChar((x++)*8,2,temperature%10+'0',2,0);
		}
}

void Store_Data(void) //存储数据
{
	  u16 DATA_BUF[6];
	
		DATA_BUF[0] =   bushu;
    DATA_BUF[1] =   xinlvMin;   
	  DATA_BUF[2] =   xinlvMax;
	  DATA_BUF[3] =   spo2Min;
	  DATA_BUF[4] =   tempMin;
	  DATA_BUF[5] =   tempMax;
	
	  STMFLASH_Write(FLASH_SAVE_ADDR + 0x20,(u16*)DATA_BUF,6); //写入数据
	
	  DelayMs(50);
}

void Read_Data(void) //读出数据
{
	   u16 DATA_BUF[10];
	
	   STM32F10x_Read(FLASH_SAVE_ADDR + 0x20,(u16*)DATA_BUF,6); //读取数据
	
	   bushu         =  DATA_BUF[0];  
	   xinlvMin      =  DATA_BUF[1];  
	   xinlvMax      =  DATA_BUF[2]; 
	   spo2Min       =  DATA_BUF[3]; 
	   tempMin       =  DATA_BUF[4];  
	   tempMax       =  DATA_BUF[5]; 
}

void CheckNewMcu(void)  // 检查是否是新的单片机，是的话清空存储区，否则保留
{
	  u8 comper_str[6];
		
	  STM32F10x_Read(FLASH_SAVE_ADDR,(u16*)comper_str,5);
	  comper_str[5] = '\0';
	  if(strstr((char *)comper_str,"FDYDZ") == NULL)  //新的单片机
		{
			 STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)"FDYDZ",5); //写入“FDYDZ”，方便下次校验
			 DelayMs(50);
			 Store_Data();//存储下初始数据
			 DelayMs(50);
	  }
		Read_Data(); //开机读取下相关数据（步数里程卡路里步长）
}

void GetSteps(void)//获取步数函数
{
	  static u16 temp=0;
	  u16 x=11; 
	
	  adxl345_read_average(&adx,&ady,&adz,10);//获取数据
		acc=ady;
		acc2=ady;
	  acc3=adx;
	  if(acc2<0)acc2=-acc2;
    if(acc3<0)acc3=-acc3;
	
		if(((u16)acc2)>=190 || ((u16)acc3)>=190)//检测到倾斜
		{
			  tiltFlag=1;
		}
		else
		{

				tiltFlag=0;
				fallTime=5;
        displayfall=0;
		}
		
		if(acc>0)//在正轴
		{
			if(acc/10>=4&&flag==1)//加速度值，在正轴值是否大于5，并且flag为1，则视为一个周期完成，步数加1
			{
				flag=0;	
				if(bushu<60000)bushu++;	//步数加1
				if(temp!=bushu)//当步数发生变化才去存储步数
				{
						temp=bushu;
					  Store_Data(); //存储步数
				}
			}
		}
		if(acc<0)//在负轴
		{
			acc=-acc;
			if(acc/10>=4)//加速度值，在负轴是否小于-5
			{
					flag=1;//falg置1
			}
		}
		
		if(bushu>9999)
		{
				OLED_ShowChar((x++)*8,6,bushu/10000+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%10000/1000+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%1000/100+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%100/10+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%10+'0',2,0);
		}
		else if(bushu>999)
		{
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,bushu%10000/1000+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%1000/100+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%100/10+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%10+'0',2,0);
		}
		else if(bushu>99)
		{
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,bushu%1000/100+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%100/10+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%10+'0',2,0);
		}
		else if(bushu>9)
		{
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,bushu%100/10+'0',2,0);
				OLED_ShowChar((x++)*8,6,bushu%10+'0',2,0);
				OLED_ShowChar((x++)*8,6,' ',2,0);
		}
		else
		{
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,bushu%10+'0',2,0);
				OLED_ShowChar((x++)*8,6,' ',2,0);
				OLED_ShowChar((x++)*8,6,' ',2,0);
		}
}

void DisplaySetValue(void) //显示设置值
{
		if(setn==8||setn==9)
		{
				OLED_ShowChar(48,4,xinlvMin/100+'0',2,setn+1-8);//显示
				OLED_ShowChar(56,4,xinlvMin%100/10+'0',2,setn+1-8);//显示
				OLED_ShowChar(64,4,xinlvMin%10+'0',2,setn+1-8);//显示
				OLED_ShowChar(48,6,xinlvMax/100+'0',2,setn+1-9);//显示
				OLED_ShowChar(56,6,xinlvMax%100/10+'0',2,setn+1-9);//显示
				OLED_ShowChar(64,6,xinlvMax%10+'0',2,setn+1-9);//显示
		}
		if(setn==10)
		{
				OLED_ShowChar(48,4,spo2Min/100+'0',2,setn+1-10);//显示
				OLED_ShowChar(56,4,spo2Min%100/10+'0',2,setn+1-10);//显示
				OLED_ShowChar(64,4,spo2Min%10+'0',2,setn+1-10);//显示
		}
		if(setn==11||setn==12)
		{
				OLED_ShowChar(48,4,tempMin/100+'0',2,setn+1-11);//显示
				OLED_ShowChar(56,4,tempMin%100/10+'0',2,setn+1-11);//显示
				OLED_ShowChar(64,4,'.',2,setn+1-11);
				OLED_ShowChar(72,4,tempMin%10+'0',2,setn+1-11);//显示
			
				OLED_ShowChar(48,6,tempMax/100+'0',2,setn+1-12);//显示
				OLED_ShowChar(56,6,tempMax%100/10+'0',2,setn+1-12);//显示
				OLED_ShowChar(64,6,'.',2,setn+1-12);
				OLED_ShowChar(72,6,tempMax%10+'0',2,setn+1-12);//显示
		}
}


void KeySettings(void)//按键设置函数
{
	  unsigned char keynum = 0,i;
	
	  keynum = KEY_Scan(1);//获取按键值
		if(keynum==1)//设置
		{
				setn++;

				if(setn > 12)
				{
						setn=0;
					  sendSetValueFlag=1;
						OLED_CLS();//清屏
						for(i=0;i<2;i++)OLED_ShowCN(i*16,4,i+16,1);//测试显示中文：心率
						for(i=0;i<2;i++)OLED_ShowCN(i*16+48,4,i+18,1);//测试显示中文：血氧
						for(i=0;i<2;i++)OLED_ShowCN(i*16+95,4,i+20,1);//测试显示中文：步数
						OLED_ShowCentigrade(112, 2);    //℃
						DelayMs(50);
						Store_Data();
				}
				
			  if(setn==8)
				{
						OLED_CLS();//清屏
						for(i=0;i<4;i++)OLED_ShowCN(i*16+32,0,i+22,0);//测试显示中文：设置心率
					  for(i=0;i<2;i++)OLED_ShowCN(i*16,4,i+26,0);   //测试显示中文：下限
						for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+28,0);   //测试显示中文：上限
						OLED_ShowChar(34,4,':',2,0);
						OLED_ShowChar(34,6,':',2,0);
				}
				if(setn==10)
				{
						for(i=0;i<4;i++)OLED_ShowCN(i*16+32,0,i+30,0);//测试显示中文：设置血氧
					  for(i=0;i<2;i++)OLED_ShowCN(i*16,4,i+26,0);   //测试显示中文：下限
					  OLED_ShowStr(0,6,"                ", 2);
				}
				if(setn==11)
				{
						for(i=0;i<4;i++)OLED_ShowCN(i*16+32,0,i+34,0);//测试显示中文：设置温度
					  for(i=0;i<2;i++)OLED_ShowCN(i*16,4,i+26,0);   //测试显示中文：下限
						for(i=0;i<2;i++)OLED_ShowCN(i*16,6,i+28,0);   //测试显示中文：上限
						OLED_ShowChar(34,4,':',2,0);
						OLED_ShowChar(34,6,':',2,0);
				}
			
				DisplaySetValue();
		}
		if(keynum==2)//加
		{
				if(setn == 1)//设置年
				{
						SysDate.year ++;
						if(SysDate.year == 100)SysDate.year=0;
						DS1302_DateSet(&SysDate);//设置时间
				}
				if(setn == 2)//设置月
				{
						SysDate.mon ++;
						if(SysDate.mon == 13)SysDate.mon = 1;
						if((SysDate.mon==4)||(SysDate.mon==6)||(SysDate.mon==9)||(SysDate.mon==11))
						{
								if(SysDate.day>30)SysDate.day=1;
						}
						else
						{
								if(SysDate.mon==2)
								{
										if(p_r==1)
										{
												if(SysDate.day>29)
														SysDate.day=1;
										}
										else
										{
												if(SysDate.day>28)
														SysDate.day=1;
										}
								}
						}
						DS1302_DateSet(&SysDate);//设置时间
				}
				if(setn == 3)//设置日
				{
						SysDate.day ++;
						if((SysDate.mon==1)||(SysDate.mon==3)||(SysDate.mon==5)||(SysDate.mon==7)||(SysDate.mon==8)||(SysDate.mon==10)||(SysDate.mon==12))//大月
						{
								if(SysDate.day==32)//最大31天
										SysDate.day=1;//从1开始
						}
						else
						{
								if(SysDate.mon==2)//二月份
								{
										if(p_r==1)//闰年
										{
												if(SysDate.day==30)//最大29天
														SysDate.day=1;
										}
										else
										{
												if(SysDate.day==29)//最大28天
														SysDate.day=1;
										}
								}
								else
								{
										if(SysDate.day==31)//最大30天
												SysDate.day=1;
								}
						}
						DS1302_DateSet(&SysDate);//设置时间
				}
				if(setn == 4)//设置星期
				{
						SysDate.week ++;
						if(SysDate.week == 8)SysDate.week=1;
						DS1302_DateSet(&SysDate);//设置时间
				}
				if(setn == 5)//设置时
				{
						SysDate.hour ++;
						if(SysDate.hour == 24)SysDate.hour=0;
						DS1302_DateSet(&SysDate);//设置时间
				}
				if(setn == 6)//设置分
				{
						SysDate.min ++;
						if(SysDate.min == 60)SysDate.min=0;
						DS1302_DateSet(&SysDate);//设置时间
				}
				if(setn == 7)//设置秒
				{
						SysDate.sec ++;
						if(SysDate.sec == 60)SysDate.sec=0;
						DS1302_DateSet(&SysDate);//设置时间
				}
				if((setn==8)&&(xinlvMax-xinlvMin>1))xinlvMin++;
				if((setn==9)&&(xinlvMax<300))xinlvMax++;
				if((setn==10)&&(spo2Min<200))spo2Min++; 
				if((setn==11)&&(tempMax-tempMin>1))tempMin++; 
				if((setn==12)&&(tempMax<999))tempMax++; 

				DisplaySetValue();
		}

		if(keynum==3)//减
		{
				if(setn == 1)//设置年
				{
						if(SysDate.year == 0)SysDate.year=100;
						SysDate.year --;
						DS1302_DateSet(&SysDate);
				}
				if(setn == 2)//设置月
				{
						if(SysDate.mon == 1)SysDate.mon=13;
						SysDate.mon --;
						if((SysDate.mon==4)||(SysDate.mon==6)||(SysDate.mon==9)||(SysDate.mon==11))
						{
								if(SysDate.day>30)
										SysDate.day=1;
						}
						else
						{
								if(SysDate.mon==2)
								{
										if(p_r==1)
										{
												if(SysDate.day>29)
														SysDate.day=1;
										}
										else
										{
												if(SysDate.day>28)
														SysDate.day=1;
										}
								}
						}
						DS1302_DateSet(&SysDate);
				}
				if(setn == 3)//设置日
				{
						SysDate.day --;
						if((SysDate.mon==1)||(SysDate.mon==3)||(SysDate.mon==5)||(SysDate.mon==7)||(SysDate.mon==8)||(SysDate.mon==10)||(SysDate.mon==12))
						{
								if(SysDate.day==0)
										SysDate.day=31;
						}
						else
						{
								if(SysDate.mon==2)
								{
										if(p_r==1)
										{
												if(SysDate.day==0)
														SysDate.day=29;
										}
										else
										{
												if(SysDate.day==0)
														SysDate.day=28;
										}
								}
								else
								{
										if(SysDate.day==0)
												SysDate.day=30;
								}
						}
						DS1302_DateSet(&SysDate);
				}
				if(setn == 4)//设置星期
				{
						if(SysDate.week == 1)SysDate.week=8;
						SysDate.week --;
						DS1302_DateSet(&SysDate);
				}
				if(setn == 5)//设置时
				{
						if(SysDate.hour == 0)SysDate.hour=24;
						SysDate.hour --;
						DS1302_DateSet(&SysDate);
				}
				if(setn == 6)//设置分
				{
						if(SysDate.min == 0)SysDate.min=60;
						SysDate.min --;
						DS1302_DateSet(&SysDate);
				}
				if(setn == 7)//设置秒
				{
						if(SysDate.sec == 0)SysDate.sec=60;
						SysDate.sec --;
						DS1302_DateSet(&SysDate);
				}
				if((setn==8)&&(xinlvMin>0))xinlvMin--;
				if((setn==9)&&(xinlvMax-xinlvMin>1))xinlvMax--;
				if((setn==10)&&(spo2Min>0))spo2Min--; 
				if((setn==11)&&(tempMin>0))tempMin--; 
				if((setn==12)&&(tempMax-tempMin>1))tempMax--; 

				DisplaySetValue();
		}
		if(keynum==4)//步数清零
		{
			 bushu = 0;
			 Store_Data();//存储数据
		}
}

void UsartSendReceiveData(void)   //串口发送接收数据
{
	  char *str1=0,i;
	  int  setValue=0;
	  char setvalue[4]={0};
	
	  if(STM32_Rx1Counter > 0)
		{
			  DelayMs(20);
			  
			  if(strstr(STM32_RX1_BUF,"setHeartMin:")!=NULL)    //接收到设置心率下限的指令
				{
						str1 = strstr(STM32_RX1_BUF,"setHeartMin:");
					  
					  while(*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
						{
								str1 = str1 + 1;
								DelayMs(10);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
								if(*str1 == '\r')break;            //换行符，直接退出while循环
								DelayMs(10);
						}
						setvalue[i] = '\0';            //加上结尾符
						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=300)
						{
								xinlvMin = setValue;    //设置的心率下限
							  DisplaySetValue();
							  Store_Data();//存储数据
						}
				}
			
				if(strstr(STM32_RX1_BUF,"setHeartMax:")!=NULL)		//接收到设置心率上限的指令
				{
						str1 = strstr(STM32_RX1_BUF,"setHeartMax:");
					  
					  while(*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
						{
								str1 = str1 + 1;
								DelayMs(10);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
								if(*str1 == '\r')break;            //换行符，直接退出while循环
								DelayMs(10);
						}
						setvalue[i] = '\0';            //加上结尾符
						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=300)
						{
								xinlvMax = setValue;    //设置的心率上限
							  DisplaySetValue();
							  Store_Data();//存储数据
						}
				}
				
				if(strstr(STM32_RX1_BUF,"setSpo2Min:")!=NULL)  //接收到设置血氧下限的指令
				{
						str1 = strstr(STM32_RX1_BUF,"setSpo2Min:");
					  
					  while(*str1 != ':')        //判断是不是0到9有效数字
						{
								str1 = str1 + 1;
								DelayMs(10);
						}
						str1 = str1 + 1;
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
								if(*str1 == '\r')break;            //换行符，直接退出while循环
								DelayMs(10);
						}
						setvalue[i] = '\0';            //加上结尾符
						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=200)
						{
								spo2Min = setValue;    //设置的血氧下限
							  DisplaySetValue();
							  Store_Data();//存储数据
						}
				}
				
				if(strstr(STM32_RX1_BUF,"setTempMin:")!=NULL)  //接收到设置温度下限的指令
				{
						str1 = strstr(STM32_RX1_BUF,"setTempMin:");
					  
					  while(*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
						{
								str1 = str1 + 1;
								DelayMs(10);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
								if(*str1 == '.')break;            //换行符，直接退出while循环
								DelayMs(10);
						}
						if(*str1 == '.')
						{
								str1 = str1 + 1;
						}
						if(*str1 >= '0' && *str1 <= '9')
						{
								setvalue[i] = *str1;
						}
						i = i + 1;
						setvalue[i] = '\0';            //加上结尾符
						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=999)
						{
								tempMin = setValue;    //设置的温度下限
							  DisplaySetValue();
							  Store_Data();//存储数据
						}
				}
				
				if(strstr(STM32_RX1_BUF,"setTempMax:")!=NULL)  //接收到设置温度上限的指令
				{
						str1 = strstr(STM32_RX1_BUF,"setTempMax:");
					  
					  while(*str1 < '0' || *str1 > '9')        //判断是不是0到9有效数字
						{
								str1 = str1 + 1;
								DelayMs(10);
						}
						i = 0;
						while(*str1 >= '0' && *str1 <= '9')        //判断是不是0到9有效数字
						{
								setvalue[i] = *str1;
								i ++; str1 ++;
								if(*str1 == '.')break;            //换行符，直接退出while循环
								DelayMs(10);
						}
						if(*str1 == '.')
						{
								str1 = str1 + 1;
						}
						if(*str1 >= '0' && *str1 <= '9')
						{
								setvalue[i] = *str1;
						}
						i = i + 1;
						setvalue[i] = '\0';            //加上结尾符
						setValue = atoi(setvalue);
						if(setValue>=0 && setValue<=999)
						{
								tempMax = setValue;    //设置的温度上限
							  DisplaySetValue();
							  Store_Data();//存储数据
						}
				}
				
				if(strstr(STM32_RX1_BUF,"stepsClear")!=NULL)  //收到步数清零指令
				{
					  BEEP = 1;
						DelayMs(80);
					  BEEP = 0;
					
						bushu = 0;
						Store_Data();  //存储数据
				}		
				
				memset(STM32_RX1_BUF, 0, STM32_RX1BUFF_SIZE);//清除缓存
				STM32_Rx1Counter = 0;
		}
	
	  /* 蓝牙发送数据 */
	  if(sendFlag==1)
		{
			  sendFlag = 0;
				printf("$Heartrate:%d#,$Spo2:%d#,$Temperature:%4.1f#,",hrAvg,spo2Avg,(float)temperature/10);
				printf("$Steps:%d#",bushu);
		    if(fallTime==0)printf("fall");
			  if(sendSetValueFlag==1)
				{
						sendSetValueFlag=0;
					  DelayMs(200);
					  printf("$setHeartMin:%d#,",xinlvMin);
						printf("$setHeartMax:%d#,",xinlvMax);
						printf("$setSpo2Min:%d#,",spo2Min);
						printf("$setTempMin:%4.1f#,",(float)tempMin/10);
						printf("$setTempMax:%4.1f#,",(float)tempMax/10);
				}
		}
}

int main(void)
{
	unsigned char i;
	
	DelayInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	I2C_Configuration(); //IIC初始化
	OLED_Init(); //OLED初始化
	KEY_Init(); //按键初始化
	DelayMs(200);
	CheckNewMcu();
  OLED_CLS();//清屏
	DS18B20_Init();
	DS1302_Init(&SysDate); 
	DelayMs(100);
	for(i=0;i<8;i++)OLED_ShowCN(i*16,2,i+8,0);//测试显示中文：欢迎使用使能手环
	DelayMs(1000);
	OLED_CLS();//清屏
	DS1302_DateRead(&SysDate);//读时间
	OLED_CLS();//清屏
	for(i=0;i<2;i++)OLED_ShowCN(i*16,4,i+16,1);//测试显示中文：心率
	for(i=0;i<2;i++)OLED_ShowCN(i*16+48,4,i+18,1);//测试显示中文：血氧
	for(i=0;i<2;i++)OLED_ShowCN(i*16+95,4,i+20,1);//测试显示中文：步数
	OLED_ShowCentigrade(112, 2);    //℃
	IIC_init();//IIC初始化
	uart1_Init(9600);
	adxl345_init();//ADXL345初始化
	Init_MAX30102();//MAX30102初始化
	TIM2_Init(99,719);   //定时器初始化，定时1ms
	//Tout = ((arr+1)*(psc+1))/Tclk ; 
	//Tclk:定时器输入频率(单位MHZ)
	//Tout:定时器溢出时间(单位us)
	while(1)
	{
		  shanshuo=!shanshuo;
		  KeySettings();
	    DisplayTime();
		  if(setn == 0)//不在设置状态下，读取相关数据
			{
					DisplayTemperature();
					GetSteps();
					GetHeartRateSpO2();
				  if(fallTime==0)
					{
							if(displayfall==0)
							{
									displayfall=1;
								  OLED_CLS();//清屏
								  for(i=0;i<3;i++)OLED_ShowCN(i*16+48,2,i+64,0);//测试显示中文：跌倒
								  UsartSendReceiveData();
								  DelayMs(1000);DelayMs(1000);
								  OLED_CLS();//清屏
									for(i=0;i<2;i++)OLED_ShowCN(i*16,4,i+16,1);//测试显示中文：心率
									for(i=0;i<2;i++)OLED_ShowCN(i*16+48,4,i+18,1);//测试显示中文：血氧
									for(i=0;i<2;i++)OLED_ShowCN(i*16+95,4,i+20,1);//测试显示中文：步数
									OLED_ShowCentigrade(112, 2);    //℃
							}
					}
			}
			UsartSendReceiveData();
			DelayMs(10);
	}
}

void TIM2_IRQHandler(void)//定时器2中断服务程序，用于记录时间
{ 
	  static u16 timeCount1 = 0;
	  static u16 timeCount3 = 0;
	
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
		{ 
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断标志位  
				
			  timeCount1 ++;
				if(timeCount1 >= 800)  
				{
						timeCount1 = 0;
						 sendFlag = 1;
					
					  if(tiltFlag)
						{
								if(fallTime>0)fallTime--;  //跌倒时间倒计时
						}
				}
			
			  timeCount3 ++;
				if(timeCount3 >= 100) 
				{
						timeCount3=0;
						if(fallTime==0){        //跌倒蜂鸣器报警
								BEEP=1;
						} 
						else
						{
								if(((hrAvg!=0)&&(hrAvg>=xinlvMax||hrAvg<=xinlvMin))||((spo2Avg!=0)&&(spo2Avg<=spo2Min))||(temperature>=tempMax||temperature<=tempMin))//不在范围蜂鸣器报警
								{
									BEEP=~BEEP;
									beepFlag|=0x01;
									
								}else
								{	
									beepFlag&=0xFE;
									BEEP=0;
								}
						}
				}
		}
}
