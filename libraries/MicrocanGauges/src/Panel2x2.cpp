#include "Panel2x2.h"
#include "Microcan.h"

#define MARGIN 12

Panel2x2::Panel2x2(WatchedValue &tl, WatchedValue &tr, WatchedValue &bl, WatchedValue &br)
{
    m_gauges[0] = new DigitalGauge(tl);
    m_gauges[1] = new DigitalGauge(tr);
    m_gauges[2] = new DigitalGauge(bl);
    m_gauges[3] = new DigitalGauge(br);
    m_canvas = new M5Canvas(&M5.Lcd);
    SizeGauges();
}

void Panel2x2::SetSize(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_first = true;
    SizeGauges();
}

void Panel2x2::Update()
{
    if (m_first)
    {
        DrawStatic();
        m_first = false;
    }
    for (int i = 0; i < 4; i++)
    {
        m_gauges[i]->Update();
    }
}

void Panel2x2::DrawStatic()
{
    m_canvas->createSprite(m_w, m_h);

    //m_canvas->fillSprite(M5.Display.color565(120, 120, 120));
    //m_canvas->fillRect(6, 6, m_w - MARGIN, m_h - MARGIN, TFT_BLACK);
    m_canvas->fillSprite(TFT_BLACK);
    
    m_canvas->pushSprite(m_x, m_y);
    m_canvas->deleteSprite();
}

void Panel2x2::SizeGauges()
{
    int v1 = MARGIN;
    int v2 = m_w / 2 - MARGIN;
    int v3 = v2 + MARGIN * 2;
    int v4 = m_w - MARGIN;
    int h1 = MARGIN;
    int h2 = m_h / 2 - MARGIN;
    int h3 = h2 + MARGIN * 2;
    int h4 = m_h - MARGIN;

    m_gauges[0]->SetSize(m_x + v1, m_y + h1, v2 - v1, h2 - h1);
    m_gauges[1]->SetSize(m_x + v3, m_y + h1, v4 - v3, h2 - h1);
    m_gauges[2]->SetSize(m_x + v1, m_y + h3, v2 - v1, h4 - h3);
    m_gauges[3]->SetSize(m_x + v3, m_y + h3, v4 - v3, h4 - h3);
}