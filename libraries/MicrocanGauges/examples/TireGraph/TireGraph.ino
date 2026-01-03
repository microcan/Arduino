#include <M5Unified.h>
#include <MicrocanGauges.h>

// data to drive the tire patch temps
// Following ASL TireX data, byte temps are 2 * deg C
byte temps[4][16];

// create a tire graph that will show four patches
TireGraph graph(16, temps);

void setup() 
{
  int size;

  // setup the hardware
  M5.begin();
  size = min((int)M5.Lcd.width(), (int)M5.Lcd.height());
  
  // size and position the graph
  graph.SetSize(10, 10, size - 20, size - 20);

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
    for (int j = 0; j < 4; j++)
    {
      for (int i = 0; i < 16; i++)
      {
        temps[j][i] = random(32) + 14 * i;
      }
    }
    
    // update the graph
    graph.Update();
  }
}
