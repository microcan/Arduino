#include "AnalogGauge.h"

AnalogGauge::AnalogGauge(String label, String units, WatchedValue &watched)
{
  m_canvas = new M5Canvas(&M5.Lcd);
  m_watched = &watched;
  m_units = units;
  m_label = label;
  m_oldValue = 0;
  m_maxChange = 0.05;
  SetSize(0, 0, M5.Lcd.width(), M5.Lcd.height());
}

void AnalogGauge::SetSize(int x, int y, int w, int h)
{
  m_x = x;
  m_y = y;
  m_w = w;
  m_h = h;

  m_cx = w / 2;
  m_cy = h / 2;

  m_r = std::min(m_cx, m_cy);

  if (m_r > 200)
  {
    m_bigFont = 72;
    m_medFont = 40;
    m_smallFont = 24;
  }
  else if (m_r > 100)
  {
    m_bigFont = 40;
    m_medFont = 24;
    m_smallFont = 18;
  }
  else if (m_r > 50)
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

void AnalogGauge::Update()
{
  m_canvas->createSprite(m_w, m_h);
  if (m_first)
  {
    m_canvas->fillSprite(TFT_BLACK);
    m_canvas->fillCircle(m_cx, m_cy, m_r, M5.Display.color565(120, 120, 120));
    m_canvas->fillCircle(m_cx, m_cy, m_r - 4, TFT_WHITE);
  }
  else
  {
    m_canvas->fillSprite(TFT_TRANSPARENT);
  }

  // angle range for tick marks
  float min = -30;
  float max = 210;

  // minor tick marks
  if (m_first)
  {
    for (int i = 0; i <= 16; i++)
    {
      float angle = min + (max - min) / 16 * i;
      float sinA = sin(angle * 0.0174532925);
      float cosA = cos(angle * 0.0174532925);

      float x0 = cosA * m_r;
      float y0 = -sinA * m_r;
      float x1 = x0 * 0.94 + m_cx;
      float y1 = y0 * 0.94 + m_cy;
      x0 += m_cx;
      y0 += m_cy;
      m_canvas->drawLine(x0, y0, x1, y1, TFT_BLACK);
    }
  }

  // clear old needle display
  float thick = m_r / 4.9;
  float norm = m_oldValue;
  float angle = max - (max - min) * norm - 4;
  float sx = sin(angle * 0.0174532925);
  float cx = cos(angle * 0.0174532925);
  float x0 = m_cx + 0.9 * cx * m_r;
  float y0 = m_cy - 0.9 * sx * m_r;
  angle += 93;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  float x1 = thick * cx + m_cx;
  float y1 = -thick * sx + m_cy;
  angle -= 180;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  float x2 = thick * cx + m_cx;
  float y2 = -thick * sx + m_cy;
  angle = max - (max - min) * norm + 3;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  float x4 = m_cx + 0.9 * cx * m_r;
  float y4 = m_cy - 0.9 * sx * m_r;

  m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_WHITE);
  m_canvas->fillTriangle(x0, y0, x4, y4, x1, y1, TFT_WHITE);

  // major tick marks as rectangles
  for (int i = 0; i <= 4; i++)
  {
    float angle = min + (max - min) / 4 * i;
    float sx = sin(angle * 0.0174532925);
    float cx = cos(angle * 0.0174532925);

    // tick number labels
    float x0 = cx * m_r * 0.65 + m_cx;
    float y0 = -sx * m_r * 0.65 + m_cy;
    float mark = m_watched->High - m_watched->Range() / 4 * i;
    m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    SetFontSize(m_smallFont);
    m_canvas->setTextDatum(textdatum_t::middle_center);
    m_canvas->drawNumber(mark, x0, y0);

    if (m_first)
    {
      // triangle tick marks
      sx = sin((angle - 1.5) * 0.0174532925);
      cx = cos((angle - 1.5) * 0.0174532925);
      x0 = cx * m_r * 0.92 + m_cx;
      y0 = -sx * m_r * 0.92 + m_cy;
      float x1 = cx * (m_r - 3) + m_cx;
      float y1 = -sx * (m_r - 3) + m_cy;
      sx = sin((angle + 1.5) * 0.0174532925);
      cx = cos((angle + 1.5) * 0.0174532925);
      float x2 = cx * (m_r - 3) + m_cx;
      float y2 = -sx * (m_r - 3) + m_cy;
      float x3 = cx * m_r * 0.92 + m_cx;
      float y3 = -sx * m_r * 0.92 + m_cy;

      m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, M5.Display.color565(80, 80, 80));
      m_canvas->fillTriangle(x3, y3, x0, y0, x2, y2, M5.Display.color565(80, 80, 80));
    }
  }

  // units text
  m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
  SetFontSize(m_medFont);
  m_canvas->setTextDatum(textdatum_t::middle_center);
  m_canvas->drawString(m_units, m_cx, m_cy - m_r * 0.35);

  // draw the needle
  float value = m_watched->Normalized();
  if (m_oldValue < value)
  {
    if (value - m_oldValue > m_maxChange)
    {
      value = m_oldValue + m_maxChange;
    }
  }
  else if (m_oldValue - value > m_maxChange)
  {
    value = m_oldValue - m_maxChange;
  }

  thick = m_r / 5;
  norm = value;
  angle = max - (max - min) * norm - 1;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  x0 = m_cx + 0.89 * cx * m_r;
  y0 = m_cy - 0.89 * sx * m_r;
  angle += 91;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  x1 = thick * cx + m_cx;
  y1 = -thick * sx + m_cy;
  angle -= 180;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  x2 = thick * cx + m_cx;
  y2 = -thick * sx + m_cy;
  angle = max - (max - min) * norm + 1;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  x4 = m_cx + 0.89 * cx * m_r;
  y4 = m_cy - 0.89 * sx * m_r;

  byte r, g, b;
  m_watched->Color(r, g, b);
  m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, M5.Display.color565(r, b, g));
  m_canvas->fillTriangle(x0, y0, x4, y4, x1, y1, M5.Display.color565(r, b, g));
  m_canvas->fillCircle(m_cx, m_cy, thick + 2, TFT_BLACK);

  m_oldValue = value;

  if (m_first)
  {
    // label text
    m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    SetFontSize(m_bigFont);
    m_canvas->setTextDatum(textdatum_t::top_center);
    m_canvas->drawString(m_label, m_cx, m_cy + m_r / 2);
  }

  m_canvas->pushSprite(m_x, m_y, TFT_TRANSPARENT);
  m_canvas->deleteSprite();
  m_first = false;
}

void AnalogGauge::SetFontSize(int points)
{
  switch (points)
  {
  case 72:
    m_canvas->setFont(&fonts::DejaVu72);
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
