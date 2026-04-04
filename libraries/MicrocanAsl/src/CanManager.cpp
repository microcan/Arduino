#include "CanManager.h"
#include <ESP32-TWAI-CAN.hpp>
#include "AslCommon.h"

// subaru CAN IDs
#define SUB_OIL_COOL 864
#define SUB_RPM_ACC 320
#define ODB_RPM 2024

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

void CanManager::ProcessSubi(CanFrame frame)
{
    if (frame.identifier == SUB_OIL_COOL) 
    {
        // oil temp
        Subaru.oilTemp = frame.data[2] - 40;
        // water temp
        Subaru.waterTemp = frame.data[3] - 40;
    }
    else if (frame.identifier == SUB_RPM_ACC) 
    {
        // rpm
        uint8_t hiByte = frame.data[3] << 3;
        Subaru.rpm = (hiByte * 32) + frame.data[2];
    }
    else
    {
        // todo gear calculation
        Subaru.gear = 0;
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
        else
        {
            ProcessSubi(rxFrame);
        }
    }

    ShiftX.Update();
}