#ifndef ASL_TIREX_H
#define ASL_TIREX_H

#include <ESP32-TWAI-CAN.hpp>

// Convenience enums for the ASL sensor position indexes.
enum AlsTireXPosition
{
  FL,
  FR,
  RL,
  RR
};

// convienience strings for tire positions
String AlsTireLabel(int pos);

// Class that handles the tire temp messages, and lets you set preferences
class AslTireX
{
private:
  bool m_connected[4] = {false, false, false, false};
  int m_zones = 8;
  int m_freq = 4;
  bool SendPrefs(int pos);

public:
  // temps by position, only valid up to the number of zones that are
  // set in the prefs.  Read this array to getthe current temps in
  // the TireX raw bytes, which are (2 * deg C)
  byte Temps[4][16];

  // set prefs at any time, ideally before CAN is connected, they will be sent to sensors on connection
  // valid zones are 1, 2, 4, 8, 16 - 16 zones is not supported in this code yet.
  // valid rates are 0 (0.5Hz), 1, 2, 4, 8, 16, 32, 64Hz
  void SetPrefs(int zones, int sampleRate);

  // Called by the manager, process a notification message from the modules
  void Process(CanFrame frame);
};

#endif