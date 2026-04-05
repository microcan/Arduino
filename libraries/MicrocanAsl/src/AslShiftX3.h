#ifndef ASL_SHIFTX3_H
#define ASL_SHIFTX3_H

#include <ESP32-TWAI-CAN.hpp>
#include <Microcan.h>

// Struct for the details from the hardware in the Announce message
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

// Class to handle all the messages to and from the ShiftX3 hardware
class AslShiftX3
{
private:
    void (*m_buttonCallback)(int, bool) = nullptr;
    void (*m_connectCallback)() = nullptr;

    WatchedValue *m_alert[2] = {nullptr, nullptr};
    float m_alertMult[2] = { 1.0, 1.0};
    WatchedValue *m_linear = nullptr;
    bool m_linearReverse = false;

    AslShiftX3HardwareConfig m_config;
    bool m_detailsUpdated = false;
    int m_connectCount = 0;

public:
    // Becomes true when any notification is received from the module
    bool GetConnected();

    // Becomes true when the hardware Announce message is received
    bool GetDetailsUpdated();

    // Get the hardware config, only updated from hardware after GetConnected() is true,
    // but don't expect these details to ever change from the defaults above.
    AslShiftX3HardwareConfig GetConfig();

    // Called by the CAN manager, process a notification message from the module
    void Process(CanFrame frame);

    // Register your own function to get called back on button state changes
    // button: 0-1, which button was pressed
    // down: true means button down event, false means button up event
    void RegisterButtonCallback(void (*callback)(int button, bool down));

    // Register a function to get notified when the first notification
    // is received from the module.  Put your initialization code
    // here to make sure it is received by the module, whichmight not be active
    // when your setup runs.
    void RegisterConnectCallback(void (*callback)());

    // Set the gear indicator LED to a number from 0 to 9
    // value: 0-9, gets displayed in the 7 segment 'gear indicator'
    bool SetDisplay(int value);

    // Set the general configuration
    // brightness: 0 = auto, 1 = dim, 100 = brightest, 0 is default
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

    // Set alert thresholds for automatic alert light state by value
    // index: 0-1, the 0 based index of the alert indicator
    // id: 0-4, five thresholds for state change can be set.  first
    //     can be >= 0, others needs to be > 0
    // threshold: >=0, the value at which this threshold comes into effect
    // r, g, b: 0-255, the alert light collor
    // flashHz: 0-10, the flash rate in Hz, 0 for solid
    bool SetAlertThreshold(int index, int id, unsigned int threshold, byte r, byte g, byte b, int flashHz);

    // Set the value to an alert light to set the light automatically by the
    // thresholds set using SetAlertThreshold
    // index: 0-1, the 0 based index of the alert indicator
    // value: >=0, the value to be comapred to the thresholds that have been set
    bool SetAlertValue(int index, unsigned int value);

    // Automatically set Alert thresholds using the ranges specified in a WatchedValue class.  Make this
    // call once in your ConnectCallback and it will set the thresholds based on the details of the watched
    // value, and keep a pointer to the watched value so all you do in your loop is update wtached.Value. 
    // index: 0-1: the alert light to set up.
    // watched: the class with the limits to use for alert thresholds, and the .Value that will be watched in 
    //          the loop.
    void SetAlertWatch(int index, WatchedValue &watched);

    // Like SetAlertWatch, but the light is off until watched.HighNormal, and flashing above HighAlarm.  This
    // is probably what you want for a classic alert light.
    // index: 0-1: the alert light to set up.
    // watched: the class with the limits to use, and the .Value to watch.
    void SetAlertWatchHigh(int index, WatchedValue &watched);

    // Custom method to set the bar graph as a shift indicator, directly setting the LEDs automatically in .Update()
    // based on the limits defined in theWatchedValue class.
    // watch: the WatchedValue class with the current value and thresholds to make everything work.
    // reverse: you need to control this, use true for left to right with displayOnTop set in SetConfiguration.
    void SetLinearGraphWatch(WatchedValue &watch, bool reverse);

    // called by CanManager is you are calling CanManager.Update in your loop.  Does the processing for the AlertWatch and 
    // LinearGraphWatch if you set those.
    void Update();
};

#endif