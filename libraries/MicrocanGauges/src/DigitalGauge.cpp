#include "DigitalGauge.h"


#define LEFT 210.0
#define RIGHT -30.0
#define SWEEP (LEFT - RIGHT)
#define DEG2RAD 0.0174532925199
#define SML_TICK_COUNT 16
#define SML_TICK_INNER 0.94
#define SML_TICK_OUTER 1.00
#define LRG_TICK_COUNT 4
#define LRG_TICK_INNER 0.92
#define LRG_TICK_OUTER 0.99
#define LRG_TICK_THICK 1.5
#define NDL_LNTH 0.89
#define NDL_BASE 0.2
#define NDL_TIP 1.0
#define TICK_LBL_RAD 0.75
#define NAME_Y 0.6
#define VALUE_Y 0.3
#define UNIT_Y 0.35
#define MAX_VAL_LNG 0.55
#define MAX_VAL_SIZ 0.05

DigitalGauge::DigitalGauge(WatchedValue &watched)
{
  m_canvas = new M5Canvas(&M5.Lcd);
  m_watched = &watched;
  m_units = m_watched->Units;
  m_label = m_watched->Name;
  m_oldValue = m_watched->Low - 0.02;
  m_maxVal = m_watched->Low;
  m_maxChange = 0.01;
  m_dialColor = TFT_BLACK;
  m_txtColor = M5.Display.color565(255, 255, 255);
  SetSize(0, 0, M5.Lcd.width(), M5.Lcd.height());
}

void DigitalGauge::SetSize(int x, int y, int w, int h)
{
  m_x = x;
  m_y = y;
  m_w = w;
  m_h = h;

  m_cx = w / 2;
  m_cy = h / 2;

  m_size = std::min(m_w, m_h) / 2;

  if (m_size > 200)
  {
    m_bigFont = 72;
    m_medFont = 40;
    m_smallFont = 24;
  }
  else if (m_size > 130)
  {
    m_bigFont = 72;
    m_medFont = 56;
    m_smallFont = 40;
  }
  else if (m_size > 50)
  {
    m_bigFont = 24;
    m_medFont = 18;
    m_smallFont = 12;
  }
  else
  {
    m_bigFont = 18;
    m_medFont = 12;
    m_smallFont = 12;
  }
  m_first = true;
}

void DigitalGauge::Update()
{
  if (m_first)
  {
    DrawStatic();
    m_first = false;
  }
  DrawDynamic();
}

void DigitalGauge::DrawStatic()
{
  float angle, sinA, cosA, x0, y0, x1, y1, x2, y2, x3, y3;

  m_canvas->createSprite(m_w, m_h);

  m_canvas->fillSprite(TFT_BLACK);
  m_canvas->fillRect(m_cx - m_size + 2, m_cy - m_size + 2, 2 * m_size - 4, 2 * m_size - 4, m_txtColor);

  // background color
  m_canvas->fillRect(m_cx - m_size + 6, m_cy - m_size + 6, 2 * m_size - 12, 2 * m_size - 12, m_dialColor);

  // name text
  SetFontSize(m_medFont);
  m_canvas->setTextDatum(textdatum_t::top_left);
  m_canvas->drawString(m_label, m_cx - m_size + 12, m_cy - m_size + 12);

  // units text
  SetFontSize(m_smallFont);
  m_canvas->setTextDatum(textdatum_t::bottom_left);
  m_canvas->drawString(m_units, m_cx - m_size + 12, m_cy + m_size - 12);

  m_canvas->pushSprite(m_x, m_y);
  m_canvas->deleteSprite();
}

void DigitalGauge::DrawDynamic()
{
    // draw the needle
  float value = m_watched->Normalized();
  float change = std::abs(m_oldValue - value);
  if (change < m_maxChange)
  {
    return;
  }

  int dec = 0;
  if (m_watched->High < 1.05) dec = 2;
  else if (m_watched->High < 10.5) dec = 1;

  m_oldValue = value;

  m_canvas->createSprite(m_w, m_h);
  m_canvas->fillSprite(TFT_TRANSPARENT);

  bool maxChanged = false;
  if (m_watched->Value > m_maxVal)
  {
    m_maxVal = m_watched->Value;
    maxChanged = true;
  }

  byte r, g, b;

  m_watched->Color(r, g, b);

  uint16_t fillColor = M5.Display.color565(r, g, b);
  m_txtColor = M5.Display.color565(255-r, 255-g, 255-b);
  m_canvas->setTextColor(m_txtColor);

  // background color
  float width = (2 * m_size - 16) * value;
  m_canvas->fillRect(m_cx - m_size + 8, m_cy - m_size * 0.5, 2 * m_size - 16, m_size, m_dialColor);
  m_canvas->fillRect(m_cx - m_size + 8, m_cy - m_size * 0.5, width, m_size, fillColor);

  // Value text
  SetFontSize(m_bigFont);
  m_canvas->setTextSize(1.5);
  m_canvas->setTextDatum(textdatum_t::middle_center);
  m_canvas->drawFloat(m_watched->Value, dec, m_cx, m_cy);

  // max value
  if (maxChanged)
  {
    SetFontSize(m_smallFont);
    m_canvas->setTextSize(1.0);
    m_canvas->fillRect(m_cx, m_cy + m_size * 0.5 + 18, m_size - 8, m_size * 0.5 - 26, m_dialColor);
    m_canvas->fillRect(m_cx, m_cy + m_size * 0.5 + 18, width / 2, m_size * 0.5 - 26, fillColor);
    m_canvas->setTextDatum(textdatum_t::bottom_right);
    m_canvas->drawFloat(m_maxVal, dec, m_cx + m_size - 12, m_cy + m_size - 12);
  }

  m_canvas->pushSprite(m_x, m_y, TFT_TRANSPARENT);
  m_canvas->deleteSprite();
}

void DigitalGauge::SetFontSize(int points)
{
  switch (points)
  {
  case 72:
    m_canvas->setFont(&fonts::DejaVu72);
    break;
  case 56:
    m_canvas->setFont(&fonts::DejaVu56);
    break;  
  case 40:
    m_canvas->setFont(&fonts::DejaVu40);
    break;
  case 24:
    m_canvas->setFont(&fonts::DejaVu24);
    break;
  case 18:
    m_canvas->setFont(&fonts::DejaVu18);
    break;
  default:
    m_canvas->setFont(&fonts::DejaVu12);
    break;
  }
}
