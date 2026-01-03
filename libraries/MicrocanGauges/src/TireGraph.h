#ifndef TIRE_GRAPH_H
#define TIRE_GRAPH_H

#include <M5Unified.h>
#include "TirePatchGraph.h"

// sizeable control that display four tire patches and a temperature legend
class TireGraph
{
private:
    int m_x, m_y, m_w, m_h;
    M5Canvas *m_canvas;
    TirePatchGraph *m_patches[4];
    bool m_first = true;
    WatchedValue* m_watched;

public:
    // Construct with the number of zones and pointer to the data
    // zones: 1, 2, 4, 8 or 16, as configured in TireX
    // data: If you are using TireX, just asl.Tirex.Temps
    TireGraph(int zones, byte data[4][16]);

    // Sets the size an position of the control
    // x, y: top left corner
    // w, h: width and height
    void SetSize(int x, int y, int w, int h);

    // Set the temp range to display, in deg C
    // minT: blue end
    // maxT: red end
    void SetPrefs(int minT, int maxT);

    // call this in your loop
    void Update();

private:
    void LayoutPatches();
    void DrawStatic();
    void DrawDynamic();
};

#endif