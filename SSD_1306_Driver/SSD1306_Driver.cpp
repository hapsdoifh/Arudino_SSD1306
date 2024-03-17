
#include <SSD1306_Driver.h>
#include <Wire.h>
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))
#define signed_max(a,b) (abs(a) > abs(b) ? a : b)

void DisplayStart(int DeviceAddr, int SerialSpeed = 200000){

    int command_list[] = {
        1, 0xAE, NULL,
        // 2, 0xD5, 0x80, //clock frequency divider
        // 2, 0xA8, 0x3F, //MUX ratio
        // 2, 0xD3, 0x00, //Display offset
        1, 0x40, NULL,  //Set Start line
        2, 0x8D, 0x14, //set voltage pump regualtor
        // 1, 0xA1, NULL,
        // 1, 0xC0, NULL,

        // 2, 0xDA, 0x12,
        // 2, 0x81, 0xCF,
        // 2, 0xD9, 0xF1,
        // 2, 0xDB, 0x40,
        1, 0xA4,NULL,
        // 1, 0xA6, NULL, //Set normal display mode
        2, 0x20, 0x00, //Set addressing mode
        1, 0xAF, NULL, //Set display on
    };
    Wire.begin();
    Wire.setClock(200000);
    Wire.beginTransmission(0x3C);
    Wire.write(0x00);
    for (int cmd_cnt{ 0 }; cmd_cnt < sizeof(command_list) / sizeof(int); cmd_cnt += 3) {
        for (int i = cmd_cnt + 1; i <= cmd_cnt + command_list[cmd_cnt]; i++) {
        Wire.write(command_list[i]);
        }
    }
    Wire.endTransmission();
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

void SendData(int data){
    Wire.beginTransmission(0x3C);
    Wire.write(0x40);
    Wire.write(data);
    Wire.endTransmission();
}

int BitMapping(int FourBitIn){
    int EightBitOut = 0;
    for (int i = 0; i < 4; i++) {
        EightBitOut += ((FourBitIn & 1) << i * 2); //LSB on top -> 0x01 on page 0 would correspsond to the top most row
        FourBitIn >>= 1;
    }  
    return EightBitOut; 
}

void WriteRegLine(int StartX, int StartY, int EndX, int EndY, int thickness = 1){
    int Cycles = signed_max((EndX - StartX),(EndY - StartY)), CycleSign = 1;
    float Slope = float((EndY-StartY))/(EndX - StartX);
    int StartMapping[] = {StartX, StartY};
    int CoordMapping[] = {0,0}, default_mapping = 0;
    if(Cycles < 0){
        CycleSign = -1;
        Cycles *= CycleSign;
    }
    if (abs(EndY - StartY) >= abs(EndX - StartX)){ //if line is longer vertically rise > run
        Slope = 1/Slope;
        default_mapping = 1;
    }
    for(int i = 0; i <= Cycles; i++){
        CoordMapping[default_mapping] = int(StartMapping[default_mapping] + i*CycleSign); //default: CoordMapping[x,y] modified:CoordMapping[y,x]
        CoordMapping[1 - default_mapping] = int(StartMapping[1-default_mapping] + i*CycleSign*Slope);

        if(CoordMapping[0] >= 0 && CoordMapping[0] < 128 && CoordMapping[1] >= 0 && CoordMapping[1] < 32)
        SetRegMap(CoordMapping[0], CoordMapping[1], thickness);
    }
}

void WriteRegMapDepricated() {
    //proof of concept, slow as it sets the memory address every time
    for (int row = 0; row < 8; row++) {
            for(int col = 0; col < 128; col++){
                SetColumnAddr(col); 
                SetPageAddr(row);
                SendData(register_map[row*128 + col]);
            }
        }
}

void WriteRegMap() {
    SetColumnAddr(0); 
    SetPageAddr(0);
    for (int row = 0; row < 8; row++) {
        for(int col = 0; col < 128; col++){
            SendData(register_map[row*128 + col]);
        }
    }
}

void SetRegMap(int x, int y, int thickness = 1){
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

void ClearScreen(){
    memset(register_map, 0, sizeof(register_map)/sizeof(char));
    WriteRegMap();
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
    Wire.write(send_data);
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
        WritePixel(EndX, i);
    }
}
