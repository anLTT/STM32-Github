/*
由于Arduino字符是UFT-8编码，而一般汉字是GB2312/GBK/GB18030等。
建议在PlatFormIO下开发Arduino。
选用板子只要有SPI的flash字库即可。原本是基于stm32f103系列开发的，
但是stm32f401rct6淘宝才8元，而且可以直接焊接字库，实在太方便了。
字库可以选用高通GT21L16S2W字库，要W类型横置横排，才能和屏点阵排列一致.
用esp-12实验也正常，其他板子注意spi分频，分频后spi应<=30MHz
*/
#include <Arduino.h>
#include <SPI.h>
#define uchar unsigned char
//定义屏和字库引脚
#define PIN_SPI_SCK PA5
#define PIN_SPI_MISO PA6
#define PIN_SPI_MOSI PA7
#define cs2 PA4  //字库cs
#define cs1 PB0  //屏cs
#define reset PB1
//#define dc PB3
#define led PB2  //屏背光


//预先存入演示数据
uint8_t bmp_8[5][16] = {
  { 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x40, 0x20, 0x18, 0x04, 0x02, 0x42, 0x42, 0x3C, 0x00, 0x00 }, /*"S",0*/
  { 0x00, 0x00, 0x00, 0x77, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00 }, /*"Y",1*/
  { 0x00, 0x00, 0x00, 0x7C, 0x22, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x22, 0x7C, 0x00, 0x00 }, /*"D",2*/
  { 0x00, 0x00, 0x00, 0x7F, 0x42, 0x02, 0x04, 0x04, 0x08, 0x10, 0x10, 0x21, 0x21, 0x7F, 0x00, 0x00 }, /*"Z",3*/
  { 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00 }, /*"!",4*/
};
uint8_t bmp_16[5][32] = {
  { 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08,
    0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8, 0x00, 0x08, 0x00, 0x00 }, /*"山",0*/
  { 0x01, 0x00, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFC,
    0x04, 0x00, 0x04, 0x00, 0x0F, 0xF8, 0x18, 0x08, 0x28, 0x08, 0xC8, 0x08, 0x0F, 0xF8, 0x08, 0x08 }, /*"岩",1*/
  { 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x3F, 0xF8, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8,
    0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8, 0x21, 0x0A, 0x01, 0x02, 0x01, 0x02, 0x00, 0xFE }, /*"电",2*/
  { 0x00, 0x00, 0x7F, 0xF8, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x01, 0x80, 0x01, 0x00, 0xFF, 0xFE,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x05, 0x00, 0x02, 0x00 }, /*"子",3*/
  { 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
    0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 }, /*"！",4*/
};
String strhz = "显示字符串变量";



//显存
union byteTbit {       //显存共用体
  uint8_t B[4800];     //显存各点阵数据,全显存。
  uint8_t b[160][30];  //显存各点阵数据,整行，从左往右，从上到下
} VRAM;

//声明要调用的函数
void rom_read_byte_16(uint32_t fontaddr, uint8_t shuzu[]);
void rom_read_byte_32(uint32_t fontaddr, uint8_t shuzu[]);
void LCD_WR_REG(uint8_t data);
void LCD_WR_DATA(uint8_t data);
void LCD_RESET(void);
void lcd_address(uint16_t x, uint16_t y, uint16_t x_total, uint16_t y_total);
void LCD_Clear(void);
void LCD_Init(void);
void transfer_mono_data_3pixel(uint8_t mono_data);
void transfer_mono_data_24pixel(uint8_t dat1, uint8_t dat2, uint8_t dat3);
void vLcd_Clear(void);
void vLcd_8x16(int x, int y, uint8_t *Bn);
void vLcd_16x16(int x, int y, uint8_t *Bn);
void sendBuffer();

void display_GB2312_str(int x, int y, String strn);
void display_GB2312_char(int x, int y, uchar *text);


void setup() {
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);  //屏可以到72MHz，但是字库30MHz
  pinMode(cs1, OUTPUT);
  pinMode(cs2, OUTPUT);
  //pinMode(dc, OUTPUT);
  pinMode(reset, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(cs1, 1);
  digitalWrite(led, 1);
  digitalWrite(reset, 1);

  LCD_Init();
  LCD_Clear();
}


