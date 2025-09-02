/*
����Arduino�ַ���UFT-8���룬��һ�㺺����GB2312/GBK/GB18030�ȡ�
������PlatFormIO�¿���Arduino��
ѡ�ð���ֻҪ��SPI��flash�ֿ⼴�ɡ�ԭ���ǻ���stm32f103ϵ�п����ģ�
����stm32f401rct6�Ա���8Ԫ�����ҿ���ֱ�Ӻ����ֿ⣬ʵ��̫�����ˡ�
�ֿ����ѡ�ø�ͨGT21L16S2W�ֿ⣬ҪW���ͺ��ú��ţ����ܺ�����������һ��.
��esp-12ʵ��Ҳ��������������ע��spi��Ƶ����Ƶ��spiӦ<=30MHz
*/
#include <Arduino.h>
#include <SPI.h>
#define uchar unsigned char
//���������ֿ�����
#define PIN_SPI_SCK PA5
#define PIN_SPI_MISO PA6
#define PIN_SPI_MOSI PA7
#define cs2 PA4  //�ֿ�cs
#define cs1 PB0  //��cs
#define reset PB1
//#define dc PB3
#define led PB2  //������


//Ԥ�ȴ�����ʾ����
uint8_t bmp_8[5][16] = {
  { 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x40, 0x20, 0x18, 0x04, 0x02, 0x42, 0x42, 0x3C, 0x00, 0x00 }, /*"S",0*/
  { 0x00, 0x00, 0x00, 0x77, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00 }, /*"Y",1*/
  { 0x00, 0x00, 0x00, 0x7C, 0x22, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x22, 0x7C, 0x00, 0x00 }, /*"D",2*/
  { 0x00, 0x00, 0x00, 0x7F, 0x42, 0x02, 0x04, 0x04, 0x08, 0x10, 0x10, 0x21, 0x21, 0x7F, 0x00, 0x00 }, /*"Z",3*/
  { 0x00, 0x00, 0x00, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00 }, /*"!",4*/
};
uint8_t bmp_16[5][32] = {
  { 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08,
    0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8, 0x00, 0x08, 0x00, 0x00 }, /*"ɽ",0*/
  { 0x01, 0x00, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFC,
    0x04, 0x00, 0x04, 0x00, 0x0F, 0xF8, 0x18, 0x08, 0x28, 0x08, 0xC8, 0x08, 0x0F, 0xF8, 0x08, 0x08 }, /*"��",1*/
  { 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x3F, 0xF8, 0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8,
    0x21, 0x08, 0x21, 0x08, 0x21, 0x08, 0x3F, 0xF8, 0x21, 0x0A, 0x01, 0x02, 0x01, 0x02, 0x00, 0xFE }, /*"��",2*/
  { 0x00, 0x00, 0x7F, 0xF8, 0x00, 0x10, 0x00, 0x20, 0x00, 0x40, 0x01, 0x80, 0x01, 0x00, 0xFF, 0xFE,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x05, 0x00, 0x02, 0x00 }, /*"��",3*/
  { 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00,
    0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00 }, /*"��",4*/
};
String strhz = "��ʾ�ַ�������";



//�Դ�
union byteTbit {       //�Դ湲����
  uint8_t B[4800];     //�Դ����������,ȫ�Դ档
  uint8_t b[160][30];  //�Դ����������,���У��������ң����ϵ���
} VRAM;

//����Ҫ���õĺ���
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
  SPI.setClockDivider(SPI_CLOCK_DIV4);  //�����Ե�72MHz�������ֿ�30MHz
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
  vLcd_Clear();  //���Դ�
  //��ʾ�ֽ�����ͼƬ
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

  //��ʾ�ַ�������
  display_GB2312_str(0, 32, strhz);

  //ֱ����ʾ�ַ���
  display_GB2312_str(0, 48, "һֱ���ã�ɽ�ҵ����ϰ岻�Ǻ���");
  display_GB2312_str(0, 64, "����ֻ֪��������Ҳ�����㸣��!");
  display_GB2312_str(0, 112, "STM32f103C8T6��+GT21L16S2W�ֿ�");
  display_GB2312_str(0, 128, "��PlatFormIO����Arduino����");
  display_GB2312_str(0, 144, "������GB2312/GBK�����ٸ��ƴ���");

  //���Դ�д������
  sendBuffer();

  delay(1000);
}



