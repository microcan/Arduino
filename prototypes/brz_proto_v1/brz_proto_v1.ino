#include <Arduino.h>
#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>
#include <M5_ADS1115.h>

// Voltmeter magic numbers
#define M5_UNIT_VMETER_I2C_ADDR 0x49
#define M5_UNIT_VMETER_EEPROM_I2C_ADDR 0x53
#define M5_UNIT_VMETER_PRESSURE_COEFFICIENT 0.015918958F

// Based on PWRCAN module DIP switch config
#define CAN_TX 17
#define CAN_RX 16

// subaru CAN IDs
#define SUB_OIL_COOL 864
#define SUB_RPM_ACC 320
#define ODB_RPM 2024

M5GFX display;
ADS1115 meter;

float g_voltRes = 0.0;
float g_voltCal = 0.0;

bool g_canOk = false;

int g_oilTemp = 888;
float g_oilPress = 88.8;
int g_waterTemp = 888;
int g_rpm = 8888;

int g_screenW = 0;
int g_screenH = 0;

bool g_oilTempChanged = true;
bool g_oilPressChanged = true;
bool g_waterChanged = true;
bool g_rpmChanged = true;

//Oil pressure guage calibration
float g_vMin = 0.47;
float g_pMin = 0.0;
float g_vMax = 5.0;
float g_pMax = 10.0;

uint32_t minUpdate = 200;
uint32_t lastUpdate = 0;

CanFrame rxFrame;

void updateDisplay() {
  uint16_t color;
  bool draw = false;
  if (g_oilTempChanged || g_waterChanged || g_rpmChanged || g_oilPressChanged) {
    draw = true;
    display.startWrite();
    display.setTextDatum(textdatum_t::top_right);
  }

  if (g_oilTempChanged) {
    g_oilTempChanged = false;
    if (g_oilTemp < 70) {
      color = TFT_BLUE;
    } else if (g_oilTemp > 110) {
      color = TFT_RED;
    } else {
      color = TFT_BLACK;
    }
    display.setTextColor(TFT_WHITE, color);
    display.fillRect(0, 20, g_screenW / 2, 62, color);
    display.setFont(&fonts::DejaVu56);
    display.drawNumber(g_oilTemp, g_screenW / 2, 24);
  }

  if (g_waterChanged) {
    if (g_waterTemp < 70) {
      color = TFT_BLUE;
    } else if (g_waterTemp > 110) {
      color = TFT_RED;
    } else {
      color = TFT_BLACK;
    }
    display.setTextColor(TFT_WHITE, color);
    g_waterChanged = false;
    display.fillRect(g_screenW / 2, 20, g_screenW / 2, 62, color);
    display.setFont(&fonts::DejaVu56);
    display.drawNumber(g_waterTemp, g_screenW, 24);
  }

  if (g_oilPressChanged) {
    if (g_oilPress < 0.5) {
      color = TFT_RED;
    } else {
      color = TFT_BLACK;
    }
    g_oilPressChanged = false;
    display.setTextColor(TFT_WHITE, color);
    display.fillRect(0, 116, g_screenW, 80, color);
    display.setFont(&fonts::DejaVu72);
    display.drawFloat(g_oilPress, 1, g_screenW - 30, 120);
  }

  if (g_rpmChanged) {
    g_rpmChanged = false;
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.fillRect(0, 225, g_screenW, 80, TFT_BLACK);
    display.setFont(&fonts::DejaVu72);
    display.drawNumber(g_rpm, g_screenW - 30, 225);
  }

  if (draw) {
    display.endWrite();
  }
}

void initDisplay() {
  display.startWrite();
  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setFont(&fonts::Font2);
  display.setTextDatum(textdatum_t::top_left);

  display.drawString("Oil Temp", 0, 0);
  display.drawString("Coolant Temp", g_screenW / 2 + 20, 0);
  display.drawString("Oil Press", 0, 96);
  display.drawString("RPM", 0, 205);

  display.endWrite();

  updateDisplay();
}

void setup() {
  int baud = 500;

  display.begin();
  display.setEpdMode(epd_mode_t::epd_fastest);
  if (display.width() > display.height()) {
    display.setRotation(display.getRotation() ^ 1);
  }

  g_screenW = display.width();
  g_screenH = display.height();

  if (ESP32Can.begin(ESP32Can.convertSpeed(baud), CAN_TX, CAN_RX, 10, 10)) {
    g_canOk = true;
  } else {
    g_canOk = false;
  }

  initDisplay();

  g_oilTemp = 0;
  g_oilPress = 0;
  g_waterTemp = 0;
  g_rpm = 0;
  g_oilTempChanged = true;
  g_oilPressChanged = true;
  g_waterChanged = true;
  g_rpmChanged = true;

  while (!meter.begin(&Wire, M5_UNIT_VMETER_I2C_ADDR, 21, 22, 400000U)) {
    Serial.println("Unit Vmeter Init Fail");
    delay(500);
  }
  meter.setEEPROMAddr(M5_UNIT_VMETER_EEPROM_I2C_ADDR);
  meter.setMode(ADS1115_MODE_SINGLESHOT);
  meter.setRate(ADS1115_RATE_8);
  meter.setGain(ADS1115_PGA_256);

  g_voltRes = meter.getCoefficient() / M5_UNIT_VMETER_PRESSURE_COEFFICIENT;
  g_voltCal = meter.getFactoryCalibration();
}

void loop() {
  if (ESP32Can.readFrame(rxFrame, 500)) {
    //Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
    if (rxFrame.identifier == SUB_OIL_COOL) {
      g_oilTemp = rxFrame.data[2] - 40;
      g_waterTemp = rxFrame.data[3] - 40;
      g_oilTempChanged = true;
      g_waterChanged = true;
    }
    if (rxFrame.identifier == SUB_RPM_ACC) {
      uint8_t hiByte = rxFrame.data[3] << 3;
      g_rpm = (hiByte * 32) + rxFrame.data[2];
      g_rpmChanged = true;
    }
  }

  if (millis() - lastUpdate > minUpdate) {
    lastUpdate = millis();

    int16_t adc_raw = meter.getSingleConversion();
    float volt = adc_raw * g_voltRes * g_voltCal / 1000.0;
    float raw = (volt - g_vMin) / (g_vMax - g_vMin) * (g_pMax - g_pMin) + g_pMin;
    g_oilPress = 0.9 * g_oilPress + 0.1 * raw;
    //g_oilPress = volt;
    g_oilPressChanged = true;

    if (!display.displayBusy()) {
      updateDisplay();
    }
  }
}