#include <Wire.h>
#include <SSD1306_Driver.h>

#define OLED_ADDRESS 0x3C  // SSD1306 I2C address

void setup() {
    // put your setup code here, to run once:
    // TWBR = 158;
    // TWSR |= bit(TWPS0)

    DisplayStart(0x3C);
    
    ClearScreen();
    delay(1000);
    // for(int i = 0; i<16; i++){
    //     DrawRegMapRect(i, i,127-i,31-i);
    //     WriteRegMap();
    // }

    // for(int i = 0; i < 128; i+=10){
    //     WriteRegLine(0,0,i,31);
    //     if(i%20 == 0)
    //     WriteRegMap();
    // }
    // for(int i = 0; i < 128; i+=10){
    //     WriteRegLine(127-i,31,127,0);
    //     if(i%20 == 0)
    //     WriteRegMap();
    // }
    // WriteRegMap();
    // // ClearScreen();
    // WriteRegMap();
    // for(int i = 0; i < 128; i+=10){
    //     WriteRegLine(0,31,i,0);
    //     if(i%20 == 0)
    //     WriteRegMap();
    // }
    // WriteRegMap();
    // for(int i = 0; i < 128; i+=10){
    //     WriteRegLine(127,31,127-i,0);
    //     if(i%20 == 0)
    //     WriteRegMap();
    // }
    // WriteRegMap();

    ClearScreen();
    DrawRegEllipse(10,0,100,30,1);
    WriteRegMap();
}

void loop() {
}
