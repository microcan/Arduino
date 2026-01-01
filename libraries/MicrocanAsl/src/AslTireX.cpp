#include "AslCommon.h"
#include "AslTireX.h"

// convert a sensor ID into a tire position
bool id2Pos(unsigned int id, int& pos)
{
  int test = highByte(id - ASL_TIREX_BASE_ID);
  if (test >=0 && test < 4)
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

void AslTireX::SetPrefs(AslTireXPrefs prefs)
{
  m_prefs = prefs;
  for (int i = 0; i < 4; i++)
  {
    if (m_connected[i])
    {
      SendPrefs(i, prefs);
    }
  }
}
        
void AslTireX::Process(CanFrame frame)
{
  if (frame.identifier > 0xFFFF)
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
    SendPrefs(pos, m_prefs);
  }

  int offset = lowByte(id);
  if (offset == ASL_TIREX_INF_TEMPS_OFF)
  {
    memcpy(Temps[pos], &frame.data[0], 8);
  }
}

bool AslTireX::SendPrefs(int pos, AslTireXPrefs prefs)
{
  unsigned int sensorId = pos2Id(pos);

  CanFrame txFrame = {0};
  txFrame.identifier = sensorId + ASL_TIREX_SET_CONFIG_OFF;
  txFrame.extd = 1;
  txFrame.data_length_code = 4;
  txFrame.data[0] = prefs.SampleRate;
  txFrame.data[1] = prefs.Zones;
  txFrame.data[2] = pos;
  txFrame.data[3] = 0;

  return ESP32Can.writeFrame(txFrame);
}