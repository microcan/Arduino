#ifndef TIRE_GRAPH_2_PANEL_H
#define TIRE_GRAPH_2_PANEL_H

#include <M5Unified.h>
#include "TirePatchGraph.h"

// sizeable control that display two tire patches and a temperature legend
class TireGraph2Panel
{
private:
    int m_x, m_y, m_w, m_h;
    M5Canvas *m_canvas;
    TirePatchGraph *m_patches[2];
    bool m_first = true;
    WatchedValue* m_min;
    WatchedValue* m_max;
    float m_oldMin = 0.5;
    float m_oldMax = 0.5;
    int m_zones = 4;
    byte (*m_data)[16];

public:
    // Construct with the number of zones and pointer to the data
    // zones: 1, 2, 4, 8 or 16, as configured in TireX
    // data: If you are using TireX, just asl.Tirex.Temps
    TireGraph2Panel(int zones, byte data[4][16]);

    // Sets the size an position of the control
    // x, y: top left corner
    // w, h: width and height
    void SetSize(int x, int y, int w, int h);

    // Set the temp range to display, in deg C
    // minT: blue end
    // maxT: red end
    void SetPrefs(int minT, int maxT);

    void ScaleToRange();

    // call this in your loop
    void Update();

private:
    void LayoutPatches();
    void DrawStatic();
    void DrawDynamic();
};

#endif