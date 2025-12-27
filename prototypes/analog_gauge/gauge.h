#ifndef GAUGE_H
#define GAUGE_H

#include <M5GFX.h>
#include <M5Unified.h>

class Gauge
{
    private:
    M5Canvas* m_canvas;
    float m_min;
    float m_max;
    String m_label;
    String m_units;
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    int m_cx;
    int m_cy;
    int m_r;
    int m_smallFont;
    int m_medFont;
    int m_bigFont;
    int m_oldValue;
    int m_maxChange;

    public:
    Gauge(String label, String units, float min, float max);
    void SetSize(int x, int y, int w, int h);
    void Update(float value);

    private:
    void SetFontSize(int points);
};

#endif