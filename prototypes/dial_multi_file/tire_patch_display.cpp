#include "tire_patch_display.h"

TirePatchDisplayer::TirePatchDisplayer(int x, int y, int w, int h, int tire, SubiDataManager* manager)
{
    m_canvas = new M5Canvas(&M5.Lcd);
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_tire = tire;
    m_manager = manager;
};

uint16_t ColorFromTemp(int temp)
{
  int red, green;
  if (temp > 255)
  {
    temp = 255;
  }
  if (temp < 0)
  {
    temp = 0;
  }
  green = 255 - temp;
  red = temp;
  return M5.Display.color565(red, green, 0); 
}

void TirePatchDisplayer::Update()
{
    m_canvas->createSprite(m_w, m_h);
    m_canvas->fillRect(0, 0, m_w/4 + 1, m_h, ColorFromTemp(m_manager->GetTireData(m_tire)[0]));//fill sprite with color XXX
    m_canvas->fillRect(m_w/4, 0, m_w/4 + 1, m_h, ColorFromTemp(m_manager->GetTireData(m_tire)[1]));//fill sprite with color XXX
    m_canvas->fillRect(m_w/2, 0, m_w/4 + 1, m_h, ColorFromTemp(m_manager->GetTireData(m_tire)[2]));//fill sprite with color XXX
    m_canvas->fillRect(3*m_w/4, 0, m_w/4 + 1, m_h, ColorFromTemp(m_manager->GetTireData(m_tire)[3]));//fill sprite with color XXX

    m_canvas->pushSprite(m_x, m_y);
    m_canvas->deleteSprite();
};
