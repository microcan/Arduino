#include "AnalogGauge.h"


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
#define NAME_Y 0.5
#define UNIT_Y 0.35
#define MAX_VAL_LNG 0.55
#define MAX_VAL_SIZ 0.05

AnalogGauge::AnalogGauge(WatchedValue &watched)
{
  m_canvas = new M5Canvas(&M5.Lcd);
  m_watched = &watched;
  m_units = m_watched->Units;
  m_label = m_watched->Name;
  m_oldValue = 0;
  m_oldMaxVal = 0;
  m_maxVal = 0;
  m_maxChange = 0.05;
  m_dialColor = TFT_BLACK;
  m_txtColor = M5.Display.color565(120, 120, 120);
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
  else if (m_r > 130)
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
  if (m_first)
  {
    DrawStatic();
    m_first = false;
  }
  DrawDynamic();
}

void AnalogGauge::DrawStatic()
{
  float angle, sinA, cosA, x0, y0, x1, y1, x2, y2, x3, y3;

  m_canvas->createSprite(m_w, m_h);

  m_canvas->fillSprite(TFT_BLACK);
  m_canvas->fillCircle(m_cx, m_cy, m_r, m_txtColor);
  m_canvas->fillCircle(m_cx, m_cy, m_r - 6, m_dialColor);

  // minor tick marks
  for (int i = 0; i <= SML_TICK_COUNT; i++)
  {
    angle = RIGHT + SWEEP / SML_TICK_COUNT * i;
    sinA = sin(angle * DEG2RAD);
    cosA = cos(angle * DEG2RAD);

    x0 = cosA * m_r * SML_TICK_OUTER + m_cx;
    y0 = -sinA * m_r * SML_TICK_OUTER + m_cy;
    x1 = cosA * m_r * SML_TICK_INNER + m_cx;
    y1 = -sinA * m_r * SML_TICK_INNER + m_cy;

    m_canvas->drawLine(x0, y0, x1, y1, m_txtColor);
  }

  // major tick marks as rectangles
  for (int i = 0; i <= LRG_TICK_COUNT; i++)
  {
    angle = RIGHT + SWEEP / LRG_TICK_COUNT * i;
    sinA = sin((angle - LRG_TICK_THICK) * DEG2RAD);
    cosA = cos((angle - LRG_TICK_THICK) * DEG2RAD);
    x0 = cosA * m_r * LRG_TICK_INNER + m_cx;
    y0 = -sinA * m_r * LRG_TICK_INNER + m_cy;
    x1 = cosA * m_r * LRG_TICK_OUTER + m_cx;
    y1 = -sinA * m_r * LRG_TICK_OUTER + m_cy;

    sinA = sin((angle + LRG_TICK_THICK) * DEG2RAD);
    cosA = cos((angle + LRG_TICK_THICK) * DEG2RAD);
    x2 = cosA * m_r * LRG_TICK_OUTER + m_cx;
    y2 = -sinA * m_r * LRG_TICK_OUTER + m_cy;
    x3 = cosA * m_r * LRG_TICK_INNER + m_cx;
    y3 = -sinA * m_r * LRG_TICK_INNER + m_cy;

    m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, m_txtColor);
    m_canvas->fillTriangle(x3, y3, x0, y0, x2, y2, m_txtColor);
  }

  m_canvas->setTextColor(m_txtColor);
  SetFontSize(m_bigFont);
  m_canvas->setTextDatum(textdatum_t::top_center);
  m_canvas->drawString(m_label, m_cx, m_cy + m_r * NAME_Y);
  m_canvas->pushSprite(m_x, m_y);
  m_canvas->deleteSprite();
}

