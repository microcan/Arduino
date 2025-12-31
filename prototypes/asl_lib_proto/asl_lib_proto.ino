#include <MicrocanAsl.h>
#include <M5GFX.h>
#include <M5Unified.h>

// Core Basic
//#define CAN_TX 17
//#define CAN_RX 16

// Tab5
#define CAN_TX 6
#define CAN_RX 7

AslManager asl;
bool displayOnTop = true;

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
  AslShiftX3HardwareConfig config = asl.ShiftX.GetConfig();
  M5.Lcd.printf("LEDs: %d, Alerts: %d, Length: %d\n", config.LedCount, config.AlertCount, config.BarGraphLength);
  M5.Lcd.printf("Firmware: %d.%d.%d\n", config.FirmwareMajor, config.FirmwareMinor, config.FirmwarePatch);

  asl.ShiftX.SetConfiguration(0, 61, displayOnTop);

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
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextScroll(true);

  asl.ShiftX.RegisterButtonCallback(OnButton);
  asl.ShiftX.RegisterConnectCallback(OnConnect);

  if (asl.Connect(CAN_TX, CAN_RX))
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
  if (now - last > 200)
  {
    last = now;
    slowCount++;
    asl.ShiftX.SetDisplay(slowCount % 10);
    asl.ShiftX.SetAlertValue(0, slowCount % 10);
    asl.ShiftX.SetAlertValue(1, slowCount % 20);
    asl.ShiftX.SetCustomLinearGraph(asl.TireX.Temps[FL][4], 50, 60, 60, displayOnTop);
  }
}
