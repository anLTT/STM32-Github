#include "My_IIC.h"
#include "gpio.h"

//*********************************************************************************
//IIC����
//
//*********************************************************************************
 void MY_delay_us__(const uint32_t ucus)
{
    register uint32_t ucustemp = ucus;
    while(ucustemp--)
    {
       (void)0;
    }
}
 
//��ʼ��IIC
void IIC_Init(void)
{					     
	stc_gpio_cfg_t stcGpioCfg;
	DDL_ZERO_STRUCT(stcGpioCfg);
	///< ��GPIO����ʱ���ſ�
  Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);	
	
  stcGpioCfg.enDir = GpioDirOut;   //IO���
	stcGpioCfg.enDrv = GpioDrvH;		 //< �˿�������������->����������
  stcGpioCfg.enPu = GpioPuDisable; //������
  stcGpioCfg.enPd = GpioPdDisable; //������
	stcGpioCfg.enOD = GpioOdDisable; //< �˿ڿ�©�������->��©����ر�
  
  Gpio_Init(IIC_SCL_PORT, IIC_SCL_PIN, &stcGpioCfg); //�˿ڳ�ʼ�� SCL
	Gpio_Init(IIC_SDA_PORT, IIC_SDA_PIN, &stcGpioCfg); //�˿ڳ�ʼ�� SDA
	
	Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL �����
	Gpio_SetIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA �����

}

//IO��������
void SDA_IN()  
{ 
	stc_gpio_cfg_t stcGpioCfg;
	
	stcGpioCfg.enDir =GpioDirIn;
	stcGpioCfg.enDrv = GpioDrvH;		 //< �˿�������������->����������
  stcGpioCfg.enPu = GpioPuDisable; //������
  stcGpioCfg.enPd = GpioPdDisable; //������
 	Gpio_Init(IIC_SDA_PORT,IIC_SDA_PIN, &stcGpioCfg);
}
void SDA_OUT()
{ 
	stc_gpio_cfg_t stcGpioCfg;
	
	stcGpioCfg.enDir = GpioDirOut;   //IO���
	stcGpioCfg.enDrv = GpioDrvH;		 //< �˿�������������->����������
  stcGpioCfg.enPu = GpioPuDisable; //������
  stcGpioCfg.enPd = GpioPdDisable; //������
	stcGpioCfg.enOD = GpioOdDisable; //< �˿ڿ�©�������->��©����ر�
	
	Gpio_Init(IIC_SDA_PORT,IIC_SDA_PIN, &stcGpioCfg);
	
  Gpio_SetIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA �����
//	IIC_SDA=1;
}


//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
//	IIC_SDA=1;	  	  
//	IIC_SCL=1;
//	delay_us(4);
  Gpio_SetIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=1
	Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=1
	MY_delay_us__(4);
	
// 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
//	delay_us(4);
//	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
	Gpio_ClrIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=0
	MY_delay_us__(4);
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
//	IIC_SCL=0;
//	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
// 	delay_us(4);
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
	Gpio_ClrIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=0
	MY_delay_us__(4);
	
//	IIC_SCL=1; 
//	IIC_SDA=1;//����I2C���߽����ź�
//	delay_us(4);
	Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=1
	Gpio_SetIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=1
	MY_delay_us__(4);	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
//	IIC_SDA=1;delay_us(2);	   
//	IIC_SCL=1;delay_us(2);
  Gpio_SetIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=1
	//delay 2 us
	Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=1
	//delay 2 us
	
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
//	IIC_SCL=0;//ʱ�����0 	   
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
//	IIC_SCL=0;
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
	SDA_OUT();
//	IIC_SDA=0;
//	delay_us(2);
//	IIC_SCL=1;
//	delay_us(2);
//	IIC_SCL=0;
	Gpio_ClrIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=0
	//delay 2 us
	Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=1
	//delay 2 us	
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
}
//������ACKӦ��		    
void IIC_NAck(void)
{
//	IIC_SCL=0;
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
	SDA_OUT();
	
//	IIC_SDA=1;
//	delay_us(2);
//	IIC_SCL=1;
//	delay_us(2);
//	IIC_SCL=0;
	
  Gpio_SetIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=1
	//delay 2 us	
	Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=1
	//delay 2 us		
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
		SDA_OUT(); 	    
//    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
    for(t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			//IIC_SDA=1;
		  Gpio_SetIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=1
		else
			//IIC_SDA=0;
		  Gpio_ClrIO(IIC_SDA_PORT, IIC_SDA_PIN); // SDA=0
		txd<<=1; 	  
//		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
//		IIC_SCL=1;
//		delay_us(2); 
//		IIC_SCL=0;	
//		delay_us(2);
		MY_delay_us__(2);
		Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=1
		MY_delay_us__(2);
		Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
		MY_delay_us__(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
    Gpio_ClrIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=0
		MY_delay_us__(0);
    Gpio_SetIO(IIC_SCL_PORT, IIC_SCL_PIN); // SCL=1    
		//IIC_SCL=0; 
    //delay_us(2);
		//IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		//delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



























