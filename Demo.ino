#include "SH1106.h" 
#include <Adafruit_NeoPixel.h>
#include "Wire.h"
#include <SD.h>
#include <SPI.h>
#define LED_PIN    14
#define LED_COUNT 1
#define BUTTON_PRESS   47
#define BUTTON_LEFT    48                    
#define BUTTON_RIGHT   21

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
SH1106  display1(0x3c,8,9);

void setup() {

  pinMode(BUTTON_LEFT,INPUT_PULLUP);
  pinMode(BUTTON_PRESS,INPUT_PULLUP);
  pinMode(BUTTON_RIGHT,INPUT_PULLUP);

  strip.begin(); 
  strip.clear();          
  strip.show();     
 
  display1.init();
  display1.flipScreenVertically();
  display1.setFont(ArialMT_Plain_10);
  delay(10);
  display1.clear();
  display1.drawString( 0, 0, "TEST START");
  display1.drawString( 0, 10, "LEFT 48");
  display1.drawString( 0, 20, "PRESS 47");
  display1.drawString( 0, 30, "RIGHT 21");
  if (!SD.begin()){
  display1.drawString( 0, 40, "SD card is not detected");
  }
  else{
    clearLine(4);
    display1.drawString( 0, 50, "SD card is detected");
  }

  display1.display();
  delay(2000);
  displayFileContent("/dstike.txt");
}

void loop() {
    if(digitalRead(BUTTON_LEFT)== LOW)
  {
   display1.drawString( 0, 20, "LEFT--OK");
   display1.display();
   setcolorAll(255,0,0);
  }
    if(digitalRead(BUTTON_PRESS)== LOW)
  {
   display1.drawString( 0, 30, "PRESS--OK");
   display1.display();
   setcolorAll(0,255,0);
  }
    if(digitalRead(BUTTON_RIGHT)== LOW)
  {
   display1.drawString( 0, 40, "RIGHT--OK");
   display1.display();
   setcolorAll(0,0,255);
  }
  
}

void setcolorAll(uint8_t r, uint8_t g, uint8_t b) {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, strip.Color(r, g, b));                                                           
  }
  strip.show();  
}

void clearLine(uint8_t lineNumber) {
  int lineHeight = 10; // 每行高度（根据字体调整）
  int y = lineNumber * lineHeight; // 计算y坐标
  display1.setColor(BLACK); // 设置画笔颜色为黑色
  display1.fillRect(0, y, display1.getWidth(), lineHeight); // 用黑色填充行区域
  display1.setColor(WHITE); // 恢复画笔颜色为白色
  display1.display(); // 更新屏幕
}

void displayFileContent(const char* filename) {
  File file = SD.open(filename, FILE_READ);

  if (!file) {
    
    display1.clear();
    display1.drawString(0, 0, "flie can not be opened");
    display1.display();
    return;
  }

  
  display1.clear();

  int y = 0; // 起始y坐标
  const int lineHeight = 10; // 行高（根据字体调整）

  while (file.available()) {
    String line = file.readStringUntil('\n'); // 按行读取文件
    display1.drawString(0, y, line);
     y += lineHeight;

 if (y >= display1.getHeight()) {
      display1.display();
      delay(2000); // 显示2秒
      display1.clear();
      y = 0;
    }
  
  }
  display1.display();
  file.close();
}


