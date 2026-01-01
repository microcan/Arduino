#ifndef ASL_COMMON_H
#define ASL_COMMON_H

// ShiftX3 CAN message IDs
// https://wiki.autosportlabs.com/ShiftX3#ShiftX3_CAN_bus_API

#define ASL_SHIFTX3_BASE_ID  0xE3600
#define ASL_SHIFTX3_MAX_ID   ASL_SHIFTX3_BASE_ID + 0xFF

// notifications from ShiftX3
#define ASL_SHIFTX3_INF_ANNOUNCE  ASL_SHIFTX3_BASE_ID
#define ASL_SHIFTX3_INF_STATS     ASL_SHIFTX3_BASE_ID + 2
#define ASL_SHIFTX3_INF_BUTTON    ASL_SHIFTX3_BASE_ID + 60

// set messages to send to ShiftX3
#define ASL_SHIFTX3_SET_CONFIG      ASL_SHIFTX3_BASE_ID + 3
#define ASL_SHIFTX3_SET_DISPLAY     ASL_SHIFTX3_BASE_ID + 50
#define ASL_SHIFTX3_SET_LED         ASL_SHIFTX3_BASE_ID + 10
#define ASL_SHIFTX3_SET_ALERT       ASL_SHIFTX3_BASE_ID + 20
#define ASL_SHIFTX3_SET_ALRT_THRSH  ASL_SHIFTX3_BASE_ID + 21
#define ASL_SHIFTX3_SET_ALRT_VAL    ASL_SHIFTX3_BASE_ID + 22

// TireX CAN message IDs
// https://wiki.autosportlabs.com/TireX#CAN_bus_protocol

#define ASL_TIREX_BASE_ID   0xA100
#define ASL_TIREX_MAX_ID    ASL_TIREX_BASE_ID + 0xFFF

// Sensor base IDs by position around the car
#define ASL_TIREX_BASE_FL   ASL_TIREX_BASE_ID
#define ASL_TIREX_BASE_FR   ASL_TIREX_BASE_ID + 0x100
#define ASL_TIREX_BASE_RL   ASL_TIREX_BASE_ID + 0x200
#define ASL_TIREX_BASE_RR   ASL_TIREX_BASE_ID + 0x300

// Notification message offsets from sensor base IDs
#define ASL_TIREX_INF_ANNOUNCE_OFF     0x00
#define ASL_TIREX_INF_STATS_OFF        0x02
#define ASL_TIREX_INF_TEMPS_OFF        0x20

// Set message offsets from base sensor IDs
#define ASL_TIREX_SET_CONFIG_OFF       0x03
#define ASL_TIREX_SET_CALIB_OFF        0x04

#endif