#ifndef ASL_MANAGER_H
#define ASL_MANAGER_H

#include <ESP32-TWAI-CAN.hpp>
#include "AslShiftX3.h"
#include "AslTireX.h"


class AslManager
{
    private:
    bool m_connected = false;

    public:
    AslShiftX3 ShiftX;
    AslTireX TireX;
    bool Connect(int txPin, int rxPin);
    void Update();
};

#endif