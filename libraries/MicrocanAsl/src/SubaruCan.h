#ifndef SUBARU_CAN_H
#define SUBARU_CAN_H

#include <ESP32-TWAI-CAN.hpp>

// subaru CAN IDs
#define SUB_STEER        24
#define SUB_SPEED       209  // brake pedal and speed
#define SUB_WHEEL       212  // wheel speed
#define SUB_RPM_ACC     320  // and clutch
#define SUB_OIL_COOL    864
#define ODB_RPM        2024

struct SubiValues
{
    float rpm;
    float oilTemp;
    float waterTemp;
    bool clutch;
    float speed;
    int gear;
};

bool SubaruPassFilter(unsigned long id, int minTimeMs);

void SubaruProcessFrame(CanFrame frame, SubiValues &values);

#endif