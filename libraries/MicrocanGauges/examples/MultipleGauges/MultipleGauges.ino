#include <M5Unified.h>
#include <MicrocanGauges.h>

// show an array of gauges
AnalogGauge* gauges[4];

// used watched values to drive gauges
WatchedValue oilP("Oil", "Bar", 0, 10);
WatchedValue oilT("Oil", "deg C", -20, 120.0);
WatchedValue waterT("Water", "deg C", -20, 120.0);
WatchedValue rpm("RPM", "", 0, 7200);

void setup() 
{
  int w, h;

  // setup thehardware
  M5.begin();
  w = M5.Lcd.width();
  h = M5.Lcd.height();

  // create four gauges watching the values
  gauges[0] = new AnalogGauge(oilT);
  gauges[1] = new AnalogGauge(waterT);
  gauges[2] = new AnalogGauge(oilP);
  gauges[3] = new AnalogGauge(rpm);

  // size and layout the gauges
  //gauges[0]->SetSize(20, 20, w / 2 - 20, w / 2 - 20);
  //gauges[1]->SetSize(w / 2 + 20, 20, w / 2 - 20, w / 2 - 20);
  //gauges[2]->SetSize(20, w / 2 + 20, w / 2 - 20, w / 2 - 20);
  //gauges[3]->SetSize(w / 2 + 20, w / 2 + 20, w / 2 - 20, w / 2 - 20);

  gauges[0]->SetSize(10, 10, 100, 100);
  gauges[1]->SetSize(10, 120, 200, 200);
  gauges[2]->SetSize(10, 330, 300, 300);
  gauges[3]->SetSize(10, 640, 500, 500);

  //int small = w/3 -20;
  //gauges[0]->SetSize(10, 520, small, small);
  //gauges[1]->SetSize(30 + small, 520, small, small);
  //gauges[2]->SetSize(50 + 2 * small, 520, small, small);
  //gauges[3]->SetSize(w / 2 - 250, 10, 500, 500);
}

unsigned long last = 0;

void loop() 
{
  unsigned long now = millis();

  // every 100ms
  if (now - last > 100) 
  {
    last = now;

    // update the watched values
    oilP.Value = oilP.Value + 0.005 * oilP.Range();
    if (oilP.Value > oilP.High) oilP.Value = oilP.Low;

    oilT.Value = oilT.Value + 0.0062 * oilT.Range();
    if (oilT.Value > oilT.High) oilT.Value = oilT.Low;

    waterT.Value = waterT.Value + 0.003654 * waterT.Range();
    if (waterT.Value > (waterT.High * 0.8)) waterT.Value = waterT.Low;

    rpm.Value = rpm.Value + 0.01 * rpm.Range();
    if (rpm.Value > rpm.High) rpm.Value = rpm.Low;

    // update the gauges
    gauges[0]->Update();
    gauges[1]->Update();
    gauges[2]->Update();
    gauges[3]->Update();
  }
}
