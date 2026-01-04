// A Panel2x2 is used to layout 4 Analog Guages in 
// a 2x2 grid.  WatchedValues drive the gauges, and
// in the loop, you just update the wached values
// and the panel

#include <M5Unified.h>
#include <MicrocanGauges.h>

// used watched values to drive the gauges in the panel
WatchedValue oilP("Oil", "Bar", 0, 10);
WatchedValue oilT("Oil", "deg C", -20, 120.0);
WatchedValue waterT("Water", "deg C", -20, 120.0);
WatchedValue rpm("RPM", "", 0, 7200);

// create the panel
Panel2x2 panel(oilP, oilT, waterT, rpm);

void setup() 
{
  int w, h;

  // setup thehardware
  M5.begin();
  w = M5.Lcd.width();
  h = M5.Lcd.height();

  int size = min(w, h);
  panel.SetSize(0, 0, size, size);
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
    if (oilP.Value > (oilP.High / 2)) oilP.Value = oilP.Low;

    oilT.Value = oilT.Value + 0.0062 * oilT.Range();
    if (oilT.Value > oilT.High) oilT.Value = oilT.Low;

    waterT.Value = waterT.Value + 0.003654 * waterT.Range();
    if (waterT.Value > (waterT.High * 0.8)) waterT.Value = waterT.Low;

    rpm.Value = rpm.Value + 0.01 * rpm.Range();
    if (rpm.Value > rpm.High) rpm.Value = rpm.Low;

    // update the panel
    panel.Update();
  }
}
