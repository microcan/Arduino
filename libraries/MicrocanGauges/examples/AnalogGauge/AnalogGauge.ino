#include <M5Unified.h>
#include <MicrocanGauges.h>

// used watched values to drive gauges
WatchedValue oilP("Oil", "Bar", 0, 10);

// create a gauge
AnalogGauge gauge(oilP);

void setup() 
{
  int w, h;

  // setup the hardware
  M5.begin();
  w = M5.Lcd.width();
  h = M5.Lcd.height();
  
  // size and position the gauge
  gauge.SetSize(10, 10, w - 10, h - 10);
}

unsigned long last = 0;

void loop() 
{
  unsigned long now = millis();

  // every 100ms
  if (now - last > 100) 
  {
    last = now;

    // update the watched value
    oilP.Value = oilP.Value + 0.005 * oilP.Range();
    if (oilP.Value > oilP.High) oilP.Value = oilP.Low;

    // update the gauge
    gauge.Update();
  }
}
