#ifndef DIGITAL_GAUGE_H
#define DIGITAL_GAUGE_H

#include <M5Unified.h>
#include <Microcan.h>

// Place an digital gauge on the screen that automatically tracks a WatchedValue
class DigitalGauge
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
    int m_size;
    int m_smallFont;
    int m_medFont;
    int m_bigFont;
    float m_oldValue;
    float m_maxVal;
    float m_maxChange;
    uint16_t m_dialColor;
    uint16_t m_txtColor;
    bool m_first = true;

public:
    // Construct with the value to watch
    // watched: the value that will place the needle, and set the number on the ticks from it the range
    DigitalGauge(WatchedValue &watched);

    // Screen pixel size and placement for the gauge
    // x, y: top left corner of the gauge in pixels
    // w, h: width and height in pixels
    void SetSize(int x, int y, int w, int h);

    // Call this in your loop
    void Update();

private:
    void SetFontSize(int points);
    void DrawStatic();
    void DrawDynamic();
};

#endif