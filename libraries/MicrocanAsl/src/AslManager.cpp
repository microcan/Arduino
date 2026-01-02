#include "AslManager.h"
#include "AslCommon.h"

bool AslManager::Connect(int txPin, int rxPin)
{
    if (ESP32Can.begin(ESP32Can.convertSpeed(1000), txPin, rxPin))
    {
        m_connected = true;
        return true;
    }
    else
    {
        m_connected = false;
        return false;
    }
}

void AslManager::Update()
{
    CanFrame rxFrame;
    while (ESP32Can.readFrame(rxFrame, 50))
    {
        if (rxFrame.identifier >= ASL_SHIFTX3_BASE_ID && rxFrame.identifier <= ASL_SHIFTX3_MAX_ID)
        {
            ShiftX.Process(rxFrame);
        }
        else if (rxFrame.identifier >= ASL_TIREX_BASE_ID && rxFrame.identifier <= ASL_TIREX_MAX_ID)
        {
            TireX.Process(rxFrame);
        }
    }
}