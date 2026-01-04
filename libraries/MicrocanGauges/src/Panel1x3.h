#ifndef PANEL_1X3_H
#define PANEL_1X3_H

#include <M5Unified.h>
#include "Microcan.h"
#include "AnalogGauge.h"

class Panel1x3
{
private:
    AnalogGauge* m_gauges[3];
    int m_x = 0;
    int m_y = 0;
    int m_w = 150;
    int m_h = 50;
    bool m_first = true;
    M5Canvas* m_canvas;

public:
    Panel1x3(WatchedValue &left, WatchedValue &center, WatchedValue &right);
    void SetSize(int x, int y, int w, int h);
    void Update();

private:
    void DrawStatic();
    void SizeGauges();
};

#endif