#ifndef ANALOG_GAUGE_H
#define ANALOG_GAUGE_H

#include <M5Unified.h>
#include <Microcan.h>

// Place an analog gauge on the screen that automatically tracks a WatchedValue
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
    float m_maxVal;
    float m_oldMaxVal;
    float m_maxChange;
    uint16_t m_dialColor;
    uint16_t m_txtColor;
    bool m_first = true;

public:
    // Construct with the value to watch
    // watched: the value that will place the needle, and set the number on the ticks from it the range
    AnalogGauge(WatchedValue &watched);

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
    void DrawNeedle(float normPos, float length, float baseThick, float tipThick, uint16_t color);
    void DrawMaxVal(float normPos, float length, float size, uint16_t color);
};

#endif