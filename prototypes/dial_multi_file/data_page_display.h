#ifndef DATA_PAGE_DISPLAY_H
#define DATA_PAGE_DISPLAY_H

#include <M5GFX.h>
#include <M5Unified.h>
#include "subi_data.h"

class DataPageDisplayer
{
    private:
    SubiDataManager* m_manager;
    M5Canvas* m_canvas;

    public:
    DataPageDisplayer(SubiDataManager* manager);
    void Update();
};

#endif