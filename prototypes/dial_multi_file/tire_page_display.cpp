#include "tire_page_display.h"

TirePageDisplayer::TirePageDisplayer(SubiDataManager* manager)
{
    int displayW, displayH, patchW, patchH, margin;

    m_manager = manager;
        
    margin = 5;
    displayW = M5.Lcd.width();
    displayH = M5.Lcd.height();
    patchW = displayW / 2 - 2* margin;
    patchH = displayH / 2 - 2* margin;

    m_fl = new TirePatchDisplayer(margin, margin, patchW, patchH, 0, manager);
    m_fr = new TirePatchDisplayer(displayW/2 + margin, margin, patchW, patchH, 1, manager);
    m_rl = new TirePatchDisplayer(margin, displayH/ 2 + margin, patchW, patchH, 2, manager);
    m_rr = new TirePatchDisplayer(displayW/2 + margin, displayH/2 + margin, patchW, patchH, 3, manager);
};

void TirePageDisplayer::Update()
{
  m_fl->Update();
  m_fr->Update();
  m_rl->Update();
  m_rr->Update();
};