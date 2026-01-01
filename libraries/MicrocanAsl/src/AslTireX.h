#ifndef ASL_TIREX_H
#define ASL_TIREX_H

#include <ESP32-TWAI-CAN.hpp>

enum AlsTireXPosition
{
  FL,
  FR,
  RL,
  RR
};

struct AslTireXPrefs
{
  // valid rates are 0 (0.5Hz), 1, 2, 4, 8, 16, 32, 64Hz
  int SampleRate = 8;
  // valid zones are 1, 2, 4, 8, 16
  // 16 zones is not supported in this code yet.
  int Zones = 8;
};

class AslTireX
{
  private: 
    AslTireXPrefs m_prefs;
    bool m_connected[4] = { false, false, false, false };
    bool SendPrefs(int pos, AslTireXPrefs prefs);

  public:
    // temps by position, only valid up to the number of zones that are 
    // set in the prefs.
    byte Temps[4][16];

    // set prefs at any time, they will be sent to sensors on connection
    void SetPrefs(AslTireXPrefs prefs);
        
    // Called by the manager, process a notification message from the modules
    void Process(CanFrame frame);
};

#endif