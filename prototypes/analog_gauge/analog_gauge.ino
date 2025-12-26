#include <M5GFX.h>
#include <M5Unified.h>
#include <M5Dial.h>
#include "gauge.h"

Gauge* gauge;
float value = 0;

void setup() 
{
  M5.begin();

  gauge = new Gauge("Oil", 0, 100);
}

void loop() 
{
  value += 0.1;
  if (value > 100)
  {
    value = 0;
  }
  gauge->Update(value);
}
