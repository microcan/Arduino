#ifndef TIRE_PATCH_DISPLAY_H
#define TIRE_PATCH_DISPLAY_H

#include <M5GFX.h>
#include <M5Unified.h>
#include "subi_data.h"

class TirePatchDisplayer
{
    private:
    int m_x, m_y, m_w, m_h;
    int m_tire;

    M5Canvas* m_canvas;
    SubiDataManager* m_manager;

    public:
    TirePatchDisplayer(int x, int y, int w, int h, int tire, SubiDataManager* manager);
    void Update();
};

#endif