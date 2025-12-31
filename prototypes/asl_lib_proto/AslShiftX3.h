#ifndef ASL_SHIFTX3_H
#define ASL_SHIFTX3_H

#include <ESP32-TWAI-CAN.hpp>

class AslShiftX3
{
    private:

    void (*m_buttonCallback)(int, bool) = nullptr;
    void (*m_connectCallback)() = nullptr;

    public:

    int LedCount = 9;
    int AlertCount = 2;
    int BarGraphLength = 7;
    bool DetailsUpdated = false;
    bool Connected = false;
    int FirmwareMajor = 0;
    int FirmwareMinor = 0;
    int FirmwarePatch = 0;

    void Process(CanFrame frame);
    void RegisterButtonCallback(void (*callback)(int, bool));
    void RegisterConnectCallback(void (*callback)());

    // Control the shiftx3 hardware, see:
    // https://wiki.autosportlabs.com/ShiftX3#ShiftX3_CAN_bus_API

    // Set the gear indicator LED to a number from 0 to 9
    // value: 0-9, gets displayed in the 7 segment 'gear indicator'
    bool SetDisplay(int value);

    // Set the general configuration
    // brightness: 0 = auto, 1 = dim, 100 = brightest
    // autoBrightScaling: 0-255, default = 61
    // displayOnTop: set to true if orientation is gear indicator above shift/graph LED strip
    bool SetConfiguration(int brightness, int autoBrightScaling, bool displayOnTop);

    // Directly set the state of LEDs on the shift light strip
    // startIndex: 0-6, the 0 based index of the first LED to set
    // count: 0-7, the number of LEDs to set, 0 for all remaining
    // r, g, b: -0-255, the colour to set
    // flashHz: 0-10, the flash rate in Hz, 0 for solid
    bool SetLed(int startIndex, int count, byte r, byte g, byte b, int flashHz);

    // Directly set the state of Alert indicator
    // index: 0-1, the 0 based index of the alert to set
    // r, g, b: -0-255, the colour to set
    // flashHz: 0-10, the flash rate in Hz, 0 for solid
    bool SetAlert(int index, byte r, byte g, byte b, int flashHz);

    // Set alert thresholds for automatic allert light state by value
    // index: 0-1, the 0 based index of the alert indicator
    // id: 0-4, five thresholds for state change canbe set.  first
    //              can be >= 0, others needs to be > 0
    // threshold: >=0, the value at which this threshold comes into effect
    // r, g, b: 0-255, the alert light collor
    // flashHz: 0-10, the flash rate in Hz, 0 for solid
    bool SetAlertThreshold(int index, int id, unsigned int threshold, byte r, byte g, byte b, int flashHz);

    // Set the value to an alert light set the light automatically by the
    // thresholds set using SetAlertThreshold
    // index: 0-1, the 0 based index of the alert indicator
    // value: >=0, the value to be comapred to the thresholds that have been set
    bool SetAlertValue(int index, unsigned int value);

    bool SetCustomLinearGraph(float value, float low, float high);
};

#endif