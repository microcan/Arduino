#include <M5Unified.h>
#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>
#include <esp_now.h>
#include <WiFi.h>

#define CAN_TX 17
#define CAN_RX 16

byte g_recvAdd[] = {0xA0, 0xDD, 0x6C, 0x68, 0xF9, 0xA8};

int g_lastStamp = 0;

void OnSend(const byte *macAdd, esp_now_send_status_t status) 
{
  if (status != ESP_NOW_SEND_SUCCESS)
  {
    M5.Lcd.printf("Send failed.\n");
  }
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

  esp_now_register_send_cb(OnSend);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, g_recvAdd, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
       
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    M5.Lcd.printf("Failed to add peer.\n");
  }

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
      
    esp_err_t result = esp_now_send(g_recvAdd, (byte *) &rxFrame, 40); //sizeof(rxFrame));
   
    if (result != ESP_OK) 
    {
      M5.Lcd.printf("Error sending the data.\n");
    }
  }
}
