#include "TireGraph2Panel.h"

#define PATCH_FRAC 0.75
#define MARGIN 12
#define TILE_COUNT 20.0

TireGraph2Panel::TireGraph2Panel(int zones, byte data[4][16])
{
    m_data = data;
    if (zones == 1 || zones == 2 || zones == 4 || zones == 8 || zones == 16)
    {
        m_zones = zones;
    }
    m_canvas = new M5Canvas(&M5.Lcd);
    m_min = new WatchedValue("Tire", "2 x C", 0, 127);
    m_max = new WatchedValue("Tire", "2 x C", 0, 127);
    m_patches[0] = new TirePatchGraph(m_zones, data[0], true);
    m_patches[1] = new TirePatchGraph(m_zones, data[1], false);
    SetSize(0, 0, 400, 400);
    SetPrefs(0, 127);
}

void TireGraph2Panel::LayoutPatches()
{
    int v1 = MARGIN;
    int v2 = m_w / 2 - MARGIN;
    int v3 = v2 + MARGIN * 2;
    int v4 = m_w - MARGIN;
    int h1 = MARGIN;
    int h2 = m_h * PATCH_FRAC - MARGIN;
    int h3 = h2 + MARGIN * 2;
    int h4 = m_h - MARGIN;
    m_patches[0]->SetSize(m_x + v1, m_y + h1, v2 - v1, h2 - h1);
    m_patches[1]->SetSize(m_x + v3, m_y + h1, v4 - v3, h2 - h1);
}

void TireGraph2Panel::SetSize(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    LayoutPatches();
    m_first = true;
}

void TireGraph2Panel::ScaleToRange()
{
    int newMin = m_min->Value / 2 - 5;
    int newMax = m_max->Value / 2 + 5;
    SetPrefs(newMin, newMax);
}

void TireGraph2Panel::SetPrefs(int minT, int maxT)
{
    delete m_min;
    delete m_max;
    m_min = new WatchedValue("TireMin", "2 x C", minT * 2, maxT * 2);
    m_min->Value= m_min->High;
    m_max = new WatchedValue("TireMax", "2 x C", minT * 2, maxT * 2);
    m_max->Value = m_max->Low;
    m_first = true;
    m_patches[0]->SetPrefs(minT, maxT);
    m_patches[1]->SetPrefs(minT, maxT);
}

void TireGraph2Panel::Update()
{
    if (m_first)
    {
        DrawStatic();
        m_first = false;
    }

    m_patches[0]->Update();
    m_patches[1]->Update();

    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < m_zones; i++)
        {
            if (m_data[j][i] > m_max->Value)
                m_max->Value = m_data[j][i];
            if (m_data[j][i] > 0 && m_data[j][i] < m_min->Value)
                m_min->Value = m_data[j][i];
        }
    }

    DrawDynamic();
}

void TireGraph2Panel::DrawStatic()
{
    m_canvas->createSprite(m_w, m_h);

    // m_canvas->fillSprite(M5.Display.color565(120, 120, 120));
    // m_canvas->fillRect(6, 6, m_w - 12, m_h - 12, TFT_BLACK);
    m_canvas->fillSprite(TFT_BLACK);

    int v1 = MARGIN;
    int v2 = m_w / 2 - MARGIN;
    int v3 = v2 + MARGIN * 2;
    int v4 = m_w - MARGIN;
    int h1 = MARGIN;
    int h2 = m_h * PATCH_FRAC - MARGIN;
    int h3 = h2 + MARGIN * 2;
    int h4 = m_h - MARGIN;

    float tileH = h4 - h3;
    float tileY = h3;
    float tileX = v1;
    float fullW = v4 - v1;

    for (int i = 0; i < TILE_COUNT; i++)
    {
        byte r, g, b;
        float tileW = fullW * ((TILE_COUNT - i) / TILE_COUNT);
        float value = (TILE_COUNT - i - 1) / (TILE_COUNT - 1) * m_min->Range() + m_min->Low;
        m_min->ColorForValue(value, r, g, b);
        m_canvas->fillRect(tileX, tileY, tileW, tileH, M5.Display.color565(r, g, b));
    }

    m_canvas->setTextColor(TFT_BLACK);
    m_canvas->setFont(&fonts::DejaVu40);
    m_canvas->setTextDatum(textdatum_t::middle_left);
    m_canvas->drawFloat(m_min->Low / 2.0, 0, v1 + 8, (h3 + h4) / 2 + 2);

    m_canvas->setTextDatum(textdatum_t::middle_right);
    m_canvas->drawFloat(m_min->High / 2.0, 0, v4 - 8, (h3 + h4) / 2 + 2);

    m_canvas->setTextDatum(textdatum_t::middle_center);
    for (int i = 0; i < 3; i++)
    {
        float value = (m_min->Low + m_min->Range() / 4.0 * (i + 1)) / 2.0;
        int x = v1 + (v4 - v1) * (i + 1) / 4.0;
        m_canvas->drawFloat(value, 0, x, (h3 + h4) / 2 + 2);
    }

    m_canvas->pushSprite(m_x, m_y);
    m_canvas->deleteSprite();
}

void TireGraph2Panel::DrawDynamic()
{
    if (m_oldMax != m_max->Normalized() || m_oldMin != m_min->Normalized())
    {
        m_canvas->createSprite(m_w, m_h);
        m_canvas->fillSprite(TFT_TRANSPARENT);

        float fullW = m_w - MARGIN * 2;
        int r = 8;
        int y = m_h * PATCH_FRAC + MARGIN - r - 2;

        //if (m_oldMax != m_max->Normalized())
        //{
            int x = MARGIN + fullW * m_oldMax;
            m_canvas->fillCircle(x, y, r, TFT_BLACK);
            x = MARGIN + fullW * m_max->Normalized();
            m_canvas->fillCircle(x, y, r, TFT_RED);
            m_oldMax = m_max->Normalized();
        //}

        //if (m_oldMin != m_min->Normalized())
        //{
            x = MARGIN + fullW * m_oldMin;
            m_canvas->fillCircle(x, y, r, TFT_BLACK);
            x = MARGIN + fullW * m_min->Normalized();
            m_canvas->fillCircle(x, y, r, TFT_BLUE);
            m_oldMin = m_min->Normalized();
        //}

        m_canvas->pushSprite(m_x, m_y, TFT_TRANSPARENT);
        m_canvas->deleteSprite();
    }
}