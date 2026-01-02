#include "TireGraph.h"

#define PATCH_FRAC 0.85
#define MARGIN 12
#define TILE_COUNT 20.0

TireGraph::TireGraph(int zones, byte data[4][16])
{
    int z = 4;
    if(zones == 1 || zones == 2 || zones == 4 || zones == 8 || zones == 16)
    {
        z = zones;
    }
    m_canvas = new M5Canvas(&M5.Lcd);
    m_watched = new WatchedValue("Tire", "2 x C", 20, 60);
    m_patches[0] = new TirePatchGraph(z, data[0], false);
    m_patches[1] = new TirePatchGraph(z, data[1], true);
    m_patches[2] = new TirePatchGraph(z, data[2], false);
    m_patches[3] = new TirePatchGraph(z, data[3], true);
    SetSize(0, 0, 400, 400);
    SetPrefs(20, 80);
}

void TireGraph::LayoutPatches()
{
    int v1 = MARGIN;
    int v2 = m_w / 2 - MARGIN;
    int v3 = v2 + MARGIN * 2;
    int v4 = m_w - MARGIN;
    int h1 = MARGIN;
    int h2 = m_h * PATCH_FRAC / 2 - MARGIN;
    int h3 = h2 + MARGIN * 2;
    int h4 = m_h * PATCH_FRAC - MARGIN;
    int h5 = h4 + 2 * MARGIN;
    int h6 = m_h - MARGIN;
    m_patches[0]->SetSize(m_x + v1, m_y + h1, v2 - v1, h2 - h1);
    m_patches[1]->SetSize(m_x + v3, m_y + h1, v4 - v3, h2 - h1);
    m_patches[2]->SetSize(m_x + v1, m_y + h3, v2 - v1, h4 - h3);
    m_patches[3]->SetSize(m_x + v3, m_y + h3, v4 - v3, h4 - h3);
}

void TireGraph::SetSize(int x, int y, int w, int h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    LayoutPatches();
    m_first = true;
}

void TireGraph::SetPrefs(int minT, int maxT)
{
    delete m_watched;
    m_watched = new WatchedValue("Tire", "2 x C", minT * 2, maxT * 2);
    m_patches[0]->SetPrefs(minT, maxT);
    m_patches[1]->SetPrefs(minT, maxT);
    m_patches[2]->SetPrefs(minT, maxT);
    m_patches[3]->SetPrefs(minT, maxT);
}

void TireGraph::Update()
{
    if (m_first)
    {
        DrawStatic();
        m_first = false;
    }

    m_patches[0]->Update();
    m_patches[1]->Update();
    m_patches[2]->Update();
    m_patches[3]->Update();

    DrawDynamic();
}

void TireGraph::DrawStatic()
{
    m_canvas->createSprite(m_w, m_h);

    int v1 = MARGIN;
    int v2 = m_w / 2 - MARGIN;
    int v3 = v2 + MARGIN * 2;
    int v4 = m_w - MARGIN;
    int h1 = MARGIN;
    int h2 = m_h * PATCH_FRAC / 2 - MARGIN;
    int h3 = h2 + MARGIN * 2;
    int h4 = m_h * PATCH_FRAC - MARGIN;
    int h5 = h4 + 2 * MARGIN;
    int h6 = m_h - MARGIN;

    float tileH = h6 - h5;
    float tileY = m_y + h5;
    float tileX = m_x + v1;
    float fullW = v4 - v1;

    for (int i = 0; i < TILE_COUNT; i++)
    {
        byte r, g, b;
        float tileW = fullW * ((TILE_COUNT - i) / TILE_COUNT);
        float value = (TILE_COUNT - i - 1) / (TILE_COUNT - 1) * m_watched->Range() + m_watched->Low;
        m_watched->ColorForValue(value, r, g, b);
        m_canvas->fillRect(tileX, tileY, tileW, tileH, M5.Display.color565(r, g, b));
    }

    m_canvas->setTextColor(TFT_BLACK);
    m_canvas->setFont(&fonts::DejaVu40);
    m_canvas->setTextDatum(textdatum_t::middle_left);
    m_canvas->drawFloat(m_watched->Low / 2.0, 0, v1 + 8, (h5 + h6) / 2 + 2);

    m_canvas->setTextDatum(textdatum_t::middle_right);
    m_canvas->drawFloat(m_watched->High / 2.0, 0, v4 - 8, (h5 + h6) / 2 + 2);

    m_canvas->setTextDatum(textdatum_t::middle_center);
    for (int i = 0; i < 3; i++)
    {
        float value = (m_watched->Low + m_watched->Range() / 4.0 * (i+1)) / 2.0;
        int x = v1 + (v4 - v1) * (i+1) / 4.0;
        m_canvas->drawFloat(value, 0, x, (h5 + h6) / 2 + 2);
    }

    m_canvas->pushSprite(m_x, m_y);
    m_canvas->deleteSprite();
}

void TireGraph::DrawDynamic()
{
}