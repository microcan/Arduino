#include "AslCommon.h"
#include "AslTireX.h"

String AlsTireLabel(int pos)
{
  if (pos == 0) return "FL";
  if (pos == 1) return "FR";
  if (pos == 2) return "RL";
  if (pos == 3) return "RR";
  return "NA";
}

// convert a sensor ID into a tire position
bool id2Pos(unsigned int id, int &pos)
{
  int test = highByte(id - ASL_TIREX_BASE_ID);
  if (test >= 0 && test < 4)
  {
    pos = test;
    return true;
  }
  return false;
}

// convert a tire position into a sensor ID
unsigned int pos2Id(int pos)
{
  unsigned int result = ASL_TIREX_BASE_ID + (pos << 8);
  return result;
}

void AslTireX::SetPrefs(int zones, int sampleRate)
{
  m_zones = zones;
  m_freq = sampleRate;
  for (int i = 0; i < 4; i++)
  {
    if (m_connected[i])
    {
      SendPrefs(i);
    }
  }
}

void AslTireX::Process(CanFrame frame)
{
  if (frame.identifier < ASL_TIREX_BASE_ID || frame.identifier > ASL_TIREX_MAX_ID)
  {
    return;
  }
  
  unsigned int id = frame.identifier;
  int pos;
  if (!id2Pos(id, pos))
  {
    return;
  }

  if (!m_connected[pos])
  {
    m_connected[pos] = true;
    SendPrefs(pos);
  }

  int offset = lowByte(id);
  if (offset == ASL_TIREX_INF_TEMPS_OFF)
  {
    memcpy(Temps[pos], &frame.data[0], 8);
  }
}

bool AslTireX::SendPrefs(int pos)
{
  unsigned int sensorId = pos2Id(pos);

  CanFrame txFrame = {0};
  txFrame.identifier = sensorId + ASL_TIREX_SET_CONFIG_OFF;
  txFrame.extd = 1;
  txFrame.data_length_code = 4;
  txFrame.data[0] = m_freq;
  txFrame.data[1] = m_zones;
  txFrame.data[2] = pos;
  txFrame.data[3] = 0;

  return ESP32Can.writeFrame(txFrame);
}