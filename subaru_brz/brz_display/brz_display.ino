#include <MicrocanAsl.h>
#include <MicrocanGauges.h>
#include <M5Unified.h>
#include <M5_ADS1115.h>

// Voltmeter magic numbers
#define M5_UNIT_VMETER_I2C_ADDR 0x49
#define M5_UNIT_VMETER_EEPROM_I2C_ADDR 0x53
#define M5_UNIT_VMETER_PRESSURE_COEFFICIENT 0.015918958F

// These are good dip switches to set in PWRCAN for Core basic
// Core Basic
//#define CAN_TX 17
//#define CAN_RX 16

// On the Tab5, the same PWRCAN setting give you these pins
#define CAN_TX 6
#define CAN_RX 7

// CanManager coordinates CAN messages between ShiftX and TireX
CanManager ASL;

ADS1115 meter;

float g_voltRes = 0.0;
float g_voltCal = 0.0;

//Oil pressure sender calibration
float g_vMin = 0.47;
float g_pMin = 0.0;
float g_vMax = 5.0;
float g_pMax = 10.0;

// do you want the ShiftX 7 segment display on the top?
bool displayOnTop = true;

//number of zones and update freq for TireX
int zones = 4;
int freq = 4;

// scaleable display for tire patches
TireGraph patch(zones, ASL.TireX.Temps);

// Some watched values to drive gauges and ShiftX display
WatchedValue oilP("Oil Press", "Bar", 0, 10);
WatchedValue oilT("Oil Temp", "deg C", 30, 55, 65, 120, 130, 150.0);
WatchedValue waterT("Water", "deg C", 30, 55, 65, 120, 130, 150.0);
// different watched values for RPM gauge vs shift lights as we want different
// low ends on te gauge (0) vs shift lights (4000)
WatchedValue rpm("RPM", "", 0, 500, 5133, 6266, 7000, 8000);
WatchedValue shift("Shift", "", 4000, 4500, 5133, 6266, 7000, 7400);
WatchedValue over("Refresh", "ms", 0, 300);
WatchedValue speed("Speed", "kph", 0, 200);

// scaleable display for four gauges, set up by which watched values you give it
Panel2x2 panel(oilT, waterT, oilP, rpm);

// callback for ShiftX button events
void OnShiftXButton(int button, bool down) {
  // Scale the tire patches to current temp range
  if (button == 0 && down) {
    patch.SetPrefs(40, 90);
  }
  // scale thetre patches to full range
  if (button == 1 && down) {
    patch.SetPrefs(50, 80);
  }
}

// callback for when ShiftX is actually found on the CAN bus.
// put all your config here.
void OnShiftXConnect() {
  // default autobrightness settings plus mounting orientation
  ASL.ShiftX.SetConfiguration(0, 61, displayOnTop);

  ASL.ShiftX.SetAlertWatch(1, oilT);
  ASL.ShiftX.SetAlertWatch(0, waterT);
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

  // Ventus RS4 ranges
  // 50 is low end
  // 55-75 seems optmal
  // 45-85 as wide range
  // 90 as a max temp
  patch.SetPrefs(50, 80);

  // register callbacks and set prefs before connection
  ASL.ShiftX.RegisterButtonCallback(OnShiftXButton);
  ASL.ShiftX.RegisterConnectCallback(OnShiftXConnect);
  ASL.TireX.SetPrefs(zones, freq);

  // connect to CAN
  ASL.Connect(CAN_TX, CAN_RX);

    if (!meter.begin(&Wire, M5_UNIT_VMETER_I2C_ADDR, 53, 54, 400000U)) {
    delay(500);
    meter.begin(&Wire, M5_UNIT_VMETER_I2C_ADDR, 53, 54, 400000U);
  }

  meter.setEEPROMAddr(M5_UNIT_VMETER_EEPROM_I2C_ADDR);
  meter.setMode(ADS1115_MODE_SINGLESHOT);
  meter.setRate(ADS1115_RATE_8);
  meter.setGain(ADS1115_PGA_256);

  g_voltRes = meter.getCoefficient() / M5_UNIT_VMETER_PRESSURE_COEFFICIENT;
  g_voltCal = meter.getFactoryCalibration();
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
    if (ASL.Subaru.clutch)
    {
      ASL.ShiftX.SetDisplay('C');
    }
    else
    {
      ASL.ShiftX.SetDisplay(ASL.Subaru.gear);
    }

    int16_t adc_raw = meter.getSingleConversion();
    float volt = adc_raw * g_voltRes * g_voltCal / 1000.0;
    float raw = (volt - g_vMin) / (g_vMax - g_vMin) * (g_pMax - g_pMin) + g_pMin;
    oilP.Value = 0.7 * oilP.Value + 0.3 * raw;

    // values from CANBUS
    oilT.Value = ASL.Subaru.oilTemp;
    waterT.Value = ASL.Subaru.waterTemp;
    rpm.Value = ASL.Subaru.rpm;
    shift.Value = ASL.Subaru.rpm;
    speed.Value = ASL.Subaru.speed;

    // update the tire patch display
    patch.Update();
    // update the gauges in the gauge panel
    panel.Update();
  }
}
