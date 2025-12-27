#include "gauge.h"

Gauge::Gauge(String label, String units, float min, float max)
{
    m_canvas = new M5Canvas(&M5.Lcd);  
    m_label = label;
    m_min = min;
    m_max = max;  
    m_units = units;
    m_oldValue = 0;
    m_maxChange = (max - min) * 0.05;
    SetSize(0, 0, M5.Lcd.width(), M5.Lcd.height());
}

void Gauge::SetSize(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;

    m_cx = w / 2;
    m_cy = h / 2;

    m_r = std::min(m_cx, m_cy);

    if (m_r > 100)
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
}

void Gauge::Update(float value)
{
  m_canvas->createSprite(m_w, m_h);
  m_canvas->fillSprite(TFT_BLACK);
  m_canvas->fillCircle(m_cx, m_cy, m_r, TFT_WHITE);

  // angle range for tick marks
  float min = -30;
  float max = 210;

  // minor tick marks
  for (int i = 0; i <= 16; i++)
  {
    float angle = min + (max - min) / 16 * i;
    float sinA = sin(angle * 0.0174532925);
    float cosA = cos(angle * 0.0174532925);

    float x0 = cosA * m_r;
    float y0 = -sinA * m_r;
    float x1 = x0 * 0.9 + m_cx;
    float y1 = y0 * 0.9 + m_cy;
    x0 += m_cx;
    y0 += m_cy;
    m_canvas->drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  // major tick marks as triangles
  for (int i = 0; i <= 4; i++)
  {
    float angle = min + (max - min) / 4 * i;
    float sx = sin(angle * 0.0174532925);
    float cx = cos(angle * 0.0174532925);

    // tick number labels
    float x0 = cx * m_r * 0.65 + m_cx;
    float y0 = -sx * m_r * 0.65 + m_cy;
    float mark = m_max - (m_max - m_min) / 4 * i;
    m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    SetFontSize(m_smallFont);
    m_canvas->setTextDatum(textdatum_t::middle_center);
    m_canvas->drawNumber(mark, x0, y0);
    
    // triangle tick marks
    x0 = cx * m_r * 0.85 + m_cx;
    y0 = -sx * m_r *0.85 + m_cy;
    sx = sin((angle - 4) * 0.0174532925);
    cx = cos((angle - 4) * 0.0174532925);
    float x1 = cx * m_r * 1.02 + m_cx;
    float y1 = -sx * m_r * 1.02 + m_cy;
    sx = sin((angle + 4) * 0.0174532925);
    cx = cos((angle + 4) * 0.0174532925);
    float x2 = cx * m_r * 1.02 + m_cx;
    float y2 = -sx * m_r * 1.02 + m_cy;
    
    m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_BLACK);
  }

  // units text
  m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
  SetFontSize(m_medFont);
  m_canvas->setTextDatum(textdatum_t::middle_center);
  m_canvas->drawString(m_units, m_cx, m_h * 0.35);

  // the gauge needle
  if (value < m_min)
  {
    value = m_min;
  }
  if (value > m_max)
  {
    value = m_max;
  }
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
  m_oldValue = value;

  float thick = m_r / 5;
  float norm = (value - m_min) / (m_max - m_min);
  float angle = max - (max - min) * norm;
  float sx = sin(angle * 0.0174532925);
  float cx = cos(angle * 0.0174532925);
  float x0 = m_cx + 0.9 * cx * m_r;
  float y0 = m_cy - 0.9 * sx * m_r;
  angle += 90;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  float x1 = thick * cx + m_cx;
  float y1 = -thick * sx + m_cy;
  angle -= 180;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  float x2 = thick * cx + m_cx;
  float y2 = -thick * sx + m_cy;
  
  m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
  m_canvas->fillCircle(m_cx, m_cy, thick + 2, TFT_BLACK);

  // label text
  m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
  SetFontSize(m_bigFont);
  m_canvas->setTextDatum(textdatum_t::top_center);
  m_canvas->drawString(m_label, m_cx, m_h * 3 / 4);

  m_canvas->pushSprite(m_x, m_y);
  m_canvas->deleteSprite();
}

void Gauge::SetFontSize(int points)
{
  switch (points)
  {
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
