#include <MicrocanAsl.h>
#include <M5GFX.h>
#include <M5Unified.h>

AslManager asl;

void OnButton(int button, bool down)
{
  if (down)
  {
    M5.Lcd.printf("Button %d DOWN\n", button);
    asl.ShiftX.SetDisplay(button);
  }
  else
  {
    M5.Lcd.printf("Button %d UP\n", button);
  }
}

void OnConnect()
{
  M5.Lcd.printf("LEDs: %d, Alerts: %d, Length: %d\n", asl.ShiftX.LedCount, asl.ShiftX.AlertCount, asl.ShiftX.BarGraphLength);
  M5.Lcd.printf("Firmware: %d.%d.%d\n", asl.ShiftX.FirmwareMajor, asl.ShiftX.FirmwareMinor, asl.ShiftX.FirmwarePatch);

  asl.ShiftX.SetConfiguration(10, 61, false);

  asl.ShiftX.SetAlertThreshold(0, 0, 0,   0,   0, 255, 0);
  asl.ShiftX.SetAlertThreshold(0, 1, 2,   0, 255, 255, 0);
  asl.ShiftX.SetAlertThreshold(0, 2, 4,   0, 255,   0, 0);
  asl.ShiftX.SetAlertThreshold(0, 3, 6, 255, 255,   0, 0);
  asl.ShiftX.SetAlertThreshold(0, 4, 8, 255,   0,   0, 5);

  asl.ShiftX.SetAlertThreshold(1, 0,  0, 255,   0,   0, 0);
  asl.ShiftX.SetAlertThreshold(1, 1,  4, 255, 255,   0, 0);
  asl.ShiftX.SetAlertThreshold(1, 2,  8,   0, 255,   0, 0);
  asl.ShiftX.SetAlertThreshold(1, 3, 12,   0, 255, 255, 0);
  asl.ShiftX.SetAlertThreshold(1, 4, 16,   0,   0, 255, 5);
  
}

void setup() 
{
  M5.begin();
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextScroll(true);

  asl.ShiftX.RegisterButtonCallback(OnButton);
  asl.ShiftX.RegisterConnectCallback(OnConnect);

  if (asl.Connect(17, 16))
  {
    M5.Lcd.printf("ASL manager connected.\n");
  }
  else
  {
    M5.Lcd.printf("Error connecting ASL wrapper\n");
  }
}

unsigned long last = 0;
int slowCount = 0;

void loop() 
{
  asl.Update();
  
  unsigned long now = millis();
  if (now - last > 1000)
  {
    last = now;
    slowCount++;
    asl.ShiftX.SetDisplay(slowCount % 10);
    asl.ShiftX.SetAlertValue(0, slowCount % 10);
    asl.ShiftX.SetAlertValue(1, slowCount % 20);
  }

}
