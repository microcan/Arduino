#include <M5Unified.h>
#include <MicrocanAsl.h>
#include <MicrocanGauges.h>

AslManager asl;
TireGraph* patch;
AnalogGauge* gauges[4];
WatchedValue watch(0, 10);

void setup() 
{
  M5.begin();

  asl.Connect(6, 7);

  int w = M5.Lcd.width();
  int h = M5.Lcd.height();
  patch = new TireGraph(8, asl.TireX.Temps);
  patch->SetSize(0, 0, w, h * 0.4);
  patch->SetPrefs(20, 40);

  gauges[0] = new AnalogGauge("First", "deg C", watch);
  gauges[1] = new AnalogGauge("Second", "deg C", watch);
  gauges[2] = new AnalogGauge("Third", "deg C", watch);
  gauges[3] = new AnalogGauge("Fourth", "deg C", watch);

  int gw = w/2 - 20;
  int gh = h*0.3 - 20;

  gauges[0]->SetSize(10, h * 0.4 + 10, gw, gh);
  gauges[1]->SetSize(w/2 + 10, h * 0.4 + 10, gw, gh);
  gauges[2]->SetSize(10, h * 0.7 + 10, gw, gh);
  gauges[3]->SetSize(w/2 + 10, h * 0.7 + 10, gw, gh);
}

void loop() 
{
  asl.Update();
  patch->Update();

  gauges[0]->Update();
  gauges[1]->Update();
  gauges[2]->Update();
  gauges[3]->Update();  
}