//���ֿ��ȡ32�ֽ�--ASCII��
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
//���ֿ��ȡ32�ֽ�--����
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
//��д����
void LCD_WR_REG(uint8_t data) {
  digitalWrite(cs1, 0);
  uint8_t cmd = 0;
  uint16_t Data = ((cmd << 15) | (data << 7));
  SPI.transfer(highByte(Data));
  SPI.transfer(lowByte(Data));
  digitalWrite(cs1, 1);
}
//��д����
void LCD_WR_DATA(uint8_t data) {
  digitalWrite(cs1, 0);
  uint8_t cmd = 1;
  uint16_t Data = ((cmd << 15) | (data << 7));
  SPI.transfer(highByte(Data));
  SPI.transfer(lowByte(Data));
  digitalWrite(cs1, 1);
}

//���ַ���д���Դ�-�����ַ�������
void display_GB2312_char(int x, int y, uchar *text) {
  uint32_t fontaddr;
  uint8_t zi32[32] = { 0 };
  uint8_t zi16[16] = { 0 };
  uint8_t i = 0;
  while ((text[i] > 0x00)) {
    if (((text[i] >= 0xb0) && (text[i] <= 0xf7)) && (text[i + 1] >= 0xa1)) {  //����
      fontaddr = (text[i] - 0xb0) * 94;
      fontaddr += (text[i + 1] - 0xa1) + 846;
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if (((text[i] >= 0xa1) && (text[i] <= 0xa3)) && (text[i + 1] >= 0xa1)) {  //��չ
      fontaddr = (text[i] - 0xa1) * 94;
      fontaddr += (text[i + 1] - 0xa1);
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {  //ASCII��
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
//���ַ���д���Դ�-�����ַ�����
void display_GB2312_str(int x, int y, String strn) {
  const char *text = strn.c_str();
  uint32_t fontaddr;
  uint8_t zi32[32] = { 0 };
  uint8_t zi16[16] = { 0 };
  uint8_t i = 0;
  while ((text[i] > 0x00)) {
    if (((text[i] >= 0xb0) && (text[i] <= 0xf7)) && (text[i + 1] >= 0xa1)) {  //����
      fontaddr = (text[i] - 0xb0) * 94;
      fontaddr += (text[i + 1] - 0xa1) + 846;
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if (((text[i] >= 0xa1) && (text[i] <= 0xa3)) && (text[i + 1] >= 0xa1)) {  //��չ
      fontaddr = (text[i] - 0xa1) * 94;
      fontaddr += (text[i + 1] - 0xa1);
      fontaddr = (ulong)(fontaddr * 32);
      rom_read_byte_32(fontaddr, zi32);
      vLcd_16x16(x, y, zi32);
      i += 2;
      x += 16;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {  //ASCII��
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

//�������꿪ʼ��//���ֽ�����д���Դ棬����0��ͷ
void vLcd_8x16(int x, int y, uint8_t *Bn) {
  int index_B = x / 8;  //�����ֽ���ţ�
  int index_b = x % 8;  //���ֽ�λ��

  for (uint8_t i = 0; i < 16; i++) {
    VRAM.b[y + i][index_B + 0] = VRAM.b[y + i][index_B] | (Bn[i] >> index_b);            //��ǰλ�ú���ȡ�ֽ�ǰ��Σ�����
    VRAM.b[y + i][index_B + 1] = VRAM.b[y + i][index_B + 1] | (Bn[i] << (8 - index_b));  //��ǰλ��ǰ���ȡ�ֽ�λ�ú��Σ�����ʣ��λ
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


//���ֽ�,ȡ3λ��ת����һ���Ҷ��ֽ�
void transfer_mono_data_3pixel(uint8_t mono_data) {
  uint8_t gray_data = 0;
  if (mono_data & 0x80) {  //10000000
    gray_data = 0xe0;      //������ 11100000�����Ǹ� D7��D6��D5 ��ֵ
  } else {
    gray_data = 0;
  }
  mono_data <<= 1;
  if (mono_data & 0x80) {
    gray_data += 0x1c;  //������ 00011100�����Ǹ� D4��D3��D2 ��ֵ
  } else
    ;
  mono_data <<= 1;
  if (mono_data & 0x80) {
    gray_data += 0x03;  //������ 00000011�����Ǹ� D1��D0 ��ֵ
  } else
    ;
  LCD_WR_DATA(gray_data);  //display 3 dots (seg_N,seg_N+1,seg_N+2)
}
//�ֽڽ���,3�ֽ�ת��24���㣺
void transfer_mono_data_24pixel(uint8_t dat1, uint8_t dat2, uint8_t dat3) {
  transfer_mono_data_3pixel(dat1);                       //���� dat1 �� D7\D6\D5 �� 3 λ,��Ӧ 3 ������(�� 1��2��3 �����������ʾ�������е�ַ���Զ�+1��
  transfer_mono_data_3pixel(dat1 << 3);                  //���� dat1 �� D4\D3\D2 �� 3 λ,��Ӧ 3 ������(�� 4��5��6 �����������ʾ�������е�ַ���Զ�+1��
  transfer_mono_data_3pixel((dat1 << 6) | (dat2 >> 2));  //��ʾ dat1 �� D1��D0 �� dat2 �� D7 λ����Ӧ 3 ������(�� 7��8��9 ��������ʾ�������е�ַ���Զ�+1 ��
  transfer_mono_data_3pixel(dat2 << 1);                  //���� dat2 �� D6\D5\D4 �� 3 λ����Ӧ 3 ������(�� 10��11��12 �������������ʾ�������е�ַ���Զ�+1��
  transfer_mono_data_3pixel(dat2 << 4);                  //���� dat2 �� D3\D2\D1 �� 3 λ����Ӧ 3 ������(�� 13��14��15 �������������ʾ�������е�ַ���Զ�+1��
  transfer_mono_data_3pixel((dat2 << 7) | (dat3 >> 1));  //���� dat2 �� D0 �� dat3 �� D7��D6 �� 3 λ����Ӧ 3 ������(�� 16��17��18 ���� ���������ʾ�������е�ַ���Զ�+1 ��
  transfer_mono_data_3pixel(dat3 << 2);                  //���� dat3 �� D5��D4��D3 �� 3 λ����Ӧ 3 ������(�� 19��20��21 ���� ���������ʾ�������е�ַ���Զ�+1��
  transfer_mono_data_3pixel(dat3 << 5);                  //���� dat3 �� D2��D1��D0 �� 3 λ����Ӧ 3 ������(�� 22��23��24 ���� ���������ʾ�������е�ַ���Զ�+1��
}
//ˢ����ȫ�Դ�д����Ļ
void sendBuffer() {
  uint16_t i, j;
  uint8_t dat1, dat2, dat3;
  uint8_t *dp = VRAM.B;
  lcd_address(0, 0, 240, 160);
  LCD_WR_REG(0x2c);
  for (i = 0; i < 160; i++) {
    for (j = 0; j < 10; j++)  //ѭ�� 10 �Σ�ÿ����ʾ 24 �����󣬺ϼ� 240 ������
    {
      dat1 = *dp;
      dp++;
      dat2 = *dp;
      dp++;
      dat3 = *dp;
      dp++;
      transfer_mono_data_24pixel(dat1, dat2, dat3);  //ÿ���ֽ���ʾ 8 ��������ʾ 8*3=24 ������
    }
  }
}
//�����Դ�
void vLcd_Clear(void) {
  memset(VRAM.B, 0x00, sizeof(VRAM.B));
}
//����
void LCD_Clear(void) {
  int i, j;
  memset(VRAM.B, 0x00, sizeof(VRAM.B));
  lcd_address(0, 0, 240, 160);
  LCD_WR_REG(0x2c);  //д��ʾ����

  for (i = 0; i < 160; i++) {
    for (j = 0; j < 10; j++) {
      transfer_mono_data_24pixel(0x00, 0x00, 0x00);  //ÿ���ֽ���ʾ 8 ��������ʾ 8*3=24 ������
    }
  }
}
//�������ı߽�
void lcd_address(uint16_t x, uint16_t y, uint16_t x_total, uint16_t y_total) {
  uint16_t x_end, y_end;
  x_end = x + (x_total - 1) / 3;
  y_end = y + y_total - 1;
  LCD_WR_REG(0x2a);  //x��ʼ
  LCD_WR_DATA(highByte(x));
  LCD_WR_DATA(lowByte(x));
  LCD_WR_DATA(highByte(x_end));
  LCD_WR_DATA(lowByte(x_end));
  LCD_WR_REG(0x2b);  //y��ʼ
  LCD_WR_DATA(highByte(y));
  LCD_WR_DATA(lowByte(y));
  LCD_WR_DATA(highByte(y_end));
  LCD_WR_DATA(lowByte(y_end));
}
//����λ
void LCD_RESET(void) {
  digitalWrite(reset, 0);
  delay(100);
  digitalWrite(reset, 1);
  delay(50);
}
//����ʼ��
void LCD_Init(void) {
  LCD_RESET();  //LCD ��λ
  delay(120);
  //************* ST7586S��ʼ��**********//
  LCD_WR_REG(0x11);   //�˳�˯��ģʽ
  LCD_WR_REG(0xC0);   // ���� VOP
  LCD_WR_DATA(0xf1);  // ���� VOP ��ֵ�ĵ� 8 λ���ܹ� 9 λ��,ÿ��һ���� 0.03667V
  LCD_WR_DATA(0x00);  // ���� VOP ��ֵ�ĵ� 9 λ��Ҳ�����һλ
  LCD_WR_REG(0xC3);   // ���� BIAS
  LCD_WR_DATA(0x02);  // 00��BIAS = 1/14,02 = 1/12,03 = 1/11,04 = 1/10,05 = 1/9
  LCD_WR_REG(0xC4);   // ������ѹ����
  LCD_WR_DATA(0x07);  // 07��8 ��ѹ
  LCD_WR_REG(0xD0);   // ����ģ���·
  LCD_WR_DATA(0x1D);  // ����ģ���·
  LCD_WR_REG(0xB5);   // N-Line = 13
  LCD_WR_DATA(0x00);  // 8d
  LCD_WR_REG(0x39);   // 0x38������Ϊ�Ҷ�ģʽ�� 0x39: ����Ϊ�ڰ�ģʽ��
  LCD_WR_REG(0x3A);   // ���� DDRAM �ӿڣ���ɫģʽ��4 �Ҷȼ���16 �Ҷȼ���
  LCD_WR_DATA(0x02);  // 0x03:16 �Ҷȼ���0x02:4 �Ҷȼ���ɫģʽ��
  LCD_WR_REG(0x36);   // ɨ��˳������
  LCD_WR_DATA(0x00);  // ɨ��˳������:MX=1,MY=1: �����ң����ϵ��µ�ɨ��˳��
  LCD_WR_REG(0xB0);   // Duty ����
  LCD_WR_DATA(0x9f);  // Duty ����:1/160
  LCD_WR_REG(0x20);   // �������ã�OFF
  LCD_WR_REG(0xf1);   //�¶Ȳ������¶ȱ仯�ı�֡Ƶ
  LCD_WR_DATA(0x15);
  LCD_WR_DATA(0x15);
  LCD_WR_DATA(0x15);
  LCD_WR_DATA(0x15);
  LCD_WR_REG(0xb1);      // ɨ����ʼ������
  LCD_WR_DATA(0x00);     // ɨ����ʼ�����ã��� COM0 ��ʼ
  LCD_WR_REG(0x29);      // ����ʾ��DISPLAY ON
  digitalWrite(led, 0);  //��������
  LCD_Clear();           //��ȫ����ɫ
}
