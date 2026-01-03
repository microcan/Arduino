#ifndef PANEL_2X2_H
#define PANEL_2X2_H

#include <M5Unified.h>
#include "Microcan.h"
#include "AnalogGauge.h"

class Panel2x2
{
private:
    AnalogGauge* m_gauges[4];
    int m_x = 0;
    int m_y = 0;
    int m_w = 100;
    int m_h = 100;
    bool m_first = true;
    M5Canvas* m_canvas;

public:
    Panel2x2(WatchedValue &tl, WatchedValue &tr, WatchedValue &bl, WatchedValue &br);
    void SetSize(int x, int y, int w, int h);
    void Update();

private:
    void DrawStatic();
    void SizeGauges();
};

#endif