#include "gauge.h"

static int displayW, displayH;

Gauge::Gauge(String label, float min, float max)
{
    m_canvas = new M5Canvas(&M5.Lcd);  
    m_label = label;
    m_min = min;
    m_max = max;  
    displayW = M5.Lcd.width();
    displayH = M5.Lcd.height();
}

void Gauge::Update(float value)
{
  m_canvas->createSprite(displayW, displayH);

  m_canvas->fillSprite(TFT_WHITE);

  float min = -30;
  float max = 210;

  for (int i = 0; i <= 16; i++)
  {
    float angle = min + (max - min) / 16 * i;
    float sx = sin(angle * 0.0174532925);
    float cx = cos(angle * 0.0174532925);

    float x0 = cx * displayW / 2;
    float y0 = -sx * displayH / 2;
    float x1 = x0 * 0.9 + displayW / 2;
    float y1 = y0 * 0.9 + displayH / 2;
    x0 += displayW / 2;
    y0 += displayH / 2;
    m_canvas->drawLine(x0, y0, x1, y1, TFT_BLACK);
  }

  for (int i = 0; i <= 4; i++)
  {
    float angle = min + (max - min) / 4 * i;
    float sx = sin(angle * 0.0174532925);
    float cx = cos(angle * 0.0174532925);

    float x0 = cx * displayW / 2 * 0.65 + displayW / 2;
    float y0 = -sx * displayH / 2 *0.65 + displayH / 2;
    float mark = m_max - (m_max - m_min) / 4 * i;
    m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
    m_canvas->setFont(&fonts::DejaVu24);
    m_canvas->setTextDatum(textdatum_t::middle_center);
    m_canvas->drawNumber(mark, x0, y0);


    x0 = cx * displayW / 2 * 0.85 + displayW / 2;
    y0 = -sx * displayH / 2 *0.85 + displayH / 2;
    sx = sin((angle - 4) * 0.0174532925);
    cx = cos((angle - 4) * 0.0174532925);
    float x1 = cx * displayW / 2 + displayW / 2;
    float y1 = -sx * displayH / 2 + displayH / 2;
    sx = sin((angle + 4) * 0.0174532925);
    cx = cos((angle + 4) * 0.0174532925);
    float x2 = cx * displayW / 2 + displayW / 2;
    float y2 = -sx * displayH / 2 + displayH / 2;
    
    m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_BLACK);
  }

  if (value < m_min)
  {
    value = m_min;
  }
  if (value > m_max)
  {
    value = m_max;
  }
  float thick = 20;
  float norm = (value - m_min) / (m_max - m_min);
  float angle = max - (max - min) * norm;
  float sx = sin(angle * 0.0174532925);
  float cx = cos(angle * 0.0174532925);
  float x0 = (1 + 0.9 * cx) * displayW / 2;
  float y0 = (1 - 0.9 * sx) * displayH / 2;
  angle += 90;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  float x1 = thick * cx + displayW / 2;
  float y1 = -thick * sx + displayH / 2;
  angle -= 180;
  sx = sin(angle * 0.0174532925);
  cx = cos(angle * 0.0174532925);
  float x2 = thick * cx + displayW / 2;
  float y2 = -thick * sx + displayH / 2;
  
  m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, TFT_RED);
  m_canvas->fillCircle(displayW / 2, displayH / 2, thick + 2, TFT_BLACK);

  m_canvas->setTextColor(TFT_BLACK, TFT_WHITE);
  m_canvas->setFont(&fonts::DejaVu40);
  m_canvas->setTextDatum(textdatum_t::top_center);
  m_canvas->drawString(m_label, displayW / 2, displayH * 3 / 4);

  m_canvas->pushSprite(0, 0);
  m_canvas->deleteSprite();
}
