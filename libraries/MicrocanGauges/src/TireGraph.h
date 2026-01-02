#ifndef TIRE_GRAPH_H
#define TIRE_GRAPH_H

#include "TirePatchGraph.h"

class TireGraph
{
private:
    int m_x, m_y, m_w, m_h;
    M5Canvas *m_canvas;
    TirePatchGraph *m_patches[4];
    bool m_first = true;
    WatchedValue* m_watched;

public:
    TireGraph(int zones, byte data[4][16]);
    void SetSize(int x, int y, int w, int h);
    void SetPrefs(int minT, int maxT);
    void Update();

private:
    void LayoutPatches();
    void DrawStatic();
    void DrawDynamic();
};

#endif