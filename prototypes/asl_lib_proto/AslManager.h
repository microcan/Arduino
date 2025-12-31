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

    // access ShiftX3 features as a property
    AslShiftX3 ShiftX;

    // access TireX features as a property
    AslTireX TireX;

    // connect to the CAN where the ASL modules are to be found
    bool Connect(int txPin, int rxPin);

    // read and process any CAN messages from ASL modules
    void Update();
};

#endif