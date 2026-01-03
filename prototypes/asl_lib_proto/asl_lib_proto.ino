#include <MicrocanAsl.h>
#include <M5Unified.h>

// These are good dip switches to set in PWRCAN for Core basic
// Core Basic
//#define CAN_TX 17
//#define CAN_RX 16

// On the Tab5, the same PWRCAN setting give you these pins
#define CAN_TX 6
#define CAN_RX 7

// CanManager coordinates CAN messages between ShiftX and TireX
CanManager ASL;

// do you want the ShiftX 7 segment display on the top?
bool displayOnTop = true;

// use some watched values to drive the alerts and the shift lights
WatchedValue rpm("RPM", "", 0, 8000);

// watching (lights, alarms) from 0-10
WatchedValue alert0("A0", "", 0, 100);

// watching from 5-10
WatchedValue alert1("A1", "", 0, 100);

// callback for ShiftX button events
void OnShiftXButton(int button, bool down)
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

// callback for when ShiftX is actually found on the CAN bus.
// put all your config here.
void OnShiftXConnect()
{
  AslShiftX3HardwareConfig config = ASL.ShiftX.GetConfig();
  M5.Lcd.printf("LEDs: %d, Alerts: %d, Length: %d\n", config.LedCount, config.AlertCount, config.BarGraphLength);
  M5.Lcd.printf("Firmware: %d.%d.%d\n", config.FirmwareMajor, config.FirmwareMinor, config.FirmwarePatch);

  // default autobrightness settings plus mounting orientation
  ASL.ShiftX.SetConfiguration(0, 61, displayOnTop);

  M5.Lcd.printf("setting alert 0\n");
  ASL.ShiftX.SetAlertWatch(0, alert0);
  M5.Lcd.printf("setting alert 1\n");
  ASL.ShiftX.SetAlertWatchHigh(1, alert1); 
  M5.Lcd.printf("setting linear graph 1\n");
  ASL.ShiftX.SetLinearGraphWatch(rpm, displayOnTop);    
}

void setup() 
{
  M5.begin();
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextScroll(true);

  // register callbacks before connection
  ASL.ShiftX.RegisterButtonCallback(OnShiftXButton);
  ASL.ShiftX.RegisterConnectCallback(OnShiftXConnect);

  if (ASL.Connect(CAN_TX, CAN_RX))
  {
    M5.Lcd.printf("CAN manager connected.\n");
  }
  else
  {
    M5.Lcd.printf("Error connecting CAN wrapper\n");
  }
}

unsigned long last = 0;
int slowCount = 0;

void loop() 
{
  // process all the CAN traffic and update ShiftX
  ASL.Update();
  
  unsigned long now = millis();
  if (now - last > 200)
  {
    last = now;
    slowCount++;

    //update shiftx displays
    ASL.ShiftX.SetDisplay(slowCount % 10);

    alert0.Value = slowCount % 100;
    alert1.Value = slowCount % 100;
    rpm.Value = (slowCount % 100) * 80;
  }
}
