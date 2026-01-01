#include <M5GFX.h>
#include <M5Unified.h>
#include <MicrocanGauges.h>
#include <Microcan.h>

AnalogGauge* gauges[4];
float value = 30;

WatchedValue oilP(0, 10);
WatchedValue oilT(-20, 120.0);
WatchedValue waterT(-20, 120.0);
WatchedValue rpm(0, 7200);

void setup() 
{
  int w, h;
  M5.begin();

  w = M5.Lcd.width();
  h = M5.Lcd.height();

  gauges[0] = new AnalogGauge("Oil", "deg C", oilT);
  gauges[1] = new AnalogGauge("Water", "deg C", waterT);
  gauges[2] = new AnalogGauge("Oil", "Bar", oilP);
  gauges[3] = new AnalogGauge("RPM", "", rpm);
  gauges[0]->SetSize(20, 20, w / 2 - 20, w / 2 - 20);
  gauges[1]->SetSize(w / 2 + 20, 20, w / 2 - 20, w / 2 - 20);
  gauges[2]->SetSize(20, w / 2 + 20, w / 2 - 20, w / 2 - 20);
  gauges[3]->SetSize(w / 2 + 20, w / 2 + 20, w / 2 - 20, w / 2 - 20);
}

unsigned long last = 0;

void loop() 
{
  unsigned long now = millis();

  if (now - last > 100) 
  {
    last = now;

    oilP.Value = oilP.Value + 0.005 * oilP.Range();
    if (oilP.Value > oilP.High) oilP.Value = oilP.Low;

    oilT.Value = oilT.Value + 0.0062 * oilT.Range();
    if (oilT.Value > oilT.High) oilT.Value = oilT.Low;

    waterT.Value = waterT.Value + 0.003654 * waterT.Range();
    if (waterT.Value > waterT.High) waterT.Value = waterT.Low;

    rpm.Value = rpm.Value + 0.01 * rpm.Range();
    if (rpm.Value > rpm.High) rpm.Value = rpm.Low;

    gauges[0]->Update();
    gauges[1]->Update();
    gauges[2]->Update();
    gauges[3]->Update();
  }
}
