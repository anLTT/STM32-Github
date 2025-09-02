#include <Arduino.h>
#include <SPI.h>
#define PIN_SPI_SCK PA5
#define PIN_SPI_MISO PA6
#define PIN_SPI_MOSI PA7
#define cs1 PA4
#define dc PB0
#define reset PB1
#define cs2 PA3
#define led PC13
#include "tupian.h"
#define USE_HORIZONTAL 0  //定义液晶屏顺时针旋转方向 	0-0度旋转，1-90度旋转，2-180度旋转，3-270度旋转
uint8_t VRAM_buf[4800];   //显存各点阵数据。整行，从左往右，从上到下
bool VRAM_bit[160][240];  //显存各点阵数据
//HardwareSerial Serial1(PA1, PA0);
void transfer_mono_data_24pixel(uint8_t dat1, uint8_t dat2, uint8_t dat3);
//spi读一个字节
uint8_t spiReadByte(uint8_t data) {
  uint8_t fromspi;
  digitalWrite(cs1, 0);
  fromspi = SPI.transfer(data);
  digitalWrite(cs1, 1);
  return fromspi;
}
//spi写一个字节
void spiWriteByte(uint8_t data) {
  digitalWrite(cs1, 0);
  SPI.transfer(data);
  digitalWrite(cs1, 1);
}
//屏写命令
void LCD_WR_REG(uint8_t data) {
  digitalWrite(cs1, 0);
  uint8_t cmd = 0;
  uint16_t Data = ((cmd << 15) | (data << 7));
  SPI.transfer(highByte(Data));
  SPI.transfer(lowByte(Data));
  digitalWrite(cs1, 1);
}
//屏写数据
void LCD_WR_DATA(uint8_t data) {
  digitalWrite(cs1, 0);
  uint8_t cmd = 1;
  uint16_t Data = ((cmd << 15) | (data << 7));
  SPI.transfer(highByte(Data));
  SPI.transfer(lowByte(Data));
  digitalWrite(cs1, 1);
}

void LCD_RESET(void) {
  digitalWrite(reset, 0);
  delay(100);
  digitalWrite(reset, 1);
  delay(50);
}

void lcd_address(uint16_t x, uint16_t y, uint16_t x_total, uint16_t y_total) {
  uint16_t x_end, y_end;
  x_end = x + (x_total - 1) / 3;
  y_end = y + y_total - 1;
  LCD_WR_REG(0x2a);//x起始
  LCD_WR_DATA(highByte(x));
  LCD_WR_DATA(lowByte(x));
  LCD_WR_DATA(highByte(x_end));
  LCD_WR_DATA(lowByte(x_end));
  LCD_WR_REG(0x2b);//y起始
  LCD_WR_DATA(highByte(y));
  LCD_WR_DATA(lowByte(y));
  LCD_WR_DATA(highByte(y_end));
  LCD_WR_DATA(lowByte(y_end));
}

void LCD_Clear(void) {
  int i, j;
  lcd_address(0, 0, 240, 160);
  LCD_WR_REG(0x2c);//写显示数据

  for (i = 0; i < 160; i++) {
    for (j = 0; j < 10; j++) {
      transfer_mono_data_24pixel(0x00, 0x00, 0x00);  //每个字节显示 8 个点阵，显示 8*3=24 个点阵
    }
  }
}

void LCD_Init(void) {
  LCD_RESET();  //LCD 复位
  delay(120);
  //************* ST7586S初始化**********//
  LCD_WR_REG(0x11);   //退出睡眠模式
  LCD_WR_REG(0xC0);   // 设置 VOP
  LCD_WR_DATA(0xf1);  // 设置 VOP 的值的低 8 位（总共 9 位）,每调一级是 0.03667V
  LCD_WR_DATA(0x00);  // 设置 VOP 的值的第 9 位，也是最高一位
  LCD_WR_REG(0xC3);   // 设置 BIAS
  LCD_WR_DATA(0x02);  // 00：BIAS = 1/14,02 = 1/12,03 = 1/11,04 = 1/10,05 = 1/9
  LCD_WR_REG(0xC4);   // 设置升压倍数
  LCD_WR_DATA(0x07);  // 07：8 倍压
  LCD_WR_REG(0xD0);   // 允许模拟电路
  LCD_WR_DATA(0x1D);  // 允许模拟电路
  LCD_WR_REG(0xB5);   // N-Line = 13
  LCD_WR_DATA(0x00);  // 8d
  LCD_WR_REG(0x39);   // 0x38：设置为灰度模式； 0x39: 设置为黑白模式。
  LCD_WR_REG(0x3A);   // 允许 DDRAM 接口：单色模式、4 灰度级、16 灰度级；
  LCD_WR_DATA(0x02);  // 0x03:16 灰度级；0x02:4 灰度级或单色模式。

  LCD_WR_REG(0x36);   // 扫描顺序设置
  LCD_WR_DATA(0x00);  // 扫描顺序设置:MX=1,MY=1: 从左到右，从上到下的扫描顺序
  LCD_WR_REG(0xB0);   // Duty 设置
  LCD_WR_DATA(0x9f);  // Duty 设置:1/160
  LCD_WR_REG(0x20);   // 反显设置：OFF
  LCD_WR_REG(0xf1);   //温度补偿，温度变化改变帧频
  LCD_WR_DATA(0x15);
  LCD_WR_DATA(0x15);
  LCD_WR_DATA(0x15);
  LCD_WR_DATA(0x15);
  LCD_WR_REG(0xb1);      // 扫描起始行设置
  LCD_WR_DATA(0x00);     // 扫描起始行设置：从 COM0 开始
  LCD_WR_REG(0x29);      // 打开显示：DISPLAY ON
  digitalWrite(led, 1);  //点亮背光
  LCD_Clear();           //清全屏白色
}


