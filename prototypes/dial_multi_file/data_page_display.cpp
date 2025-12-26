#include "data_page_display.h"

static int displayW, displayH, margin, height;

DataPageDisplayer::DataPageDisplayer(SubiDataManager* manager)
{
    m_manager = manager;
    m_canvas = new M5Canvas(&M5.Lcd);    
    displayW = M5.Lcd.width();
    displayH = M5.Lcd.height();
    margin = 3;
    height = displayH / 4;
};

void DisplayValue(M5Canvas* canvas, float value, float low, float high, int offset, int decimals)
{
    uint16_t color = TFT_RED;
  if (value < low)
  {
    color = TFT_BLUE;
  }
  else if (value > high)
  {
    color = TFT_RED;
  }
  else
  {
    color = TFT_BLACK;
  }

  canvas->setTextDatum(textdatum_t::top_right);
  canvas->setTextColor(TFT_WHITE, color);
  canvas->fillRect(margin + 70, offset, displayW - 70 - margin, height, color);
  canvas->setFont(&fonts::DejaVu56);
  canvas->drawFloat(value, decimals, displayW - margin, offset + margin);
}

void DataPageDisplayer::Update()
{

  SubiData data = m_manager->GetData();
  
  m_canvas->createSprite(displayW, displayH);

  m_canvas->setTextColor(TFT_WHITE, TFT_BLACK);
  m_canvas->setFont(&fonts::DejaVu18);
  m_canvas->setTextDatum(textdatum_t::top_right);
  m_canvas->drawString("Oil", 70 - margin, margin);
  m_canvas->drawString("Water", 70 - margin, height + margin);
  m_canvas->drawString("Oil P", 70 - margin, height * 2 + margin);
  m_canvas->drawString("RPM", 70 - margin, height * 3 + margin);

  DisplayValue(m_canvas, data.OilTemp, 70, 110, 0, 0);
  DisplayValue(m_canvas, data.WaterTemp, 70, 110, height, 0);
  DisplayValue(m_canvas, data.OilPress, 0.5, 10, height * 2, 1);
  DisplayValue(m_canvas, data.RPM, 0, 7000, height * 3, 0);

  m_canvas->pushSprite(0, 0);
  m_canvas->deleteSprite();
};