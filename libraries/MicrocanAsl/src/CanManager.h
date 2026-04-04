#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include "AslShiftX3.h"
#include "AslTireX.h"

struct SubiValues
{
    float rpm;
    float oilTemp;
    float waterTemp;
    int gear;
};

// wrapper to manage the CAN connection toboth TireX and ShiftX3
class CanManager
{
private:
    bool m_connected = false;
    void ProcessSubi(CanFrame frame);
public:
    // access ShiftX3 features as a property
    AslShiftX3 ShiftX;

    // access TireX features as a property
    AslTireX TireX;

    // access updated subi properties
    SubiValues Subaru;

    // connect to the CAN where the ASL modules are to be found.  Call this in your setup.
    bool Connect(int txPin, int rxPin);

    // read and process any CAN messages from ASL modules.  Call this in your loop.  This
    // also does any required updates for TireX and ShiftX
    void Update();
};

#endif