void AnalogGauge::DrawNeedle(float normPos, float length, float baseThick, float tipThick, uint16_t color)
{
  float thick = baseThick * m_r;
  float angle = LEFT - SWEEP * normPos - tipThick;
  float sx = sin(angle * DEG2RAD);
  float cx = cos(angle * DEG2RAD);
  float x0 = m_cx + length * cx * m_r;
  float y0 = m_cy - length * sx * m_r;
  angle += 90 + tipThick;
  sx = sin(angle * DEG2RAD);
  cx = cos(angle * DEG2RAD);
  float x1 = thick * cx + m_cx;
  float y1 = -thick * sx + m_cy;
  angle -= 180;
  sx = sin(angle * DEG2RAD);
  cx = cos(angle * DEG2RAD);
  float x2 = thick * cx + m_cx;
  float y2 = -thick * sx + m_cy;
  angle = LEFT - SWEEP * normPos + tipThick;
  sx = sin(angle * DEG2RAD);
  cx = cos(angle * DEG2RAD);
  float x4 = m_cx + length * cx * m_r;
  float y4 = m_cy - length * sx * m_r;

  m_canvas->fillTriangle(x0, y0, x1, y1, x2, y2, color);
  m_canvas->fillTriangle(x0, y0, x4, y4, x1, y1, color);
}

void AnalogGauge::DrawMaxVal(float normPos, float length, float size, uint16_t color)
{
  float angle = LEFT - SWEEP * normPos;
  float sx = sin(angle * DEG2RAD);
  float cx = cos(angle * DEG2RAD);
  float x0 = m_cx + length * cx * m_r;
  float y0 = m_cy - length * sx * m_r;
  m_canvas->fillCircle(x0, y0, size, color);
}


void AnalogGauge::DrawDynamic()
{
  m_canvas->createSprite(m_w, m_h);
  m_canvas->fillSprite(TFT_TRANSPARENT);
  m_canvas->setTextColor(m_txtColor);

  // clear old needle display
  DrawNeedle(m_oldValue, NDL_LNTH + 0.01, NDL_BASE + 0.05, NDL_TIP + 2, m_dialColor);

  // clear old max val
  DrawMaxVal(m_oldMaxVal, MAX_VAL_LNG, MAX_VAL_SIZ * m_r + 2, m_dialColor);

  // major tick mark labels
  for (int i = 0; i <= LRG_TICK_COUNT; i++)
  {
    float angle = RIGHT + SWEEP / LRG_TICK_COUNT * i;
    float sx = sin(angle * DEG2RAD);
    float cx = cos(angle * DEG2RAD);

    // tick number labels
    float x0 = cx * m_r * TICK_LBL_RAD + m_cx;
    float y0 = -sx * m_r * TICK_LBL_RAD + m_cy;
    float mark = m_watched->High - m_watched->Range() / LRG_TICK_COUNT * i;
    SetFontSize(m_smallFont);
    m_canvas->setTextDatum(textdatum_t::middle_center);
    m_canvas->drawNumber(mark, x0, y0);
  }

  // units text
  SetFontSize(m_medFont);
  m_canvas->setTextDatum(textdatum_t::middle_center);
  m_canvas->drawString(m_units, m_cx, m_cy - m_r * UNIT_Y);

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

  byte r, g, b;

  // draw max val
  if (value > m_maxVal)
  {
    m_oldMaxVal = m_maxVal;
    m_maxVal = value;
  }

  float maxScaled = m_maxVal * m_watched->Range() + m_watched->Low;
  m_watched->ColorForValue(maxScaled, r, g, b);
  DrawMaxVal(m_maxVal, MAX_VAL_LNG, MAX_VAL_SIZ * m_r, M5.Display.color565(r, g, b));


  m_watched->Color(r, g, b);
  DrawNeedle(value, NDL_LNTH, NDL_BASE, NDL_TIP, M5.Display.color565(r, g, b));

  // middle button
  m_canvas->fillCircle(m_cx, m_cy, NDL_BASE * m_r + 2, m_txtColor);

  m_oldValue = value;

  m_canvas->pushSprite(m_x, m_y, TFT_TRANSPARENT);
  m_canvas->deleteSprite();
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
