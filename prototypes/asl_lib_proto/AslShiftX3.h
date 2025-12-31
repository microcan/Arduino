#ifndef ASL_SHIFTX3_H
#define ASL_SHIFTX3_H

#include <ESP32-TWAI-CAN.hpp>

struct AslShiftX3HardwareConfig
{
    // LED count includes the alert LEDs
    int LedCount = 9;
    // The number of Alert LEDs
    int AlertCount = 2;
    // The number of LEDs in the linear graph
    int BarGraphLength = 7;
    // Firmware details get populated by the Announce or Stats messages
    int FirmwareMajor = 0;
    int FirmwareMinor = 0;
    int FirmwarePatch = 0;
};

class AslShiftX3
{
    private:

    void (*m_buttonCallback)(int, bool) = nullptr;
    void (*m_connectCallback)() = nullptr;

    AslShiftX3HardwareConfig m_config;
    bool m_detailsUpdated = false;
    bool m_connected = false;
    
    public:

    // Becomes true when any notification is received from the module
    bool GetConnected();

    // Becomes true when the hardware Announce message is received
    bool GetDetailsupdated();

    // Get the hardware config
    AslShiftX3HardwareConfig GetConfig();

    // Called by the manager, process a notification message from the module
    void Process(CanFrame frame);

    // Register a function to get notified on button state changes
    // button: 0-1, which button was pressed
    // down: true means button down event, false means button up event
    void RegisterButtonCallback(void (*callback)(int button, bool down));

    // Register a function to get notified when the first notification
    // is received from the module.  Put your initialization code
    // here to mak sure it is received by the module
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

    // Custom method to set the bar graph as a shift indicator, directly setting the LEDs
    // value: >= 0, can be outside low and high.  Typically provide RPM as value
    // low: >= 0, if value is less or equal to low, then no lights are on
    // high: > low, lights turn on from none at low to all at high, and colors go from blue to green
    //         to orange to red at high.  all lights on an red for value >= high
    // flashThreshold: > 0, if value is above flashThreshold, all lights flasg at 10Hz
    // reverse: you need to control this, use true for left to right with displayOnTop set in SetConfiguration.
    bool SetCustomLinearGraph(float value, float low, float high, float flashThreshold, bool reverse);
};

#endif