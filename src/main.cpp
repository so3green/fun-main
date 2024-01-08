#include <Arduino.h>
#include <FastLED.h>
void setup() {
  pinMode(2,INPUT);
  Serial.begin(115200);
  pinMode(25,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(33,OUTPUT);
  ledcAttachPin(25,0);
  ledcAttachPin(26,1);
  ledcAttachPin(33,2);
  ledcSetup(0,1000,10);
  ledcSetup(1,1000,10);
  ledcSetup(2,1000,10);
}

void loop() {
  Serial.println(analogRead(2));
  ledcWrite(0,0);
  ledcWrite(1,0);
  ledcWrite(2,0);
  delay(1000);
  ledcWrite(0,100);
  ledcWrite(1,100);
  ledcWrite(2,100);
  delay(1000);
}