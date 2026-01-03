#include "AslShiftX3.h"
#include "AslCommon.h"

void AslShiftX3::Process(CanFrame in)
{
    // hardware details
    if (in.identifier == ASL_SHIFTX3_INF_ANNOUNCE)
    {
        if (!m_detailsUpdated)
        {
            m_detailsUpdated = true;
            m_config.LedCount = in.data[0];
            m_config.AlertCount = in.data[1];
            m_config.BarGraphLength = in.data[2];
            m_config.FirmwareMajor = in.data[3];
            m_config.FirmwareMinor = in.data[4];
            m_config.FirmwarePatch = in.data[5];
        }

        if (!m_connected)
        {
            m_connected = true;
            if (m_connectCallback != nullptr)
            {
                m_connectCallback();
            }
        }
    }
    // button state change
    else if (in.identifier == ASL_SHIFTX3_INF_BUTTON)
    {
        if (!m_connected)
        {
            m_connected = true;
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
        if (!m_connected)
        {
            m_connected = true;
            if (m_connectCallback != nullptr)
            {
                m_connectCallback();
            }
        }
    }
}

bool AslShiftX3::GetConnected()
{
    return m_connected;
}

bool AslShiftX3::GetDetailsupdated()
{
    return m_detailsUpdated;
}

AslShiftX3HardwareConfig AslShiftX3::GetConfig()
{
    return m_config;
}

void AslShiftX3::RegisterButtonCallback(void (*callback)(int, bool))
{
    m_buttonCallback = callback;
}

void AslShiftX3::RegisterConnectCallback(void (*callback)())
{
    m_connectCallback = callback;
    // call the callback immediately if already connected
    if (m_connected)
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
    startIndex = limit(startIndex, 0, m_config.BarGraphLength - 1);
    count = limit(count, 0, m_config.BarGraphLength);
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
    index = limit(index, 0, m_config.AlertCount - 1);
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
    index = limit(index, 0, m_config.AlertCount - 1);
    id = limit(id, 0, 4);
    flashHz = limit(flashHz, 0, 10);

    if (threshold < id)
    {
        threshold = id;
    }

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
    if (!m_connected)
    {
        return false;
    }
    index = limit(index, 0, m_config.AlertCount - 1);

    CanFrame txFrame = {0};
    txFrame.identifier = ASL_SHIFTX3_SET_ALRT_VAL;
    txFrame.extd = 1;
    txFrame.data_length_code = 3;
    txFrame.data[0] = index;
    txFrame.data[1] = lowByte(value);
    txFrame.data[2] = highByte(value);

    return ESP32Can.writeFrame(txFrame);
}

void AslShiftX3::SetAlertWatch(int index, WatchedValue &watched)
{
    index = limit(index, 0, 1);

    m_alert[index] = &watched;
    // put floats into a good int range
    if (m_alert[index]->Range() <= 1)
    {
        m_alertMult[index] = 1000;
    }
    else if (m_alert[index]->Range() < 10)
    {
        m_alertMult[index] = 100;
    }
    else if (m_alert[index]->Range() < 100)
    {
        m_alertMult[index] = 10;
    }

    byte r, g, b;

    watched.ColorForValue(watched.Low, r, g, b);
    SetAlertThreshold(index, 0, watched.Low * m_alertMult[index], r, g, b, 10);

    watched.ColorForValue(watched.LowAlarm, r, g, b);
    SetAlertThreshold(index, 1, watched.LowAlarm * m_alertMult[index], r, g, b, 0);

    watched.ColorForValue(watched.LowNormal, r, g, b);
    SetAlertThreshold(index, 2, watched.LowNormal * m_alertMult[index], r, g, b, 0);

    watched.ColorForValue(watched.HighNormal, r, g, b);
    SetAlertThreshold(index, 3, watched.HighNormal * m_alertMult[index], r, g, b, 0);

    watched.ColorForValue(watched.High, r, g, b);
    SetAlertThreshold(index, 4, watched.HighAlarm * m_alertMult[index], r, g, b, 10);
}

void AslShiftX3::SetAlertWatchHigh(int index, WatchedValue &watched)
{
    index = limit(index, 0, 1);

    m_alert[index] = &watched;
    // put floats into a good int range
    if (m_alert[index]->Range() <= 1)
    {
        m_alertMult[index] = 1000;
    }
    else if (m_alert[index]->Range() < 10)
    {
        m_alertMult[index] = 100;
    }
    else if (m_alert[index]->Range() < 100)
    {
        m_alertMult[index] = 10;
    }

    byte r, g, b;

    watched.ColorForValue(watched.HighNormal, r, g, b);
    SetAlertThreshold(index, 0, watched.HighNormal * m_alertMult[index], r, g, b, 0);

    watched.ColorForValue(watched.High, r, g, b);
    SetAlertThreshold(index, 1, watched.HighAlarm * m_alertMult[index], r, g, b, 10);
}

void AslShiftX3::SetLinearGraphWatch(WatchedValue &watch, bool reverse)
{
    m_linear = &watch;
    m_linearReverse = reverse;
}

void AslShiftX3::Update()
{
    for (int i = 0; i < 2; i++)
    {
        if (m_alert[i] != nullptr)
        {
            SetAlertValue(i, m_alert[i]->Value * m_alertMult[i]);
        }
    }

    if (m_linear != nullptr)
    {
        byte r, g, b;

        m_linear->Color(r, g, b);

        // should we flash?
        int flashHz;

        if (m_linear->HighAlarmActive())
        {
            flashHz = 10;
        }
        else
        {
            flashHz = 0;
        }

        // figure outhow many lights should be full on,
        // what fraction the next one should be on, and
        // how many should be off
        float norm = m_linear->Normalized();
        int full = floor(norm * m_config.BarGraphLength);
        float frac = norm * m_config.BarGraphLength - full;

        int solidStart, partialStart, blankStart;
        if (m_linearReverse)
        {
            solidStart = m_config.BarGraphLength - full;
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
        if (result && (full < m_config.BarGraphLength))
        {
            result = SetLed(partialStart, 1, r * frac, g * frac, b * frac, flashHz);
        }
        if (result && (full < (m_config.BarGraphLength - 1)))
        {
            result = SetLed(blankStart, m_config.BarGraphLength - full - 1, 0, 0, 0, 0);
        }
    }
}
