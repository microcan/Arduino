#include "Panel1x3.h"
#include "Microcan.h"

#define MARGIN 12

Panel1x3::Panel1x3(WatchedValue &left, WatchedValue &center, WatchedValue &right)
{
    m_gauges[0] = new AnalogGauge(left);
    m_gauges[1] = new AnalogGauge(center);
    m_gauges[2] = new AnalogGauge(right);
    m_canvas = new M5Canvas(&M5.Lcd);
    SizeGauges();
}

void Panel1x3::SetSize(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_first = true;
    SizeGauges();
}

void Panel1x3::Update()
{
    if (m_first)
    {
        DrawStatic();
        m_first = false;
    }
    for (int i = 0; i < 3; i++)
    {
        m_gauges[i]->Update();
    }
}

void Panel1x3::DrawStatic()
{
    m_canvas->createSprite(m_w, m_h);

    //m_canvas->fillSprite(M5.Display.color565(120, 120, 120));
    //m_canvas->fillRect(6, 6, m_w - MARGIN, m_h - MARGIN, TFT_BLACK);
    m_canvas->fillSprite(TFT_BLACK);
    
    m_canvas->pushSprite(m_x, m_y);
    m_canvas->deleteSprite();
}

void Panel1x3::SizeGauges()
{
    int v1 = MARGIN;
    int v2 = m_w / 3 - MARGIN;
    int v3 = v2 + MARGIN * 2;
    int v4 = m_w * 2 / 3 - MARGIN;
    int v5 = v4 + MARGIN * 2;
    int v6 = m_w - MARGIN;
    int h1 = MARGIN;
    int h2 = m_h - MARGIN;

    m_gauges[0]->SetSize(m_x + v1, m_y + h1, v2 - v1, h2 - h1);
    m_gauges[1]->SetSize(m_x + v3, m_y + h1, v4 - v3, h2 - h1);
    m_gauges[2]->SetSize(m_x + v5, m_y + h1, v6 - v5, h2 - h1);
}