#include "CanManager.h"
#include <ESP32-TWAI-CAN.hpp>
#include "AslCommon.h"

bool CanManager::Connect(int txPin, int rxPin)
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

void CanManager::Update()
{
    CanFrame rxFrame;

    while (ESP32Can.readFrame(rxFrame, 0))
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

    ShiftX.Update();
}