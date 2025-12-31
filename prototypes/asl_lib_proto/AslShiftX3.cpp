#include "AslShiftX3.h"
#include "MicrocanCommon.h"
#include "AslCommon.h"

void AslShiftX3::Process(CanFrame in)
{
    // hardware details
    if (in.identifier == ASL_SHIFTX3_INF_ANNOUNCE)
    {
        if (!DetailsUpdated)
        {
            DetailsUpdated = true;
            LedCount = in.data[0];
            AlertCount = in.data[1];
            BarGraphLength = in.data[2];
            FirmwareMajor = in.data[3];
            FirmwareMinor = in.data[4];
            FirmwarePatch = in.data[5];
        }
                    
        if (!Connected)
        {
            Connected = true;
            if (m_connectCallback != nullptr)
            {
                m_connectCallback();
            }
        }
    }
    // button state change
    else if (in.identifier == ASL_SHIFTX3_INF_BUTTON)
    {
        if (!Connected)
        {
            Connected = true;
            if (m_connectCallback != nullptr)
            {
                m_connectCallback();
            }
        }

        if (m_buttonCallback != nullptr)
        {
            bool down = in.data[0] == 1;
            m_buttonCallback(in.data[1], down);
        }
    }
    // firmware version info
    else if (in.identifier == ASL_SHIFTX3_INF_STATS)
    {
        if (!Connected)
        {
            Connected = true;
            if (m_connectCallback != nullptr)
            {
                m_connectCallback();
            }
        }
    }
}

void AslShiftX3::RegisterButtonCallback(void (*callback)(int, bool))
{
    m_buttonCallback = callback;
}

void AslShiftX3::RegisterConnectCallback(void (*callback)())
{
    m_connectCallback = callback;
    // call the callback immediately if already connected
    if (Connected)
    {
        m_connectCallback();
    }
}

bool AslShiftX3::SetDisplay(int value)
{
    value = limit(value, 0, 9);
    CanFrame txFrame = {0};
    txFrame.identifier = ASL_SHIFTX3_SET_DISPLAY;
    txFrame.extd = 1;
    txFrame.data_length_code = 2;
    txFrame.data[0] = 0;
    txFrame.data[1] = value + 48;

    return ESP32Can.writeFrame(txFrame);
}

bool AslShiftX3::SetConfiguration(int brightness, int autoBrightScaling, bool displayOnTop)
{
    brightness = limit(brightness, 0, 100);
    autoBrightScaling = limit(autoBrightScaling, 0, 255);

    CanFrame txFrame = {0};
    txFrame.identifier = ASL_SHIFTX3_SET_CONFIG;
    txFrame.extd = 1;
    txFrame.data_length_code = 3;
    txFrame.data[0] = brightness;
    txFrame.data[1] = autoBrightScaling;
    if (displayOnTop)
    {
        txFrame.data[2] = 1;
    }
    else 
    {
        txFrame.data[2] = 0;
    }
    return ESP32Can.writeFrame(txFrame);   
}

bool AslShiftX3::SetLed(int startIndex, int count, byte r, byte g, byte b, int flashHz)
{
    startIndex = limit(startIndex, 0, BarGraphLength - 1);
    count = limit(count, 0, BarGraphLength);
    flashHz = limit(flashHz, 0, 10);

    CanFrame txFrame = {0};
    txFrame.identifier = ASL_SHIFTX3_SET_LED;
    txFrame.extd = 1;
    txFrame.data_length_code = 6;
    txFrame.data[0] = startIndex;
    txFrame.data[1] = count;
    txFrame.data[2] = r;
    txFrame.data[3] = g;
    txFrame.data[4] = b;
    txFrame.data[5] = flashHz;

    return ESP32Can.writeFrame(txFrame);   
}

bool AslShiftX3::SetAlert(int index, byte r, byte g, byte b, int flashHz)
{
    index = limit(index, 0, AlertCount - 1);
    flashHz = limit(flashHz, 0, 10);

    CanFrame txFrame = {0};
    txFrame.identifier = ASL_SHIFTX3_SET_ALERT;
    txFrame.extd = 1;
    txFrame.data_length_code = 5;
    txFrame.data[0] = index;
    txFrame.data[1] = r;
    txFrame.data[2] = g;
    txFrame.data[3] = b;
    txFrame.data[4] = flashHz;

    return ESP32Can.writeFrame(txFrame);   
}

bool AslShiftX3::SetAlertThreshold(int index, int id, unsigned int threshold, byte r, byte g, byte b, int flashHz)
{
    index = limit(index, 0, AlertCount - 1);
    id = limit(id, 0, 4);
    flashHz = limit(flashHz, 0, 10);

    CanFrame txFrame = {0};
    txFrame.identifier = ASL_SHIFTX3_SET_ALRT_THRSH;
    txFrame.extd = 1;
    txFrame.data_length_code = 8;
    txFrame.data[0] = index;
    txFrame.data[1] = id;
    txFrame.data[2] = lowByte(threshold);
    txFrame.data[3] = highByte(threshold);
    txFrame.data[4] = r;
    txFrame.data[5] = g;
    txFrame.data[6] = b;
    txFrame.data[7] = flashHz;

    return ESP32Can.writeFrame(txFrame);   
}

bool AslShiftX3::SetAlertValue(int index, unsigned int value)
{
    index = limit(index, 0, AlertCount - 1);

    CanFrame txFrame = {0};
    txFrame.identifier = ASL_SHIFTX3_SET_ALRT_VAL;
    txFrame.extd = 1;
    txFrame.data_length_code = 3;
    txFrame.data[0] = index;
    txFrame.data[1] = lowByte(value);
    txFrame.data[2] = highByte(value);

    return ESP32Can.writeFrame(txFrame);   
}

bool AslShiftX3::SetCustomLinearGraph(float value, float low, float high, float flashThreshold, bool reverse)
{
    byte r, g, b;

    if (high <= low)
    {
        return false;
    }

    // limit, normalize and colorize.
    value = limit(value, low, high);
    float norm = (value - low) / (high - low);
    colorize(norm, r, g, b);

    // should we flash?
    int flashHz;
    if (value < flashThreshold)
    {
        flashHz = 0;
    }
    else
    {
        flashHz = 10;
    }

    // figure outhow many lights should be full on, 
    // what fraction the next one should be on, and
    // how many should be off
    int full = floor(norm * BarGraphLength);
    float frac = norm * BarGraphLength - full;

    int solidStart, partialStart, blankStart;
    if (reverse)
    {
        solidStart = BarGraphLength - full;
        partialStart = solidStart - 1;
        blankStart = 0;
    }
    else
    {
        solidStart = 0;
        partialStart = full;
        blankStart = full + 1;
    }

    // set the lights
    bool result = true;
    if (full > 0)
    {
        result = SetLed(solidStart, full, r, g, b, flashHz);
    }
    if (result && (full < BarGraphLength))
    {
        result = SetLed(partialStart, 1, r * frac, g * frac, b * frac, flashHz);
    }
    if (result && (full < (BarGraphLength - 1)))
    {
        result = SetLed(blankStart, BarGraphLength - full - 1, 0, 0, 0, 0);
    }
    return result;
}