void loop() {
  vLcd_Clear();  //清显存
  //显示字节数组图片
  vLcd_8x16(4, 0, bmp_8[0]);
  vLcd_8x16(12, 0, bmp_8[1]);
  vLcd_8x16(20, 0, bmp_8[2]);
  vLcd_8x16(28, 0, bmp_8[3]);
  vLcd_8x16(36, 0, bmp_8[4]);
  vLcd_16x16(0, 16, bmp_16[0]);
  vLcd_16x16(16, 16, bmp_16[1]);
  vLcd_16x16(32, 16, bmp_16[2]);
  vLcd_16x16(48, 16, bmp_16[3]);
  vLcd_16x16(64, 16, bmp_16[4]);

  //显示字符串变量
  display_GB2312_str(0, 32, strhz);

  //直接显示字符串
  display_GB2312_str(0, 48, "一直觉得，山岩电子老板不是好人");
  display_GB2312_str(0, 64, "天天只知道开车，也不发点福利!");
  display_GB2312_str(0, 112, "STM32f103C8T6板+GT21L16S2W字库");
  display_GB2312_str(0, 128, "用PlatFormIO开发Arduino代码");
  display_GB2312_str(0, 144, "先设置GB2312/GBK编码再复制代码");

  //把显存写入屏内
  sendBuffer();

  delay(1000);
}



