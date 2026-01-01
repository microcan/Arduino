#include <M5GFX.h>
#include <M5Unified.h>
#include <MicrocanGauges.h>

AnalogGauge* gauges[4];
float value = 30;

void setup() 
{
  int w, h;
  M5.begin();

  w = M5.Lcd.width();
  h = M5.Lcd.height();

  gauges[0] = new AnalogGauge("Oil", "deg C", 30, 130);
  gauges[1] = new AnalogGauge("Water", "deg C", 30, 130);
  gauges[2] = new AnalogGauge("Oil", "Bar", 30, 130);
  gauges[3] = new AnalogGauge("RPM", "", 30, 130);
  gauges[0]->SetSize(20, 20, w/2 - 20, w/2 - 20);
  gauges[1]->SetSize(w/2 + 20, 20, w/2 - 20, w/2 - 20);
  gauges[2]->SetSize(20, w/2 + 20, w/2 - 20, w/2 - 20);
  gauges[3]->SetSize(w/2 + 20, w/2 + 20, w/2 - 20, w/2 - 20);
}

void loop() 
{
  value += 0.2;
  if (value > 130)
  {
    value = 30;
  }
  gauges[0]->Update(value);
  gauges[1]->Update(value);
  gauges[2]->Update(value);
  gauges[3]->Update(value);
}
