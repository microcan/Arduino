#include <MicrocanAsl.h>
#include <MicrocanGauges.h>
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
int zones = 8;
int freq = 4;

TireGraph patch(zones, ASL.TireX.Temps);

WatchedValue oilP("Oil", "Bar", 0, 10);
WatchedValue oilT("Oil", "deg C", -20, 120.0);
WatchedValue waterT("Water", "deg C", -20, 120.0);
WatchedValue rpm("RPM", "", 0, 500, 5000, 6000, 6800, 8000);
WatchedValue shift("RPM", "", 4000, 4500, 5000, 6000, 6800, 7200);
WatchedValue over("Refresh", "ms", 0, 300);

// create the panel
Panel2x2 panel(oilP, oilT, waterT, rpm);


// callback for ShiftX button events
void OnShiftXButton(int button, bool down)
{

}

// callback for when ShiftX is actually found on the CAN bus.
// put all your config here.
void OnShiftXConnect()
{
  // default autobrightness settings plus mounting orientation
  ASL.ShiftX.SetConfiguration(0, 61, displayOnTop);

  ASL.ShiftX.SetAlertWatch(0, oilT);
  ASL.ShiftX.SetAlertWatchHigh(1, waterT); 
  ASL.ShiftX.SetLinearGraphWatch(shift, displayOnTop);    
}

void setup() 
{
  M5.begin();

  int w = M5.Lcd.width();
  int h = M5.Lcd.height();

  // this will only look good on a screen that is a lot more H than W
  patch.SetSize(0, 0, w, h - w);
  panel.SetSize(0, h-w, w, w);

  patch.SetPrefs(20, 40);

  // register callbacks before connection
  ASL.ShiftX.RegisterButtonCallback(OnShiftXButton);
  ASL.ShiftX.RegisterConnectCallback(OnShiftXConnect);
  ASL.TireX.SetPrefs(zones, freq);

  ASL.Connect(CAN_TX, CAN_RX);
}

unsigned long last = 0;
int slowCount = 0;

void loop() 
{
  // process all the CAN traffic and update ShiftX
  ASL.Update();
  
  unsigned long now = millis();
  //if (now - last > 100)
  {
    over.Value = (now - last);
    last = now;
    slowCount++;

    //update shiftx displays
    ASL.ShiftX.SetDisplay(slowCount % 10);

    // update watched values
    // update the watched values
    oilP.Value = oilP.Value + 0.005 * oilP.Range();
    if (oilP.Value > (oilP.High / 2)) oilP.Value = oilP.Low;
    oilT.Value = oilT.Value + 0.0062 * oilT.Range();
    if (oilT.Value > oilT.High) oilT.Value = oilT.Low;
    waterT.Value = waterT.Value + 0.003654 * waterT.Range();
    if (waterT.Value > (waterT.High)) waterT.Value = waterT.Low;
    rpm.Value = rpm.Value + 0.01 * rpm.Range();
    if (rpm.Value > 7200) rpm.Value = rpm.Low;
    shift.Value = rpm.Value;

    patch.Update();
    panel.Update();
  }
}
