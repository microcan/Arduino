#include <M5GFX.h>
#include <M5Unified.h>
#include <M5Dial.h>
#include "gauge.h"

Gauge* gauge;
float value = 30;

void setup() 
{
  M5.begin();

  gauge = new Gauge("Oil", "deg C", 30, 130);
  //gauge->SetSize(20, 20, 100, 100);
}

void loop() 
{
  value += 0.1;
  if (value > 130)
  {
    value = 30;
  }
  gauge->Update(value);
}
