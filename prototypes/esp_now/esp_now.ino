#include <M5Unified.h>
#include <WiFi.h>

void setup()
{
  M5.begin();

  WiFi.mode(WIFI_MODE_STA);
  
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextScroll(true);
  M5.Lcd.println("Hello");
  byte mac[6];
  WiFi.macAddress(mac);
  M5.Lcd.printf("MAC address: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  M5.Lcd.println(WiFi.macAddress());
}
 
void loop(){

}
