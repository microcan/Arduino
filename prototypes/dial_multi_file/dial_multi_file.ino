#include <M5GFX.h>
#include <M5Unified.h>
#include <M5Dial.h>
#include "subi_data.h"
#include "tire_page_display.h"
#include "data_page_display.h"

SubiDataManager*  manager;
TirePageDisplayer* tirePage;
DataPageDisplayer* dataPage;

bool onTirePage = true;

void setup() 
{
    M5.begin();
    M5.Lcd.fillScreen(TFT_WHITE);
    
    manager = new SubiDataManager();
    tirePage = new TirePageDisplayer(manager);
    dataPage = new DataPageDisplayer(manager);
}

void loop() 
{
  M5Dial.update();

  if (M5.BtnA.wasSingleClicked())
  {
    onTirePage = !onTirePage;
    M5.Lcd.fillScreen(TFT_WHITE);
  }

  manager->Update();
  if (onTirePage)
  {
    tirePage->Update();
  }
  else
  {
    dataPage->Update();
  }
} 
