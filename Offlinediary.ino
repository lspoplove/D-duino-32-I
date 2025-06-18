#include <Wire.h>
#include <SH1106Wire.h>  // ThingPulse的SH1106库
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// OLED屏幕定义
#define OLED_SDA 8
#define OLED_SCL 9
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
SH1106Wire display(0x3c, OLED_SDA, OLED_SCL);  // I2C地址通常为0x3C


#define FILE_COUNT 20  // 谚语文件总数
unsigned long lastChangeTime = 0;
const long displayInterval = 5000;  // 5秒切换一次
int currentFileIndex = 1;  // 当前显示的文件索引(1-20)

void setup() {
  Serial.begin(115200);
  
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
    Serial.print("找到的文件数量不足，期望20个，实际找到");
    Serial.println(count);
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
    Serial.print("无法打开文件: ");
    Serial.println(filename);
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
}
