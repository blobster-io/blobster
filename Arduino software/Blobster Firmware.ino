// ****************************************************************
// * BLOBSTER Project - Arduino driven smart side purge mechanism *
// * Published under GNU GPL v3 license                           *
// ****************************************************************

#include <Arduino.h>
#include <U8g2lib.h>
#include <Servo.h>
#include <SPI.h>
#include <SD.h>

// Version info
#define firmwareVersion "Version 0.1 ALPHA"

// Standard Blobster wiring Arduino pins assigment
#define inductionTriggerArduinoPin 4
#define servoKArduinoPin 3
#define servoXArduinoPin 5
#define servoYArduinoPin 6

// Default values when config on microSD card is not present or there's no microSD card attached at all
int homeX = 35;
int homeY = 42;
int initV = 54;
int initX = 35;
int initY = 42;
int activeX = 102;
int activeY = 68;
int activeV = 100;
int closerY = 48;

// Display constructor
U8G2_SSD1305_128X64_ADAFRUIT_F_HW_I2C u8g2(U8G2_R0);

// Global variables
File configFile;

Servo servoK;
Servo servoX;
Servo servoY;

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

// Blobster splash screen logo
#define blobsterSplashscreenLogo_width 60
#define blobsterSplashscreenLogo_height 32
static unsigned char blobsterSplashscreenLogo_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0xFC, 
  0x03, 0x00, 0x80, 0x00, 0x30, 0x00, 0x00, 0xFE, 0x07, 0x00, 0x80, 0x00, 
  0x30, 0x00, 0x00, 0xFE, 0x07, 0x00, 0x80, 0x01, 0x30, 0x00, 0x00, 0xFE, 
  0x0F, 0x00, 0x80, 0x01, 0x38, 0x04, 0x00, 0xFF, 0x0F, 0x00, 0x84, 0x03, 
  0x38, 0x0C, 0x80, 0xFF, 0x0F, 0x00, 0x86, 0x03, 0x3C, 0x0C, 0xC0, 0xFF, 
  0x3F, 0x00, 0x86, 0x03, 0x3C, 0x0C, 0xE0, 0xFF, 0x7F, 0x00, 0x87, 0x07, 
  0x3C, 0x1C, 0xE0, 0xFF, 0x7F, 0x00, 0x87, 0x07, 0x3C, 0x1C, 0xE0, 0xFC, 
  0xF3, 0x00, 0xC7, 0x07, 0x7E, 0x1C, 0x60, 0xF8, 0xE1, 0x80, 0xC7, 0x07, 
  0x7E, 0x3E, 0x30, 0xF3, 0xCC, 0x81, 0xC7, 0x07, 0xFE, 0x3E, 0xB8, 0xF7, 
  0xDE, 0x83, 0xEF, 0x07, 0xFC, 0x3D, 0xBC, 0xF7, 0xDE, 0x87, 0xF7, 0x07, 
  0xFC, 0x3B, 0xBC, 0xF7, 0xDE, 0x87, 0xFB, 0x07, 0xFC, 0x17, 0x3C, 0xF3, 
  0xCC, 0x87, 0xFD, 0x07, 0xFC, 0x0F, 0x7C, 0xF8, 0xE1, 0x07, 0xFE, 0x03, 
  0xF8, 0x0F, 0xF8, 0xFC, 0xF3, 0x03, 0xFE, 0x03, 0xF8, 0x0F, 0xFF, 0xFF, 
  0xFF, 0x07, 0xFF, 0x01, 0xF0, 0x8F, 0xFF, 0xFF, 0xFF, 0x1F, 0xFE, 0x00, 
  0xE0, 0x9F, 0xFF, 0x01, 0xF8, 0xBF, 0x7F, 0x00, 0x80, 0xFF, 0xFF, 0x03, 
  0xFC, 0xFF, 0x3F, 0x00, 0x00, 0xF8, 0xFF, 0x0F, 0xFF, 0xFF, 0x01, 0x00, 
  0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 
  0xFF, 0x3F, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 
  0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, };

void updateSplashDisplayStatus(const char *messageText)
{
  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 51, 128, 13);
  u8g2.setDrawColor(1);  
  u8g2.drawStr((128 - u8g2.getStrWidth(messageText)) / 2, 62, messageText);
  u8g2.sendBuffer();
  // Wait for display
  delay(1000);
}

void setup(void) {
  // Initialization of peripherals
  
  // Display
  u8g2.begin();
  u8g2.clearDisplay();
  u8g2.drawXBM(34, 0, blobsterSplashscreenLogo_width, blobsterSplashscreenLogo_height, blobsterSplashscreenLogo_bits);
  u8g2.setFont(u8g2_font_helvB14_tf);
  u8g2.drawStr((128 - u8g2.getStrWidth("BLOBSTER")) / 2, 50,"BLOBSTER");
  u8g2.setFont(u8g2_font_helvR08_tf);
  updateSplashDisplayStatus(firmwareVersion);
  // Wait for user to be able to read version    
  delay(4000); 

  // SD card configuration
  updateSplashDisplayStatus("Initializing SD card...");
  if (SD.begin(10)) {
    updateSplashDisplayStatus("SD card initialized.");
    configFile = SD.open("config");
    if (configFile) {
      // Read from the file until there's nothing else in it:
      updateSplashDisplayStatus("Reading config...");
      while (configFile.available()) {
        // TBD: Config file processing
        configFile.read();
      }
      updateSplashDisplayStatus("Config read succesfully.");
    }
    else
    {
      updateSplashDisplayStatus("No config file, defaults used.");
    }
    // Close the file:
    configFile.close();
  } else {
    updateSplashDisplayStatus("No SD card, defaults used.");  
  }
  // Wait for user to be able to read message regarding config used
  delay(4000);

  // Induction trigger
  pinMode(inductionTriggerArduinoPin, INPUT);
  updateSplashDisplayStatus("Mapping induction trigger...");

  // Servos
  updateSplashDisplayStatus("Initializating servos...");
  servoK.attach(servoKArduinoPin);
  servoX.attach(servoXArduinoPin);
  servoY.attach(servoYArduinoPin);

  servoK.write(initV);
  servoX.write(initX);
  servoY.write(activeY);
  u8g2.clearBuffer();
}

void loop(void) {
  activeXflag = !digitalRead(inductionTriggerArduinoPin);
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


