#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>

// Based on PWRCAN module DIP switch config
#define CAN_TX 17
#define CAN_RX 16

// subaru CAN IDs
#define SUB_OIL_COOL 864
#define SUB_RPM_ACC 320
#define ODB_RPM 2024

M5GFX display;

bool CanOk;

int OilTemp;
int WaterTemp;
int RPM;

int Width;
int Height;

bool oilChanged;
bool waterChanged;
bool rpmChanged;

uint32_t minUpdate;
uint32_t lastUpdate;

CanFrame rxFrame;

void updateDisplay() {
  if (oilChanged || waterChanged || rpmChanged) {
    display.startWrite();
    display.setFont(&fonts::DejaVu72);
    display.setTextDatum(textdatum_t::top_right);
  }

  if (oilChanged) {
    oilChanged = false;
    display.fillRect(0, 20, Width, 72, TFT_BLACK);
    display.drawNumber(OilTemp, Width - 20, 20);
  }

  if (waterChanged) {
    waterChanged = false;
    display.fillRect(0, 116, Width, 72, TFT_BLACK);
    display.drawNumber(WaterTemp, Width - 20, 116);
  }

  if (rpmChanged) {
    rpmChanged = false;
    display.fillRect(0, 212, Width, 72, TFT_BLACK);
    display.drawNumber(RPM, Width - 20, 212);
  }

  if (oilChanged || waterChanged || rpmChanged) {
    display.endWrite();
  }
}

void initDisplay() {
  display.startWrite();
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setFont(&fonts::Font2);
  display.setTextDatum(textdatum_t::top_left);
  
  display.drawString("Oil Temp C", 0, 0);
  display.drawString("Coolant Temp C", 0, 96);
  display.drawString("RPM", 0, 192);

  display.endWrite();

  updateDisplay();
}

void setup() {
  int baud = 500;
  OilTemp = 888;
  WaterTemp = 888;
  RPM = 8888;
  oilChanged = true;
  waterChanged = true;
  rpmChanged = true;
  minUpdate = 200;
  lastUpdate = 0;

  display.begin();
  if (display.width() > display.height()) {
    display.setRotation(display.getRotation() ^ 1);
  }

  Width = display.width();
  Height = display.height();

  if (ESP32Can.begin(ESP32Can.convertSpeed(baud), CAN_TX, CAN_RX, 10, 10)) {
    CanOk = true;
  } else {
    CanOk = false;
  }

  initDisplay();

  OilTemp = 0;
  WaterTemp = 0;
  RPM = 0;
  oilChanged = true;
  waterChanged = true;
  rpmChanged = true;
}

void loop() {
  if (ESP32Can.readFrame(rxFrame, 500)) {
    //Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
    if (rxFrame.identifier == SUB_OIL_COOL) {
      OilTemp = rxFrame.data[2] - 40;
      WaterTemp = rxFrame.data[3] - 40;
      oilChanged = true;
      waterChanged = true;
    }
    if (rxFrame.identifier == SUB_RPM_ACC) {
      RPM = (rxFrame.data[2] * 256 + rxFrame.data[3]) / 24;
      rpmChanged = true;
    }
  }

  if (millis() - lastUpdate > minUpdate) {
    lastUpdate = millis();
    if (!display.displayBusy()) {
      updateDisplay();
    }
  }
}