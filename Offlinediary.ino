#include <Wire.h>
#include <SH1106Wire.h>  // ThingPulse的SH1106库
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Adafruit_NeoPixel.h>

// OLED屏幕定义
#define OLED_SDA 8
#define OLED_SCL 9
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LED_PIN   14  
#define LED_COUNT  1
  
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

SH1106Wire display(0x3c, OLED_SDA, OLED_SCL);  // I2C地址通常为0x3C


#define FILE_COUNT 20  // 谚语文件总数
unsigned long lastChangeTime = 0;
const long displayInterval = 5000;  // 5秒切换一次
int currentFileIndex = 1;  // 当前显示的文件索引(1-20)

void setup() {
  
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  
  // 初始化OLED
  display.init();
  display.flipScreenVertically();  // 根据需要调整屏幕方向
  display.setFont(ArialMT_Plain_10);  // 使用内置字体
  display.clear();
  display.drawString(0, 0, "Initializing...");
  display.display();
  
  // 初始化SD卡
  if(!SD.begin()){
    display.clear();
    display.drawString(0, 0, "SD Card Fail!");
    display.display();
    return;
  }
  
  // 检查文件数量
  int count = countFiles();
  if(count < FILE_COUNT){
    display.drawString(0, 0, "Files not enough!");
  }
  
  lastChangeTime = millis();
}


void loop() {
  unsigned long currentTime = millis();
  
  if(currentTime - lastChangeTime >= displayInterval){
    lastChangeTime = currentTime;
    
    // 构建文件名 (proverb1.txt ~ proverb20.txt)
    String filename = "/proverb" + String(currentFileIndex) + ".txt";
    
    // 读取并显示文件内容
    displayProverb(filename);
    
    // 更新文件索引
    currentFileIndex++;
    if(currentFileIndex > FILE_COUNT){
      currentFileIndex = 1;
    }
  }
}

// 计算SD卡根目录下的文件数量
int countFiles(){
  int count = 0;
  File root = SD.open("/");
  while(File entry = root.openNextFile()){
    if(!entry.isDirectory()){
      count++;
    }
    entry.close();
  }
  root.close();
  return count;
}

// 显示谚语文件内容
void displayProverb(String filename){
  File file = SD.open(filename);
  if(!file){
    display.drawString(0, 0, "Can not open file");
    return;
  }
  
  // 清空屏幕
  display.clear();
  display.setFont(ArialMT_Plain_10);  // 使用10像素高的字体
  
  // 逐行读取并显示
  int line = 0;
  while(file.available()){
    String text = file.readStringUntil('\n');
    text.trim();
    
    // 计算显示位置
    int yPos = line * 12;  // 每行12像素高度
    
    // 分割过长的行
    if(text.length() > 25){ // 大约25个字符适合128像素宽度
      String firstPart = text.substring(0, 25);
      String secondPart = text.substring(25,50);
      String thirdPart = text.substring(50,75);
      String fourthPart = text.substring(75,100);
      String fifthPart = text.substring(100);
      display.drawString(0, yPos, firstPart);
      line++;
      yPos = line * 12;
      display.drawString(0, yPos, secondPart);
      line++;
      yPos = line * 12;
      display.drawString(0, yPos, thirdPart);
      line++;
      yPos = line * 12;
      display.drawString(0, yPos, fourthPart);
      line++;
      yPos = line * 12;
      display.drawString(0, yPos, fifthPart);

    } else {
      display.drawString(0, yPos, text);
    }
    
    line++;
    if(yPos > SCREEN_HEIGHT - 12) break;  // 超出屏幕高度停止显示
  }
  
  file.close();
  display.display();
  rainbow(5); 
}
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
