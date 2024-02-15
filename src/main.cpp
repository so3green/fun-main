#include <Arduino.h>

//メインの明るさ設定用変数
unsigned int bright = 127;
//暗い時用の明るさ設定変数
unsigned int bright_small = 60;
//明るさ０の時用の明るさ設定変数
unsigned int bright_none = 0;
unsigned int bright_red = 0;
unsigned int bright_blue = 0;
unsigned int bright_green = 0;
//JSN-SR-04のechoPinの設定
#define readpin 2
int distance;

// FastLED simple sketch 

#include <FastLED.h>
#define LED_PIN     5
#define NUM_LEDS    150
#define BRIGHTNESS  20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define DELAYVAL 220 // Time (in milliseconds) to pause between pixels

#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include <iostream>



const char ssid[] = "ESP32";
const char pass[] = "12345678";       // パスワードは8文字以上
const IPAddress ip(192,168,123,45);
const IPAddress subnet(255,255,255,0);
const char* PARAM_MESSAGE = "message";

AsyncWebServer server(80);  
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}
void appendFile(fs::FS &fs, const char * path,const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);
    File root = fs.open(dirname);
    if(!root){
      writeFile(SPIFFS, "/test.csv", "Hello ");
      return;
    }
}
unsigned long prev, next, interval;
void setup() {
  FastLED.clear();
  FastLED.show();
  //各種ピンモードの設定
  pinMode(25,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(33,OUTPUT);
  pinMode(25,OUTPUT);
  pinMode(18,OUTPUT);
  pinMode(19,OUTPUT);
  pinMode(21,OUTPUT);
  pinMode(2, INPUT);
  //LED用のピンのセットアップ
  ledcAttachPin(25,0);
  ledcAttachPin(26,1);
  ledcAttachPin(33,2);
  ledcAttachPin(18,3);
  ledcAttachPin(19,4);
  ledcAttachPin(21,5);
  ledcSetup(0,20000,10);
  ledcSetup(1,20000,10);
  ledcSetup(2,20000,10);
  ledcSetup(3,20000,10);
  ledcSetup(4,20000,10);
  ledcSetup(5,20000,10);
  // シリアル通信の速度設定（115200を使用)
  Serial.begin(115200);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  delay(50);
  interval = 960;
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  WiFi.softAP(ssid, pass);           // SSIDとパスの設定
  delay(100);                        // このdelayを入れないと失敗する場合がある
  WiFi.softAPConfig(ip, ip, subnet); // IPアドレス、ゲートウェイ、サブネットマスクの設定
  
  IPAddress myIP = WiFi.softAPIP();   // WiFi.softAPIP()でWiFi起動

  // 各種情報を表示
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(myIP);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<a href=\"/test.csv\">dammy.csv</a>");
    });
    server.on("/test.csv", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test.csv", "text/csv");
        delay(1) ;
    });
    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
    listDir(SPIFFS, "/test.csv", 0);
}


