#include <Arduino.h>
#include <M5Unified.h>
#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>
#include <esp_now.h>
#include <WiFi.h>

#define CAN_TX 6
#define CAN_RX 7

#define SDIO2_CLK GPIO_NUM_12
#define SDIO2_CMD GPIO_NUM_13
#define SDIO2_D0  GPIO_NUM_11
#define SDIO2_D1  GPIO_NUM_10
#define SDIO2_D2  GPIO_NUM_9
#define SDIO2_D3  GPIO_NUM_8
#define SDIO2_RST GPIO_NUM_15

byte g_recvAdd[] = {0xA0, 0xDD, 0x6C, 0x68, 0xF9, 0xA8};

int g_lastStamp = 0;

const char *ssid     = "Gasthof";
const char *password = "Puflatsch2016";

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
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextScroll(true);
  M5.Lcd.println("Hello");

  WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3, SDIO2_RST);

  WiFi.mode(WIFI_STA);
  //if (esp_now_init() != ESP_OK) 
  //{
  //  M5.Lcd.printf("Error initializing ESP-NOW.\n");
  //}
    M5.Display.println("WiFi mode set to STA");
    WiFi.begin(ssid, password);
    M5.Display.print("Connecting to ");
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Display.print(".");
    }

    M5.Display.println("");
    M5.Display.print("Connected to ");
    M5.Display.println(ssid);
    M5.Display.print("IP address: ");
    M5.Display.println(WiFi.localIP());
    
  byte mac[6];
  WiFi.macAddress(mac);
  M5.Lcd.printf("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  M5.Lcd.println(WiFi.macAddress());

  //esp_now_register_recv_cb(OnRecv);

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
