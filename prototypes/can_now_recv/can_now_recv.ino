#include <M5Unified.h>
#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>
#include <esp_now.h>
#include <WiFi.h>

#define CAN_TX 17
#define CAN_RX 16

byte g_recvAdd[] = {0xA0, 0xDD, 0x6C, 0x68, 0xF9, 0xA8};

int g_lastStamp = 0;

void PrintCan(CanFrame in)
{
  M5.Lcd.printf("ID %X DL %03d D %03d %03d %03d %03d %03d %03d %03d %03d\r\n", in.identifier, in.data_length_code,
    in.data[0], in.data[1], in.data[2], in.data[3], in.data[4], in.data[5], in.data[6], in.data[7]);
}

void OnRecv(const esp_now_recv_info* info, const byte *incomingData, int len) 
{
  CanFrame rxFrame;
  memcpy(&rxFrame, incomingData, sizeof(rxFrame));
  PrintCan(rxFrame);
}

void setup() 
{
  M5.begin();
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextScroll(true);
  M5.Lcd.println("Hello");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) 
  {
    M5.Lcd.printf("Error initializing ESP-NOW.\n");
  }

  esp_now_register_recv_cb(OnRecv);

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
    PrintCan(rxFrame);
  }
}