void loop() {
  distance = digitalRead(2);
  // シリアルモニターに対して情報を流す
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(50);
  //距離が20cm～40cmであったとき
  if(distance == 1){
    appendFile(SPIFFS,"/test.csv",",");
    FastLED.clear();
    FastLED.show();
    ledcWrite(0,0);
    ledcWrite(1,0);
    ledcWrite(2,0);
    //LEDの設定の初期化
    bright_red = 255;
    bright_blue = 10;
    bright_green = 85;
    //開始時刻の保存
    unsigned long times = millis();
    //音楽の再生開始
    digitalWrite(27,HIGH);
    delay(50);
    digitalWrite(27,LOW); 
    //LEDの座標の初期化
    int led_num = 0;
    int when = 0;
    
    delay(1200);
    for(int i = 1; i < 143; i++){
      bright_red = 255;
      bright_blue = 255;
      bright_green = 255;
      FastLED.clear();
      leds[i-1] = CRGB(bright_red, bright_green, bright_blue);
      leds[i] = CRGB(bright_red, bright_green, bright_blue);
      leds[i+1] = CRGB(bright_red, bright_green, bright_blue);
      FastLED.show();
      delay(2);
    }
    bright_red = 255;
          bright_blue = 0;
          bright_green = 0;
          for(int i = 0; i < 144; i++){
            leds[i]=CRGB(bright_red,bright_green,bright_blue);
          }
          FastLED.show();
    unsigned long start_time = millis();
    while(1){
      //時間の更新
      unsigned long curr = millis();
      //interval分だけ時間がすぎると反応する
      if((curr-prev) >= interval){
        //最終動作時刻の更新
        prev = curr;
        distance = digitalRead(2);
        if(distance == 0){
          appendFile(SPIFFS,"/test.csv","0");
        }
        else if(distance == 1){
          appendFile(SPIFFS,"/test.csv","1");
        }
        //10個同時にLEDを作動させる
        if(when <= 10 && when >= 7){
          for(int l = 0; l < 6; l++){
          leds[led_num] = CRGB( bright_red, bright_green, bright_blue);
          led_num++;
          }
        }
        else if(when == 0){
          bright_red = 100;
          bright_blue = 0;
          bright_green = 100;
          for(int l = 0; l < 6; l++){
          leds[led_num] = CRGB( bright_red, bright_green, bright_blue);
          led_num++;
          }
        }
        else if(when == 11){
          bright_red = 100;
          bright_blue = 0;
          bright_green = 100;
          for(int l = 0; l < 6; l++){
          leds[led_num] = CRGB( bright_red, bright_green, bright_blue);
          led_num++;
          }
          for(int j=0; j <= led_num-1; j++){
            bright_red = 0;
            bright_blue = 255;
            bright_green = 0;
            leds[j] = CRGB( bright_red, bright_green, bright_blue);
          }
        }
        else{
          for(int l = 0; l < 6; l++){
          leds[led_num] = CRGB( bright_red, bright_green, bright_blue);
          led_num++;
          }
        }
        when++;
        //LEDの点灯
        FastLED.show();
        //60個LEDがついた時点で色を更新する
      }
        if(when == 1){
          bright_red = 100;
          bright_blue = 0;
          bright_green = 100;
        }
        if(when == 11){
          bright_red = 0;
          bright_blue = 255;
          bright_green = 0;
        }
        if(when == 17){
          bright_red = 0;
          bright_blue = 255;
          bright_green = 255;
        }
        if(when == 21){
          bright_red = 150;
          bright_blue = 255;
          bright_green = 255;
        }
        //120個LEDがついた時点で色を更新する
        if(led_num == 78){
          for(int j=0; j <= led_num-1; j++){
            bright_red = 0;
          bright_blue = 255;
          bright_green = 255;
            leds[j] = CRGB( bright_red, bright_green, bright_blue);
          }
        }
        if(led_num == 102){
          for(int j=0; j <= led_num-1; j++){
            bright_red = 150;
          bright_blue = 255;
          bright_green = 250;
            leds[j] = CRGB( bright_red, bright_green, bright_blue);
          }
        }
        if(when == 8){
          int bright = 40;
          int bright_blue_1 = 12;
          int bright_blue_led = 0;
          for(int j=0; j <= 40; j++){
            bright = j;
            bright_blue_led = bright_blue_1 * j;
            ledcWrite(0,0);
            ledcWrite(1,0);
            ledcWrite(2,bright_blue_led);
            delay(10);
          }
          for(int j=40; j >= 0; j--){
            bright = j;
            bright_blue_led = bright_blue_1 * j;
            ledcWrite(0,0);
            ledcWrite(1,0);
            ledcWrite(2,bright_blue_led);
            delay(10);
          }
          for(int j=0; j <= 40; j++){
            bright = j;
            bright_blue_led = bright_blue_1 * j;
            ledcWrite(0,0);
            ledcWrite(1,0);
            ledcWrite(2,bright_blue_led);
            delay(10);
          }
          for(int j=40; j >= 0; j--){
            bright = j;
            bright_blue_led = bright_blue_1 * j;
            ledcWrite(0,0);
            ledcWrite(1,0);
            ledcWrite(2,bright_blue_led);
            delay(10);
          }
        }
        if(curr - times >=30000){
        int bright = 40;
        bright_blue = 2;
        int bright_blue_led = 0;
        for(int j=40; j >= 0; j--){
          bright = j;
          bright_blue_led = bright_blue * j;
          FastLED.setBrightness(bright);
          FastLED.show();
          ledcWrite(3,0);
          ledcWrite(4,0);
          ledcWrite(5,bright_blue_led);
          delay(10);
        }
        for(int j=0; j <= 40; j++){
          bright = j;
          bright_blue_led = bright_blue * j;
          FastLED.setBrightness(bright);
          FastLED.show();
          ledcWrite(3,0);
          ledcWrite(4,0);
          ledcWrite(5,bright_blue_led);
          delay(10);
        }
        for(int j=40; j >= 0; j--){
          bright = j;
          bright_blue_led = bright_blue * j;
          FastLED.setBrightness(bright);
          FastLED.show();
          ledcWrite(3,0);
          ledcWrite(4,0);
          ledcWrite(5,bright_blue_led);
          delay(10);
        }
        for(int j=0; j <= 40; j++){
          bright = j;
          bright_blue_led = bright_blue * j;
          FastLED.setBrightness(bright);
          FastLED.show();
          ledcWrite(3,0);
          ledcWrite(4,0);
          ledcWrite(5,bright_blue_led);
          delay(10);
        }
        for(int j=40; j >= 0; j--){
          bright = j;
          bright_blue_led = bright_blue * j;
          FastLED.setBrightness(bright);
          FastLED.show();
          ledcWrite(3,0);
          ledcWrite(4,0);
          ledcWrite(5,bright_blue_led);
          delay(10);
        }
        FastLED.clear();
        FastLED.show();
        delay(1000);
        break;
      }
    }
  }
  else if(distance == 0){
    ledcWrite(0,bright_small);
    ledcWrite(1,bright_none);
    ledcWrite(2,bright_none);
    ledcWrite(3,bright_none);
    ledcWrite(4,bright_none);
    ledcWrite(5,bright_none);
  }
}