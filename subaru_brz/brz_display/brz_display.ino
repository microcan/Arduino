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

//number of zones and update freq for TireX
int zones = 8;
int freq = 4;

// scaleable display for tire patches
TireGraph patch(zones, ASL.TireX.Temps);

// Some watched values to drive gauges and ShiftX display
WatchedValue oilP("Oil P", "Bar", 0, 10);
WatchedValue oilT("Oil T", "deg C", -20, 120.0);
WatchedValue waterT("Water", "deg C", -20, 120.0);
// different watched values for RPM gauge vs shift lights as we want different
// low ends on te gauge (0) vs shift lights (4000)
WatchedValue rpm("RPM", "", 0, 500, 5000, 6000, 6800, 8000);
WatchedValue shift("Shift", "", 4000, 4500, 5000, 6000, 6800, 7200);
WatchedValue over("Refresh", "ms", 0, 300);

// scaleable display for four gauges, set up by which watched values you give it
Panel2x2 panel(oilP, oilT, waterT, over);

// callback for ShiftX button events
void OnShiftXButton(int button, bool down) {
  // zero the simulated water temp on button 0
  if (button == 0 && down) {
    patch.ScaleToRange();
  }
  // zero the simulated oil temp on button 1
  if (button == 1 && down) {
    patch.SetPrefs(0, 127);
  }
}

// callback for when ShiftX is actually found on the CAN bus.
// put all your config here.
void OnShiftXConnect() {
  // default autobrightness settings plus mounting orientation
  ASL.ShiftX.SetConfiguration(0, 61, displayOnTop);

  ASL.ShiftX.SetAlertWatch(0, oilT);
  ASL.ShiftX.SetAlertWatch(1, waterT);
  ASL.ShiftX.SetLinearGraphWatch(shift, displayOnTop);
}

void setup() {
  M5.begin();

  int w = M5.Lcd.width();
  int h = M5.Lcd.height();

  // this will only look good on a screen that is a lot more H than W
  // size the tire tempdisplay
  patch.SetSize(0, 0, w, h - w);
  // size the gauge display
  panel.SetSize(0, h - w, w, w);

  // scale the tire temp display to a gradient for full range 0 deg C to 127 degC
  patch.SetPrefs(0, 127);

  // register callbacks and set prefs before connection
  ASL.ShiftX.RegisterButtonCallback(OnShiftXButton);
  ASL.ShiftX.RegisterConnectCallback(OnShiftXConnect);
  ASL.TireX.SetPrefs(zones, freq);

  // connect to CAN
  ASL.Connect(CAN_TX, CAN_RX);
}

unsigned long last = 0;
int slowCount = 0;

void loop() {
  // process all the CAN traffic and update ShiftX
  ASL.Update();

  // update displays and values every 100ms
  unsigned long now = millis();
  if (now - last > 100)
  {
    over.Value = (now - last);
    last = now;
    slowCount++;

    //update shiftx 7-segment display
    ASL.ShiftX.SetDisplay(ASL.Subaru.gear);

    // simulate changing values from sensors by updating values for the watched items
    // in a real implementation here is where you would update the values with the actual
    // sensor readings
    oilP.Value = oilP.Value + 0.005 * oilP.Range();
    if (oilP.Value > (oilP.High / 2)) oilP.Value = oilP.Low;

    // values from CANBUS
    oilT.Value = ASL.Subaru.oilTemp;
    waterT.Value = ASL.Subaru.waterTemp;
    rpm.Value = ASL.Subaru.rpm;
    shift.Value = ASL.Subaru.rpm;

    // update the tire patch display
    patch.Update();
    // update the gauges in the gauge panel
    panel.Update();
  }
  sleep(1);
}
