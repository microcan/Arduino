#include "Microcan.h"
#include <algorithm>

int limit(int value, int low, int high)
{
    return std::min(std::max(value, low), high);
}

float limit(float value, float low, float high)
{
    return std::min(std::max(value, low), high);
}

WatchedValue::WatchedValue(String name, String units, float low, float high)
{
    Name = name;
    Units = units;
    Low = low;
    High = high;
    float range = Range();
    LowAlarm = Low + 0.1 * range;
    LowNormal = Low + 0.25 * range;
    HighNormal = Low + 0.75 * range;
    HighAlarm = Low + 0.9 * range;
    Value = Low;
}

WatchedValue::WatchedValue(String name, String units, float low, float lowAlarm, float lowNormal, float highNormal, float highAlarm, float high)
{
    Name = name;
    Units = units;
    Low = low;
    LowAlarm = lowAlarm;
    LowNormal = lowNormal;
    HighNormal = highNormal;
    HighAlarm = highAlarm;
    High = high;
    Value = Low;
}

float WatchedValue::Range()
{
    return High - Low;
}

float WatchedValue::LimitToWatch()
{
    return limit(Value, Low, High);
}

float WatchedValue::Normalized()
{
    return (LimitToWatch() - Low) / (High - Low);
}

float WatchedValue::LowAlarmActive()
{
    return Value < LowAlarm;
}

float WatchedValue::HighAlarmActive()
{
    return Value > HighAlarm;
}

void WatchedValue::ColorForValue(float value, unsigned char &r, unsigned char &g, unsigned char &b)
{
    float limited = limit(value, Low, High);

    if (limited < LowNormal)
    {
        float frac = (limited - Low) / (LowNormal - Low);
        r = 0;
        g = 255.0 * (frac);
        b = 255.0 * (1.0 - frac);
    }
    else if (limited < HighNormal)
    {
        float frac = (limited - LowNormal) / (HighNormal - LowNormal);
        r = 255.0 * frac;
        g = 255.0 * (1.0 - frac) + 165.0 * (frac);
        b = 0;
    }
    else
    {
        float frac = (limited - HighNormal) / (High - HighNormal);
        r = 255;
        g = 165.0 * (1.0 - frac);
        b = 0;
    }
}

void WatchedValue::Color(unsigned char &r, unsigned char &g, unsigned char &b)
{
    ColorForValue(Value, r, g, b);
}
