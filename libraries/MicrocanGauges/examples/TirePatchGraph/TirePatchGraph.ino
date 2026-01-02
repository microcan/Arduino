#include <M5Unified.h>
#include <MicrocanGauges.h>
#include <Microcan.h>

// data to drive the tire patch temps
// Following ASL TireX data, byte temps are 2 * deg C
byte temps[16];

// create a tire patch graph
TirePatchGraph graph(16, temps, false);

void setup() 
{
  int w, h;

  // setup the hardware
  M5.begin();
  w = min((int)M5.Lcd.width(), 300);
  h = min((int)M5.Lcd.height(), 300);
  
  // size and position the gauge
  graph.SetSize(10, 10, w, h);

  // set temp range for colors from 20 deg c to 100 deg c
  graph.SetPrefs(20, 100);
}

unsigned long last = 0;

void loop() 
{
  unsigned long now = millis();

  // every 100ms
  if (now - last > 200) 
  {
    last = now;

    // update the temp values with a ~random ramp
    // raw byte temps are 2 x deg C
    for (int i = 0; i < 16; i++)
    {
      temps[i] = random(128) + 8 * i;
    }
    
    // update the gauge
    graph.Update();
  }
}
