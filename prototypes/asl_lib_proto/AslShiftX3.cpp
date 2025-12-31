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
    else
    {
        //M5.Lcd.printf("Unknown message ID: %X\n", in.identifier);
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

bool AslShiftX3::SetCustomLinearGraph(float value, float low, float high)
{
    byte r, g, b;
    float norm = (value - low) / (high - low);
    // blue - 0
    // green - 0.5
    // orange - 0.75
    // red - 1.0
    if (norm <= 0.5)
    {
        float miniNorm = norm * 2.0;
        r = 0;
        g = 255.0 * (miniNorm);
        b = 255.0 * (1.0 - miniNorm);
    }
    else if (norm <= 0.75)
    {
        float miniNorm = (norm - 0.5) * 4.0;
        r = 255.0 * miniNorm;
        g = 255.0 * (1.0 - miniNorm) + 165.0 * (miniNorm);
        b = 0;
    }
    else
    {
        float miniNorm = (norm - 0.75) * 4.0;
        r = 255.0;
        g = 165.0 * (1.0 - miniNorm);
        b = 0;
    }

    int full = floor(norm * BarGraphLength);
    float frac = norm * BarGraphLength - full;

    SetLed(0, full, r, g, b, 0);
    SetLed(full, 1, r * frac, g * frac, b * frac, 0);
}

