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

    public:
    Gauge(String label, float min, float max);
    void Update(float value);
};

#endif