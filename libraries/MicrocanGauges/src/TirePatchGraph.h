#ifndef TIRE_PATCH_GRAPH_H
#define TIRE_PATCH_GRAPH_H

#include <M5Unified.h>
#include <Microcan.h>

class TirePatchGraph
{
private:
    int m_x, m_y, m_w, m_h;
    byte *m_data;
    int m_zones;
    bool m_flip;
    WatchedValue *m_watched;

    M5Canvas *m_canvas;

public:
    TirePatchGraph(int zones, byte *data, bool flip);
    void SetSize(int x, int y, int w, int h);
    void SetPrefs(int minT, int maxT);
    void Update();
};

#endif