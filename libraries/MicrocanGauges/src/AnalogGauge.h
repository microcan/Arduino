#ifndef ANALOG_GAUGE_H
#define ANALOG_GAUGE_H

#include <M5GFX.h>
#include <M5Unified.h>
#include <Microcan.h>

class AnalogGauge
{
private:
    M5Canvas *m_canvas;
    WatchedValue *m_watched;
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
    float m_oldValue;
    float m_maxChange;
    bool m_first = true;

public:
    AnalogGauge(String label, String units, WatchedValue &watched);
    void SetSize(int x, int y, int w, int h);
    void Update();

private:
    void SetFontSize(int points);
};

#endif