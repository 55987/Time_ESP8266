#include <ESP8266WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// 创建U8G2对象
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0,  U8X8_PIN_NONE, SCL, SDA);

const char* ssid = "";  //改成自己的WiFi名称
const char* password = "";//改成自己的WiFi密码

const char week[7][10] = {"日", "一", "二", "三", "四", "五", "六"};

const char* ntpServer = "pool.ntp.org";
const long  gmtOffsetSec = 3600*8;    // 时区偏移，单位为秒（例如，东八区为28800）
const int   daylightOffsetSec = 0; // 夏令时偏移，单位为秒
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffsetSec, 60000*60*12); // 更新间隔为60000毫秒（1分钟）

String unixTimestampToDate(time_t timestamp) {
    struct tm *timeinfo;
    timeinfo = localtime(&timestamp);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y年%m月%d日", timeinfo);
    return String(buffer);
}

// 将Unix时间戳转换为日期时间格式
String unixTimestampToTime(time_t timestamp) {
    struct tm *timeinfo;
    timeinfo = localtime(&timestamp);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    return String(buffer);
}

void setup() {
  pinMode(D0,OUTPUT);
  display.begin();
  display.setBusClock(800000);
  display.enableUTF8Print();
  display.clearBuffer();
  
  // 连接Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(D0,LOW);
    delay(100);
    digitalWrite(D0,HIGH);
    delay(900);
  }
    digitalWrite(D0,HIGH);

  // 初始化NTP客户端
    timeClient.begin();
    
    digitalWrite(D0,LOW);
    for(int i=0;i<10;i++){
      timeClient.update();
    }
    delay(500);
    digitalWrite(D0,HIGH);
}

void loop() {
  // 更新时间
  timeClient.update();

    time_t timestamp = timeClient.getEpochTime();
    String formattedDate = unixTimestampToDate(timestamp);
    String formattedTime = unixTimestampToTime(timestamp);

  // 清屏并设置字体
  display.clearBuffer();
    
  // 显示UI
  display.drawBox(0,24,128,2);


  // 显示时间
  display.setFont(u8g2_font_logisoso28_tf);
  display.setCursor(-1, 62);
  display.print(unixTimestampToTime(timestamp));
  
  // 显示日期
  display.setFont(u8g2_font_wqy14_t_gb2312a);
  display.setCursor(5, 18);
  display.print(unixTimestampToDate(timestamp));
    
  // 显示星期
  display.setFont(u8g2_font_wqy14_t_gb2312a);
  display.setCursor(107, 18);
  display.print(week[timeClient.getDay()]);

  // 发送缓冲区到显示
  display.sendBuffer();
}
