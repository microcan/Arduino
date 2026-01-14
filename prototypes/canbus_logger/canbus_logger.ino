#include <M5Unified.h>
#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// Core Basic
#define CAN_TX 17
#define CAN_RX 16

// Tab5
//#define CAN_TX 6
//#define CAN_RX 7

#define SD_SPI_CS_PIN   4
#define SD_SPI_SCK_PIN  18
#define SD_SPI_MISO_PIN 38
#define SD_SPI_MOSI_PIN 23

unsigned long lastStamp = 0;
bool fileGood = false;
bool logging = false;
fs::File canFile;
bool didSomething = false;

void setup() 
{
  M5.begin();
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextScroll(true);
  M5.Lcd.println("Hello");

  if (ESP32Can.begin(ESP32Can.convertSpeed(1000), CAN_TX, CAN_RX))
  {
    M5.Lcd.printf("CAN Bus started, success.\n");
  }
  else
  {
    M5.Lcd.printf("ERROR on CAN Bus start.\n");
  }

  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) 
  {
    M5.Display.print("SD card not detected\n");
  } 
  else 
  {
    M5.Display.print("SD card detected\n");
    // get next file name
    for (int i = 1; i <= 9999; i++)
    {
      char name[256];
      sprintf(name, "/can_data_%04d.csv", i);
      if (!SD.exists(name))
      {
        canFile = SD.open(name, FILE_WRITE, true);
        if (canFile)
        {
          M5.Display.printf("Logging to %s\n", name);
          canFile.printf("tick, ID, data_len, d0, d1, d2, d3, d4, d5, d6, d7\n");
          fileGood = true;
        }
        break;
      }
    }
  }
}

void loop() 
{
  M5.update();

  CanFrame rxFrame;
  unsigned long currentStamp = millis();

  if (currentStamp - lastStamp > 30000) 
  {
    lastStamp = currentStamp;
    if (!didSomething)
    {
      M5.Lcd.printf("%04d: still alive.\r\n", currentStamp / 1000);
    }
    didSomething = false;
  }

  if (M5.BtnA.wasClicked() && !logging)
  {
    M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
    logging = true;
    M5.Lcd.printf("Logging activated\n");
  }

  if (M5.BtnC.wasClicked() && logging)
  {
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    logging = false;
    M5.Lcd.printf("Logging deactivated\n");
  }

  while (ESP32Can.readFrame(rxFrame, 0))
  {
    didSomething = true;
    M5.Lcd.printf("%X DL %03d D %03d %03d %03d %03d %03d %03d %03d %03d\n", rxFrame.identifier, rxFrame.data_length_code,
         rxFrame.data[0], rxFrame.data[1], rxFrame.data[2], rxFrame.data[3], rxFrame.data[4], rxFrame.data[5], rxFrame.data[6], rxFrame.data[7]);

    if (fileGood && logging)
    {
      canFile.printf("%06d, %05d, %03d, %03d, %03d, %03d, %03d, %03d, %03d, %03d, %03d\n", millis(), rxFrame.identifier, rxFrame.data_length_code,
         rxFrame.data[0], rxFrame.data[1], rxFrame.data[2], rxFrame.data[3], rxFrame.data[4], rxFrame.data[5], rxFrame.data[6], rxFrame.data[7]);
      canFile.flush();
    }
  }

  delay(1);
}
