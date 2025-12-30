#include <M5Unified.h>
#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>

#define CAN_TX 17
#define CAN_RX 16

int g_lastStamp = 0;

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
}

void loop() 
{
  CanFrame rxFrame;
  int currentStamp = millis();

  if (currentStamp - g_lastStamp > 30000) 
  {
    g_lastStamp = currentStamp;
    M5.Lcd.printf("%04d: still alive.\r\n", currentStamp / 1000);
  }

  while (ESP32Can.readFrame(rxFrame, 50))
  {
    M5.Lcd.printf("ID %X DL %03d D %03d %03d %03d %03d %03d %03d %03d %03d\r\n", rxFrame.identifier, rxFrame.data_length_code,
         rxFrame.data[0], rxFrame.data[1], rxFrame.data[2], rxFrame.data[3], rxFrame.data[4], rxFrame.data[5], rxFrame.data[6], rxFrame.data[7]);
  }
}
