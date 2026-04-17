#include "SubaruCan.h"

unsigned long WhiteList[] =
    {
        SUB_STEER,
        SUB_SPEED,
        SUB_WHEEL,
        SUB_RPM_ACC,
        SUB_OIL_COOL};

unsigned long LastUpdate[] =
    {
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0};

bool SubaruPassFilter(unsigned long id, int minTimeMs)
{
  unsigned long now = millis();
  for (int j = 0; j < 5; j++)
  {
    // Serial.printf("Filter %d ", j);
    if (id == WhiteList[j])
    {
      if ((now - LastUpdate[j]) > minTimeMs)
      {
        LastUpdate[j] = now;
        // Serial.println("passed");
        return true;
      }
    }
    // Serial.println("failed");
  }
  return false;
};

void SubaruProcessFrame(CanFrame frame, SubiValues &values)
{
  if (frame.identifier == SUB_OIL_COOL)
  {
    // oil temp
    values.oilTemp = frame.data[2] - 40;
    // water temp
    values.waterTemp = frame.data[3] - 40;
  }
  else if (frame.identifier == SUB_RPM_ACC)
  {
    // rpm
    uint8_t hiByte = frame.data[3] << 3;
    values.rpm = (hiByte * 32) + frame.data[2];

    uint8_t clutch = frame.data[1] >> 7;
    if (clutch > 0)
    {
      values.clutch = true;
    }
    else
    {
      values.clutch = false;
    }
  }
  else if (frame.identifier == SUB_SPEED)
  {
    // speed in kph
    values.speed = (frame.data[0] + frame.data[1] * 256) / 17.4;

    if (values.speed > 0)
    {
      // wheel RPM, 8.447 is kph to m/min and tire circum of 1.973m 
      float wheelRpm = values.speed * 8.447;

      // gear ratio with final ratio of 4.1
      float ratio = values.rpm / wheelRpm / 4.10;

      // based on ratio half between the real ratios
      // of 3.626, 2.188, 1.541, 1.213, 1.0, 0.767
      if (ratio > 2.82) values.gear = 1;
      else if (ratio > 1.84) values.gear = 2;
      else if (ratio > 1.37) values.gear = 3;
      else if (ratio > 1.10) values.gear = 4;
      else if (ratio > 0.88) values.gear = 5;
      else values.gear = 6;

    }
    else
    {
      values.gear = 0;
    }
  }
}
