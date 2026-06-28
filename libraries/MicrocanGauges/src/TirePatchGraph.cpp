#include "TirePatchGraph.h"

TirePatchGraph::TirePatchGraph(int zones, byte *data, bool flip)
{
  m_canvas = new M5Canvas(&M5.Lcd);
  m_x = 0;
  m_y = 0;
  m_w = 100;
  m_h = 100;
  m_zones = 4;
  if (zones == 1 || zones == 2 || zones == 4 || zones == 8 || zones == 16)
  {
    m_zones = zones;
  }
  m_data = data;
  m_flip = flip;
  m_watched = new WatchedValue("Tire", "2 x C", 30, 70);
};

void TirePatchGraph::SetSize(int x, int y, int w, int h)
{
  m_x = x;
  m_y = y;
  m_w = w;
  m_h = h;
}

void TirePatchGraph::SetPrefs(int minT, int maxT)
{
  delete m_watched;
  m_watched = new WatchedValue("Tire", "2 x C", minT * 2, maxT * 2);
}

void TirePatchGraph::Update()
{
  byte r, g, b;

  m_canvas->createSprite(m_w, m_h);

  int w = m_w / m_zones;
  float temp = 0;
  for (int i = 0; i < m_zones; i++)
  {
    int index = i;
    if (m_flip)
    {
      index = m_zones - 1 - i;
    }
    m_watched->ColorForValue(m_data[index], r, g, b);
    m_canvas->fillRect(w * i, 0, w, m_h, M5.Display.color565(r, g, b));
    temp += m_data[index];
  }

  temp = temp / m_zones / 2.0;
  m_watched->ColorForValue(temp * 2.0, r, g, b);

  // Value text
  m_canvas->setFont(&fonts::Font8);
  m_canvas->setTextSize(2);
  m_canvas->setTextColor(M5.Display.color565(255-r, 255-g, 255-b));
  m_canvas->setTextDatum(textdatum_t::middle_center);
  m_canvas->drawFloat(temp, 0, m_w / 2, m_h / 2);

  m_canvas->pushSprite(m_x, m_y);
  m_canvas->deleteSprite();
};
