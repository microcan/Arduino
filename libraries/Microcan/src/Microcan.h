#ifndef MICROCAN_COMMON_H
#define MICROCAN_COMMON_H

// limit int value to be in the range from low to high
int limit(int value, int low, int high);

// limit float value to be in the range from low to high
float limit(float value, float low, float high);

// all then information needed for displays to decide whether
// a watched value is alarmingly low or high.  
class WatchedValue
{
public:
  // The current value of thewatched item.
  float Value;

  // The low end of the watch.  The value might go lower, 
  // but this is the low end of the watch.  For instance, 
  // RPM might go from 500 to 7000, but if you want your shift lights 
  // to start at 4000, low would be 4000. Also the BLUE point in
  // color 
  float Low;

  // Values below LowAlarm deserve some kind of alarm notification. Can be any value.
  float LowAlarm;

  // The normal range starts here.  The GREEN point in color.  Must be > Low.
  float LowNormal;

  // The top of the normal range.  The ORANGE point in color. Must be > LowNormal.
  float HighNormal;

  // Values above HighAlarm deserve some kind of alarm notification.  Can be any value.
  float HighAlarm;

  // Watch tops out here.  The RED point in color.  Values can go higher, but
  // graphs and color are topped out.  Must be > HighNormal
  float High;

  // Just provide a low and high and let the code set up some reasonable normal and alarm ranges
  WatchedValue(float low, float high);

  // Constructor where you provide all the details.  high > highNormal > lowNormal > low.
  // Alarm levels can be anything but would be weird for them to not be lowNormal > lowAlarm > low and 
  // high > highAlarm > highNormal
  WatchedValue(float low, float lowAlarm, float lowNormal, float highNormal, float highAlarm, float high);

  // Returns High - Low.
  float Range();

  // Returns Value limited to be High >= return >= Low.
  float LimitToWatch();

  // Returns 0.0 if Value <= Low up to 1.0 if Value >= High
  float Normalized();

  // Returns Value < LowAlarm
  float LowAlarmActive();

  // Returns Value > HighAlarm
  float HighAlarmActive();

  // Returns the color in a BLUE->GREEN->ORANGE->RED color ramp for Value
  void Color(unsigned char &r, unsigned char &g, unsigned char &b);

  // Returns the color in a BLUE->GREEN->ORANGE->RED color ramp for value
  void ColorForValue(float value, unsigned char &r, unsigned char &g, unsigned char &b);
};

#endif