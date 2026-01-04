#include <M5Unified.h>
#include <MicrocanAsl.h>
#include <MicrocanGauges.h>

// CAN GPIO pins
int canTx = 6;
int canRx = 7;

// Number of zones and update frequency for TireX
int zones = 8;
int freq = 4;

// CanManager managers the CAN connection and sends messages to TireX orShiftX3 as appropriate
CanManager asl;

// TireGraph is a scaleable control to display all four tire patches
TireGraph patch(zones, asl.TireX.Temps);

void setup() 
{
  M5.begin();

  asl.TireX.SetPrefs(zones, freq);
  asl.Connect(canTx, canRx);

  // size the patch display to be square
  int size = min(M5.Lcd.width(), M5.Lcd.height());
  patch.SetSize(0, 0, size, size);

  // set the temperature range to display in deg C
  // 20deg or colder will be blue, 40deg or hotter will be red
  patch.SetPrefs(20, 40);
}

unsigned long last = 0;

void loop() 
{
  unsigned long now = millis();
  // update processing all the can messages
  asl.Update();
  
  // every 100ms update the tire patch display
  if (now - last > 100) 
  {
    last = now;
    patch.Update();
  } 
}
