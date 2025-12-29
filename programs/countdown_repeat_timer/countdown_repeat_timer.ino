#include <M5GFX.h>
#include <M5Dial.h>

#define TIMING 0 
#define SETTING 1

int g_timer = 15;
int g_mode = TIMING;
int g_startTick = 0;
int g_reps = 0;
int g_encoderCount = 0;
M5Canvas* g_canvas;

void UpdateDisplay(int remain, String info)
{
  int remain_min = floor(remain / 60.0);
  int remain_sec = floor(remain - 60.0 * remain_min);
  char display[256];
  sprintf(display, "%d:%02d", remain_min, remain_sec);

  g_canvas->createSprite(M5Dial.Lcd.width(), M5Dial.Lcd.height());
  g_canvas->fillSprite(TFT_BLACK);

  g_canvas->setTextColor(TFT_WHITE, TFT_BLACK);
  g_canvas->setFont(&fonts::DejaVu72);
  g_canvas->setTextDatum(textdatum_t::middle_center);
  g_canvas->drawString(display, M5Dial.Lcd.width() / 2, M5Dial.Lcd.height() / 2);

  g_canvas->setFont(&fonts::DejaVu24);
  g_canvas->setTextDatum(textdatum_t::middle_center);
  g_canvas->drawString(info, M5Dial.Lcd.width() / 2, M5Dial.Lcd.height() / 4);

  g_canvas->pushSprite(0, 0);
  g_canvas->deleteSprite();
}

void setup() {
  auto cfg = M5.config();
  M5Dial.begin(cfg, true, false);  // encoder, RFID
  g_canvas = new M5Canvas(&M5Dial.Lcd); 
  g_startTick = millis();
  int dummy = M5Dial.Encoder.readAndReset();
}

void loop() {
  M5Dial.update();

  int now = millis();

  // switch to setting mode if encoder has changed
  int encoder = M5Dial.Encoder.readAndReset();
  if (g_mode == TIMING && encoder != 0)
  {
    g_mode = SETTING;
  }

  // start/restart the timer when the button is clicked.
  // this needs to be checked after the encoder as sometimes 
  // the user moves theencoder when clicking
  if (M5Dial.BtnA.wasSingleClicked())
  {
    M5Dial.Speaker.tone(4000, 20);
    g_mode = TIMING;
    g_startTick = now;
    g_reps = 0;
  }

  // the encoder mechanical detents are every four encoder changes,
  // only change the setpoint every four encoder changes.
  if (g_mode == SETTING)
  {
    g_encoderCount += encoder;
    if (g_encoderCount >= 4)
    {
      g_timer += 5;
      g_encoderCount = 0;
      M5Dial.Speaker.tone(8000, 20);
    }
    if (g_encoderCount <= -4)
    {
      g_timer -= 5;
      g_encoderCount = 0;
      M5Dial.Speaker.tone(6000, 20);
    }
    if (g_timer < 15) g_timer = 15;
    if (g_timer > 300) g_timer = 300;
    UpdateDisplay(g_timer, "Set");
  }
  
  // if timing count down and repeat, incrementing reps every repeat
  if (g_mode == TIMING)
  {
    int remain = g_timer - (now - g_startTick) / 1000;
    if (remain < 0)
    {
      M5Dial.Speaker.tone(4000, 400);
      remain = g_timer;
      g_startTick = now + 400;
      g_reps++;
    }
    char reps[256];
    sprintf(reps, "Rep: %02d", g_reps + 1);
    UpdateDisplay(remain, reps);
  }
}

