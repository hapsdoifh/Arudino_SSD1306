#include <Wire.h>

#define OLED_ADDRESS 0x3C  // SSD1306 I2C address
void WritePixel(int x, int y);
void DrawRect(int StartX, int StartY, int EndX, int EndY);
void DrawRegMapRect(int StartX, int StartY, int EndX, int EndY);
static char register_map[128*8];

void WriteRegMap();

void setup() {
  // put your setup code here, to run once:
  // Wire.begin();
  // TWBR = 158;

  // int data = 0x01 << (y%4);
  memset(register_map, 0, sizeof(register_map));
  int command_list[] = {
    1,
    0xAE,
    NULL,
    // 2, 0xD5, 0x80, //clock frequency divider
    // 2, 0xA8, 0x3F, //MUX ratio
    // 2, 0xD3, 0x00, //Display offset
    1,
    0x40,
    NULL,  //Set Start line
    2,
    0x8D,
    0x14,
    // 1, 0xA1, NULL,
    // 1, 0xC0, NULL,

    // 2, 0xDA, 0x12,
    // 2, 0x81, 0xCF,
    // 2, 0xD9, 0xF1,
    // 2, 0xDB, 0x40,
    1,
    0xA4,
    NULL,
    // 1, 0xA6, NULL,
    2,
    0x20,
    0x00,
    1,
    0xAF,
    NULL,
  };
    Wire.setClock(100000);
    Wire.begin(9);
    Wire.setClock(100000);
  Wire.beginTransmission(0x3C);
  Wire.write(0x00);
  for (int cmd_cnt{ 0 }; cmd_cnt < sizeof(command_list) / sizeof(int); cmd_cnt += 3) {
    for (int i = cmd_cnt + 1; i <= cmd_cnt + command_list[cmd_cnt]; i++) {
      Wire.write(command_list[i]);
    }
  }
  Wire.endTransmission();
  WritePixel(0, 0);
  for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 128; i++) {
      Wire.beginTransmission(0x3C);
      Wire.write(0x40);  //0b 0100 0000 - data byte
      Wire.write(0x00);
      Wire.endTransmission();
    }
  }
  // delay(1000);
  // Wire.beginTransmission(0x3C);
  // Wire.write(0x00);
  // Wire.write(0x21);
  // Wire.write(0x0f);
  // Wire.write(0x7f);
  // Wire.endTransmission();
  // // TWSR |= bit(TWPS0)

  // Wire.beginTransmission(0x3C);
  // Wire.write(0x00);
  // Wire.write(0x22);
  // Wire.write(0x03);
  // Wire.write(0x07);
  // Wire.endTransmission();


  // Wire.beginTransmission(0x3C);
  // Wire.write(0x40);
  // for(int i = 0; i<32; i++)
  //     Wire.write(0xff);
  // Wire.endTransmission();
      // TWSR |= bit(TWPS0)
    delay(1000);
    for(int i = 0; i<16; i++){
        DrawRegMapRect(i,i,127-i,31-i);
        WriteRegMap();
    }
    // DrawRegMapRect(0,0,127,31);
    // WriteRegMap();
  // DrawRect(50,10,100,30);
}

void SetColumnAddr(int column) {
  Wire.beginTransmission(0x3C);
  Wire.write(0x00);
  Wire.write(0x21);  // set column addr
  Wire.write(column);
  Wire.write(0x7f);
  Wire.endTransmission();
}

void SetPageAddr(int page) {
  Wire.beginTransmission(0x3C);
  Wire.write(0x00);
  Wire.write(0x22);  // set page addr
  Wire.write(page);
  Wire.write(0x07);
  Wire.endTransmission();
}

int BitMapping(int FourBitIn){
    int EightBitOut = 0;
    for (int i = 0; i < 4; i++) {
        EightBitOut += ((FourBitIn & 1) << i * 2);
        FourBitIn >>= 1;
    }  
    return EightBitOut; 
}

void SendData(int data){
    Wire.beginTransmission(0x3C);
    Wire.write(0x40);
    Wire.write(data);
    Wire.endTransmission();
}

void WriteRegMap() {
  for (int row = 0; row < 8; row++) {
        for(int col = 0; col < 128; col++){
            SetColumnAddr(col); 
            SetPageAddr(row);
            SendData(register_map[row*128 + col]);
        }
    }
}

void SetRegMap(int x, int y){
    int row = y / 4;
    int col = x;
    int data = 0x01 << (y % 4);
    data = BitMapping(data);
    register_map[row*128 + col] |= data;
}

void DrawRegMapRect(int StartX, int StartY, int EndX, int EndY) {
    for (int i = StartX; i <= EndX; i++) {
        SetRegMap(i, StartY);
    }
    for (int i = StartX; i <= EndX; i++) {
        SetRegMap(i, EndY);
    }
    for (int i = StartY; i <= EndY; i++) {
        SetRegMap(StartX, i);
    }
    for (int i = StartY; i <= EndY; i++) {
        SetRegMap(EndX, i);
    }
}

void WritePixel(int x, int y) {
  if (x < 0 || y < 0 || x > 127 || y > 31) {
    return;
  }
  Wire.beginTransmission(0x3C);
  Wire.write(0x00);
  Wire.write(0x21);  // set column addr
  Wire.write(x);
  Wire.write(0x7f);
  Wire.endTransmission();
  // TWSR |= bit(TWPS0)

  Wire.beginTransmission(0x3C);
  Wire.write(0x00);
  Wire.write(0x22);  // set page addr
  Wire.write(y / 4);
  Wire.write(0x07);
  Wire.endTransmission();

  int data = 0x01 << (y % 4);
  int send_data = 0;
  for (int i = 0; i < 4; i++) {
    send_data += ((data & 1) << i * 2);
    data >>= 1;
  }  //this doesn't work because when you write to a new page it resets the previous pixel there

  Wire.beginTransmission(0x3C);
  Wire.write(0x40);
  // Wire.write(send_data);
  Wire.write(0b01000000);
  Wire.write(0b00010000);
  Wire.write(0b00000100);
  Wire.write(0b00000001);
  Wire.endTransmission();
}

void DrawRect(int StartX, int StartY, int EndX, int EndY) {
  for (int i = StartX; i < EndX; i++) {
    WritePixel(i, StartY);
  }
  for (int i = StartX; i < EndX; i++) {
    WritePixel(i, EndY);
  }
  for (int i = StartY; i < EndY; i++) {
    WritePixel(StartX, i);
  }
  for (int i = StartY; i < EndY; i++) {
    3WritePixel(EndX, i);
  }
}

void loop() {
}
