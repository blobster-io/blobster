// ****************************************************************
// * BLOBSTER Project - Arduino driven smart side purge mechanism *
// * Published under GNU GPL v3 license                           *
// ****************************************************************

#include <Arduino.h>
#include <U8g2lib.h>
#include <Servo.h>

/* Display constructor */
U8G2_SSD1305_128X64_ADAFRUIT_F_HW_I2C u8g2(U8G2_R0);

#define inductionTrigger 4

Servo servoK;
Servo servoX;
Servo servoY;

int homeX = 35;
int homeY = 42;
int initV = 54;
int initX = 35;
int initY = 42;
int activeX = 102;
int activeY = 68;
int activeV = 100;
int closerY = 48;

long int megaBlobTimestamp = 0;
long int entryTimestamp = 0;

int blobsTotal = 0;
int blobsTotalTime = 0;
int h = 0;
int m = 0;
int s = 0;

bool activeXflag = false;
bool active = false;

long int startTimestamp = 0;
long int endTimestamp = 0;
long int lastblobTime = 0;

void setup(void) {
  pinMode(inductionTrigger, INPUT);
 
  servoK.attach(3);
  servoX.attach(5);
  servoY.attach(6);

  servoK.write(initV);
  servoX.write(initX);
  servoY.write(activeY);

  u8g2.begin();
}

void loop(void) {
  activeXflag = !digitalRead(inductionTrigger);
  String messageText = "";
  entryTimestamp = millis();
  int megaBlobPosition = 0;

  u8g2.firstPage();
  do { 
    u8g2.setFont(u8g2_font_helvB14_tf);
    if (activeXflag) { 
      messageText = "Purging: ";
      if (active) {
        messageText += (entryTimestamp - startTimestamp) / 1000;
        messageText += "s";
      }
      u8g2.setCursor(0,14);
      u8g2.print(messageText);
      megaBlobPosition = closerY + (entryTimestamp - megaBlobTimestamp) / 1000;
      if (megaBlobPosition <= activeY) { 
        // Moving slighly down during purge of megablob
        servoY.write(megaBlobPosition);
      }
    } else {
      u8g2.setCursor(0,14);
      u8g2.print("Idle...");
    };
    u8g2.drawHLine(0,35,128);
    u8g2.drawHLine(0,36,128);
    u8g2.drawHLine(0,37,128);
        u8g2.setFont(u8g2_font_helvR08_tf);
        h = blobsTotalTime / 3600;
        m = (blobsTotalTime - h * 3600) / 60;
        s = (blobsTotalTime - h * 3600 - m * 60);
        messageText = "Total purge time: ";
        if (h < 10) messageText += "0"; 
        messageText += h;
        messageText += ":";
        if (m < 10) messageText += "0"; 
        messageText += m;
        messageText += ":";
        if (s < 10) messageText += "0"; 
        messageText += s;
        u8g2.setCursor(0,62);
        u8g2.print(messageText);
        messageText = "Total purge count: ";
        messageText += blobsTotal;
        u8g2.setCursor(0,50);
        u8g2.print(messageText);
        u8g2.setFont(u8g2_font_helvB08_tf);
        if (blobsTotal != 0) {
          messageText = "Last purge time: ";
          messageText += lastblobTime;
          messageText += "s";
          u8g2.setCursor(0,30);
          u8g2.print(messageText);
        } else {
          if (activeXflag) {
            messageText = "Processing first purge...";
          } else {
            messageText = "Waiting for first purge...";    
          }
          u8g2.setCursor(0,30);
          u8g2.print(messageText);
        }
  } while ( u8g2.nextPage() );  

  // Printhead raised induction trigger, purge starts
  if (activeXflag && !active) {
    startTimestamp = millis();
    servoX.write(activeX);
    delay(500);
    servoY.write(closerY);
    megaBlobTimestamp = millis();
    active = true;
  }; 

  // Printhead left position, time to kick of and park
  if (!activeXflag && active)
  {
    endTimestamp = millis();
    lastblobTime = (endTimestamp - startTimestamp) / 1000;
    blobsTotal++;
    blobsTotalTime += lastblobTime;
    servoX.write(initX);
    delay(500);
    servoY.write(initY);
    delay(500);
    servoK.write(activeV);
    delay(200);
    servoK.write(initV);
    delay(200);
    servoY.write(activeY);
    active = false;
  }
}


