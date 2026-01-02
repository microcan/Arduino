#include <MicrocanAsl.h>
#include <M5GFX.h>
#include <M5Unified.h>

// Core Basic
//#define CAN_TX 17
//#define CAN_RX 16

// Tab5
#define CAN_TX 6
#define CAN_RX 7

AslManager ASL;

bool displayOnTop = true;

WatchedValue tire("Tire", "", 50, 60);
WatchedValue alert0("A0", "", 0, 10);
WatchedValue alert1("A1", "", 0, 20);

void OnButton(int button, bool down)
{
  if (down)
  {
    M5.Lcd.printf("Button %d DOWN\n", button);
    ASL.ShiftX.SetDisplay(button);
  }
  else
  {
    M5.Lcd.printf("Button %d UP\n", button);
  }
}

void OnConnect()
{
  AslShiftX3HardwareConfig config = ASL.ShiftX.GetConfig();
  M5.Lcd.printf("LEDs: %d, Alerts: %d, Length: %d\n", config.LedCount, config.AlertCount, config.BarGraphLength);
  M5.Lcd.printf("Firmware: %d.%d.%d\n", config.FirmwareMajor, config.FirmwareMinor, config.FirmwarePatch);

  ASL.ShiftX.SetConfiguration(0, 61, displayOnTop);

  ASL.ShiftX.SetAlertThresholds(0, alert0);
  ASL.ShiftX.SetAlertThresholds(1, alert1);  
}

void setup() 
{
  M5.begin();
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextScroll(true);

  AslTireXPrefs prefs;
  prefs.SampleRate = 8;
  prefs.Zones = 1;
  ASL.TireX.SetPrefs(prefs);

  ASL.ShiftX.RegisterButtonCallback(OnButton);
  ASL.ShiftX.RegisterConnectCallback(OnConnect);

  if (ASL.Connect(CAN_TX, CAN_RX))
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
  ASL.Update();
  
  unsigned long now = millis();
  if (now - last > 200)
  {
    last = now;
    slowCount++;
    ASL.ShiftX.SetDisplay(slowCount % 10);
    ASL.ShiftX.SetAlertValue(0, slowCount % 10);
    ASL.ShiftX.SetAlertValue(1, slowCount % 20);
    tire.Value = ASL.TireX.Temps[FL][4];
    ASL.ShiftX.SetCustomLinearGraph(tire, displayOnTop);
  }
}
