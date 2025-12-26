#ifndef TIRE_PAGE_DISPLAY_H
#define TIRE_PAGE_DISPLAY_H

#include <M5GFX.h>
#include <M5Unified.h>
#include "subi_data.h"
#include "tire_patch_display.h"

class TirePageDisplayer
{
    private:
    SubiDataManager* m_manager;

    TirePatchDisplayer* m_fl;
    TirePatchDisplayer* m_fr;
    TirePatchDisplayer* m_rl;
    TirePatchDisplayer* m_rr;
    
    public:
    TirePageDisplayer(SubiDataManager* manager);
    void Update();
};


#endif