//从字库读取32字节--ASCII码
void rom_read_byte_16(uint32_t fontaddr, uint8_t shuzu[]) {
  digitalWrite(cs2, 0);
  SPI.transfer(0x0B);
  SPI.transfer((fontaddr & 0xff0000) >> 16);
  SPI.transfer((fontaddr & 0xff00) >> 8);
  SPI.transfer(fontaddr & 0xff);
  SPI.transfer(0x00);
  for (uint8_t i = 0; i < 16; i++) {
    shuzu[i] = SPI.transfer(0x00);
  }
  digitalWrite(cs2, 1);
}
//从字库读取32字节--汉字
void rom_read_byte_32(uint32_t fontaddr, uint8_t shuzu[]) {
  digitalWrite(cs2, 0);
  SPI.transfer(0x0B);
  SPI.transfer(fontaddr >> 16);
  SPI.transfer(fontaddr >> 8);
  SPI.transfer(fontaddr);
  SPI.transfer(0x00);
  for (uint8_t i = 0; i < 32; i++) {
    shuzu[i] = SPI.transfer(0);
  }
  digitalWrite(cs2, 1);
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

//把字符串写入显存-输入字符数组型
void display_GB2312_char(int x, int y, uchar *text) {
  uint32_t fontaddr;
  uint8_t zi32[32] = { 0 };
  uint8_t zi16[16] = { 0 };
  uint8_t i = 0;
  while ((text[i] > 0x00)) {
    if (((text[i] >= 0xb0) && (text[i] <= 0xf7)) && (text[i + 1] >= 0xa1)) {  //汉字
      fontaddr = (text[i] - 0xb0) * 94;
      fontaddr += (text[i + 1] - 0xa1) + 846;
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if (((text[i] >= 0xa1) && (text[i] <= 0xa3)) && (text[i + 1] >= 0xa1)) {  //扩展
      fontaddr = (text[i] - 0xa1) * 94;
      fontaddr += (text[i + 1] - 0xa1);
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {  //ASCII码
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 16);
      fontaddr = (unsigned long)(fontaddr + 0x3B7C0);
      rom_read_byte_16(fontaddr, zi16);
      vLcd_8x16(x, y, zi16);
      i += 1;
      x += 8;
    }
  }
}
//把字符串写入显存-输入字符串型
void display_GB2312_str(int x, int y, String strn) {
  const char *text = strn.c_str();
  uint32_t fontaddr;
  uint8_t zi32[32] = { 0 };
  uint8_t zi16[16] = { 0 };
  uint8_t i = 0;
  while ((text[i] > 0x00)) {
    if (((text[i] >= 0xb0) && (text[i] <= 0xf7)) && (text[i + 1] >= 0xa1)) {  //汉字
      fontaddr = (text[i] - 0xb0) * 94;
      fontaddr += (text[i + 1] - 0xa1) + 846;
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if (((text[i] >= 0xa1) && (text[i] <= 0xa3)) && (text[i + 1] >= 0xa1)) {  //扩展
      fontaddr = (text[i] - 0xa1) * 94;
      fontaddr += (text[i + 1] - 0xa1);
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {  //ASCII码
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 16);
      fontaddr = (unsigned long)(fontaddr + 0x3B7C0);
      rom_read_byte_16(fontaddr, zi16);
      vLcd_8x16(x, y, zi16);
      i += 1;
      x += 8;
    }
  }
}

//任意坐标开始，//把字节数组写入显存，坐标0开头
void vLcd_8x16(int x, int y, uint8_t *Bn) {
  int index_B = x / 8;  //当行字节序号；
  int index_b = x % 8;  //当字节位序

  for (uint8_t i = 0; i < 16; i++) {
    VRAM.b[y + i][index_B + 0] = VRAM.b[y + i][index_B] | (Bn[i] >> index_b);            //当前位置后半段取字节前半段，右移
    VRAM.b[y + i][index_B + 1] = VRAM.b[y + i][index_B + 1] | (Bn[i] << (8 - index_b));  //当前位置前半段取字节位置后半段，左移剩下位
  }
}
void vLcd_16x16(int x, int y, uint8_t *Bn) {
  int index_B = x / 8;
  int index_b = x % 8;

  for (uint8_t i = 0; i < 16; i++) {
    VRAM.b[y + i][index_B + 0] = VRAM.b[y + i][index_B + 0] | (Bn[i * 2] >> index_b);
    VRAM.b[y + i][index_B + 1] = VRAM.b[y + i][index_B + 1] | (Bn[i * 2] << (8 - index_b));
    VRAM.b[y + i][index_B + 1] = VRAM.b[y + i][index_B + 1] | (Bn[i * 2 + 1] >> index_b);
    VRAM.b[y + i][index_B + 2] = VRAM.b[y + i][index_B + 2] | (Bn[i * 2 + 1] << (8 - index_b));
  }
}


//单字节,取3位，转化成一个灰度字节
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
//字节接力,3字节转化24个点：
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
//刷屏，全显存写入屏幕
void sendBuffer() {
  uint16_t i, j;
  uint8_t dat1, dat2, dat3;
  uint8_t *dp = VRAM.B;
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
//清零显存
void vLcd_Clear(void) {
  memset(VRAM.B, 0x00, sizeof(VRAM.B));
}
//清屏
void LCD_Clear(void) {
  int i, j;
  memset(VRAM.B, 0x00, sizeof(VRAM.B));
  lcd_address(0, 0, 240, 160);
  LCD_WR_REG(0x2c);  //写显示数据

  for (i = 0; i < 160; i++) {
    for (j = 0; j < 10; j++) {
      transfer_mono_data_24pixel(0x00, 0x00, 0x00);  //每个字节显示 8 个点阵，显示 8*3=24 个点阵
    }
  }
}
//设置屏的边界
void lcd_address(uint16_t x, uint16_t y, uint16_t x_total, uint16_t y_total) {
  uint16_t x_end, y_end;
  x_end = x + (x_total - 1) / 3;
  y_end = y + y_total - 1;
  LCD_WR_REG(0x2a);  //x起始
  LCD_WR_DATA(highByte(x));
  LCD_WR_DATA(lowByte(x));
  LCD_WR_DATA(highByte(x_end));
  LCD_WR_DATA(lowByte(x_end));
  LCD_WR_REG(0x2b);  //y起始
  LCD_WR_DATA(highByte(y));
  LCD_WR_DATA(lowByte(y));
  LCD_WR_DATA(highByte(y_end));
  LCD_WR_DATA(lowByte(y_end));
}
//屏复位
void LCD_RESET(void) {
  digitalWrite(reset, 0);
  delay(100);
  digitalWrite(reset, 1);
  delay(50);
}
//屏初始化
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
  digitalWrite(led, 0);  //点亮背光
  LCD_Clear();           //清全屏白色
}
