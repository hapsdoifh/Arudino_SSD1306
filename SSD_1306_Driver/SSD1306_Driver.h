#ifndef SSD1306_Driver
#define SSD1306_Driver
static char register_map[128*8];
void WriteRegMap();
void WriteRegMapDepricated();
void SetRegMap(int x, int y, int thickness = 1);
void DrawRegMapRect(int StartX, int StartY, int EndX, int EndY);
void DisplayStart(int DeviceAddr, int SerialSpeed = 200000);
void ClearScreen();
void WriteRegLine(int StartX, int StartY, int EndX, int EndY, int thickness = 1);
void SetRegMap(int x, int y, int thickness = 1);
#endif