void transfer_mono_data_3pixel(uint8_t mono_data) {
  uint8_t gray_data = 0;
  if (mono_data & 0x80) {  //10000000
    gray_data = 0xe0;      //二进制 11100000，就是给 D7、D6、D5 赋值
  } else {
    gray_data = 0;
  }
  mono_data <<= 1;
  if (mono_data & 0x80) {
    gray_data += 0x1c;  //二进制 00011100，就是给 D4、D3、D2 赋值
  } else
    ;
  mono_data <<= 1;
  if (mono_data & 0x80) {
    gray_data += 0x03;  //二进制 00000011，就是给 D1、D0 赋值
  } else
    ;
  LCD_WR_DATA(gray_data);  //display 3 dots (seg_N,seg_N+1,seg_N+2)
}

//显示 24 个点阵。方法一：
void transfer_mono_data_24pixel(uint8_t dat1, uint8_t dat2, uint8_t dat3) {
  transfer_mono_data_3pixel(dat1);                       //传送 dat1 的 D7\D6\D5 这 3 位,对应 3 个点阵(第 1、2、3 个）点阵会显示出来；列地址是自动+1的
  transfer_mono_data_3pixel(dat1 << 3);                  //传送 dat1 的 D4\D3\D2 这 3 位,对应 3 个点阵(第 4、5、6 个）点阵会显示出来；列地址是自动+1的
  transfer_mono_data_3pixel((dat1 << 6) | (dat2 >> 2));  //显示 dat1 的 D1、D0 和 dat2 的 D7 位，对应 3 个点阵(第 7、8、9 个）会显示出来；列地址是自动+1 的
  transfer_mono_data_3pixel(dat2 << 1);                  //传送 dat2 的 D6\D5\D4 这 3 位，对应 3 个点阵(第 10、11、12 个）个点阵会显示出来；列地址是自动+1的
  transfer_mono_data_3pixel(dat2 << 4);                  //传送 dat2 的 D3\D2\D1 这 3 位，对应 3 个点阵(第 13、14、15 个）个点阵会显示出来；列地址是自动+1的
  transfer_mono_data_3pixel((dat2 << 7) | (dat3 >> 1));  //传送 dat2 的 D0 和 dat3 的 D7、D6 这 3 位，对应 3 个点阵(第 16、17、18 个） 个点阵会显示出来；列地址是自动+1 的
  transfer_mono_data_3pixel(dat3 << 2);                  //传送 dat3 的 D5、D4、D3 这 3 位，对应 3 个点阵(第 19、20、21 个） 个点阵会显示出来；列地址是自动+1的
  transfer_mono_data_3pixel(dat3 << 5);                  //传送 dat3 的 D2、D1、D0 这 3 位，对应 3 个点阵(第 22、23、24 个） 个点阵会显示出来；列地址是自动+1的
}

//显示 240*160 点阵的图像，转换为专用格式数据写入屏
void disp_240x160(uint8_t *dp) {
  uint16_t i, j;
  uint8_t dat1, dat2, dat3;

  lcd_address(0, 0, 240, 160);
  LCD_WR_REG(0x2c);
  for (i = 0; i < 160; i++) {
    for (j = 0; j < 10; j++)  //循环 10 次，每次显示 24 个点阵，合计 240 个点阵
    {
      dat1 = *dp;
      dp++;
      dat2 = *dp;
      dp++;
      dat3 = *dp;
      dp++;
      transfer_mono_data_24pixel(dat1, dat2, dat3);  //每个字节显示 8 个点阵，显示 8*3=24 个点阵
    }
  }
}
//发送显存数据并显示，可以用C51格式的图片数组
void sendBuffer() {
  disp_240x160(VRAM_buf);
}


void setup() {
  Serial.begin(9600);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  pinMode(cs1, OUTPUT);
  pinMode(dc, OUTPUT);
  pinMode(reset, OUTPUT);
  pinMode(cs2, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(cs1, 1);
  digitalWrite(led, 1);
  digitalWrite(reset, 1);

  LCD_Init();
  LCD_Clear();
}

void loop() {

  LCD_Clear();
  for (uint16_t i = 0; i < 4800; i++) {
    VRAM_buf[i] = gImage_1[i];
  }
  sendBuffer();
  delay(1000);

  LCD_Clear();
  //把数据加载到显存
  for (uint16_t i = 0; i < 4800; i++) {
    VRAM_buf[i] = BMP1[i];
  }
  sendBuffer();
  delay(1000);
}
