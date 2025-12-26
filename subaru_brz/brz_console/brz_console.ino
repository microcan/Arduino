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

// LED pin numbers
#define GREEN_PIN 2
#define YELLOW_PIN 5
#define RED_PIN 1

#define DISPLAY_UPDATE 200
#define FLASH_UPDATE 50

#define FLASH 192

M5GFX display;
ADS1115 meter;

float g_voltRes = 0.0;
float g_voltCal = 0.0;

bool g_canOk = false;

int g_oilTemp = 888;
float g_oilPress = 88.8;
int g_waterTemp = 888;
int g_rpm = 8888;

int g_oilPeak = 888;
int g_waterPeak = 888;

int g_screenW = 0;
int g_screenH = 0;

bool g_oilTempChanged = true;
bool g_oilPressChanged = true;
bool g_waterChanged = true;
bool g_rpmChanged = true;

bool g_oilPeakChanged = true;
bool g_waterPeakChanged = true;

//Oil pressure sender calibration
float g_vMin = 0.47;
float g_pMin = 0.0;
float g_vMax = 5.0;
float g_pMax = 10.0;

uint32_t g_lastDisplayUpdate = 0;
uint32_t g_lastFlashUpdate = 0;

uint g_flasher = HIGH;

int g_inset = 70;
int g_h72 = 80;

// flash LEDs by returning isON && FLASH
uint isOn(uint state) {
  if (state == FLASH) {
    return g_flasher;
  } else {
    return state;
  }
}

// set the lights on or off, with on potentially meaning flashing
void setLights(uint g, uint y, uint r) {
  digitalWrite(RED_PIN, isOn(r));
  digitalWrite(GREEN_PIN, isOn(g));
  digitalWrite(YELLOW_PIN, isOn(y));
}

void updateDisplay() {
  uint16_t color;
  bool draw = false;
  if (g_oilTempChanged || g_waterChanged || g_rpmChanged || g_oilPressChanged || g_oilPeakChanged || g_waterPeakChanged) {
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
    display.fillRect(g_inset, 0, g_screenW - g_inset, g_h72, color);
    display.setFont(&fonts::DejaVu72);
    display.drawNumber(g_oilTemp, g_screenW, 4);
  }

  if (g_waterChanged) {
    g_waterChanged = false;
    if (g_waterTemp < 70) {
      color = TFT_BLUE;
    } else if (g_waterTemp > 110) {
      color = TFT_RED;
    } else {
      color = TFT_BLACK;
    }
    display.setTextColor(TFT_WHITE, color);
    display.fillRect(g_inset, g_h72, g_screenW - g_inset, g_h72, color);
    display.setFont(&fonts::DejaVu72);
    display.drawNumber(g_waterTemp, g_screenW, g_h72 + 4);
  }

  if (g_oilPressChanged) {
    g_oilPressChanged = false;
    if (g_oilPress < 0.5) {
      color = TFT_RED;
    } else {
      color = TFT_BLACK;
    }
    display.setTextColor(TFT_WHITE, color);
    display.fillRect(g_inset, 2 * g_h72, g_screenW - g_inset, g_h72, color);
    display.setFont(&fonts::DejaVu72);
    display.drawFloat(g_oilPress, 1, g_screenW, 2 * g_h72 + 4);
  }

  if (g_rpmChanged) {
    g_rpmChanged = false;
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.fillRect(g_inset, 3 * g_h72, g_screenW - g_inset, 40, TFT_BLACK);
    display.setFont(&fonts::DejaVu40);
    display.drawNumber(g_rpm, g_screenW, 3 * g_h72 + 3);
  }

  if (g_oilPeakChanged) {
    g_oilPeakChanged = false;
    if (g_oilPeak < 70) {
      color = TFT_BLUE;
    } else if (g_oilPeak > 110) {
      color = TFT_RED;
    } else {
      color = TFT_BLACK;
    }
    display.setTextColor(TFT_WHITE, color);
    display.fillRect(40, g_screenH - 38, g_screenW / 2 - 40, 38, color);
    display.setFont(&fonts::DejaVu40);
    display.drawNumber(g_oilPeak, g_screenW / 2, g_screenH - 36);
  }

  if (g_waterPeakChanged) {
    g_waterPeakChanged = false;
    if (g_waterPeak < 70) {
      color = TFT_BLUE;
    } else if (g_waterPeak > 110) {
      color = TFT_RED;
    } else {
      color = TFT_BLACK;
    }
    display.setTextColor(TFT_WHITE, color);
    display.fillRect(g_screenW / 2 + 40, g_screenH - 38, g_screenW / 2 - 40, 38, color);
    display.setFont(&fonts::DejaVu40);
    display.drawNumber(g_waterPeak, g_screenW, g_screenH - 36);
  }

  if (draw) {
    display.endWrite();
  }
}

void initDisplay() {
  display.startWrite();

  display.fillScreen(TFT_BLACK);
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setFont(&fonts::DejaVu18);
  display.setTextDatum(textdatum_t::top_left);

  display.drawString("Oil", 0, 4);
  display.drawString("Water", 0, g_h72 + 4);
  display.drawString("Oil P", 0, g_h72 * 2 + 4);
  display.drawString("RPM", 0, g_h72 * 3 + 3);
  display.drawString("OP", 0, g_screenH - 36);
  display.drawString("WP", g_screenW / 2 + 3, g_screenH - 36);

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

  if (ESP32Can.begin(ESP32Can.convertSpeed(baud), CAN_TX, CAN_RX, 10, 100)) {
    g_canOk = true;
  } else {
    g_canOk = false;
  }

  initDisplay();

  g_oilTemp = 0;
  g_oilPress = 0;
  g_waterTemp = 0;
  g_rpm = 0;
  g_oilPeak = 0;
  g_waterPeak = 0;
  g_oilTempChanged = true;
  g_oilPressChanged = true;
  g_waterChanged = true;
  g_rpmChanged = true;
  g_oilPeakChanged = true;
  g_waterPeakChanged = true;

  if (!meter.begin(&Wire, M5_UNIT_VMETER_I2C_ADDR, 21, 22, 400000U)) {
    delay(500);
    meter.begin(&Wire, M5_UNIT_VMETER_I2C_ADDR, 21, 22, 400000U);
  }

  meter.setEEPROMAddr(M5_UNIT_VMETER_EEPROM_I2C_ADDR);
  meter.setMode(ADS1115_MODE_SINGLESHOT);
  meter.setRate(ADS1115_RATE_8);
  meter.setGain(ADS1115_PGA_256);

  g_voltRes = meter.getCoefficient() / M5_UNIT_VMETER_PRESSURE_COEFFICIENT;
  g_voltCal = meter.getFactoryCalibration();

  // Set the LED pins as an output
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);

  // HIGH is OFF
  setLights(HIGH, HIGH, HIGH);
}

void loop() {
  CanFrame rxFrame;
  ulong now;
  int val;

  if (ESP32Can.readFrame(rxFrame, FLASH_UPDATE)) {
    //Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
    if (rxFrame.identifier == SUB_OIL_COOL) {
      // oil temp
      val = rxFrame.data[2] - 40;
      if (g_oilTemp != val)
      {
        g_oilTempChanged = true;
        g_oilTemp = val;
      }
      // water temp
      val = rxFrame.data[3] - 40;
      if (g_waterTemp != val)
      {
        g_waterChanged = true;
        g_waterTemp = val;
      }    
      if (g_oilTemp > g_oilPeak) {
        g_oilPeak = g_oilTemp;
        g_oilPeakChanged = true;
      }
      if (g_waterTemp > g_waterPeak) {
        g_waterPeak = g_waterTemp;
        g_waterPeakChanged = true;
      }
    }
    // RPM
    if (rxFrame.identifier == SUB_RPM_ACC) {
      uint8_t hiByte = rxFrame.data[3] << 3;
      val = (hiByte * 32) + rxFrame.data[2];
      if (g_rpm != val) {
        g_rpm = val;
        g_rpmChanged = true;
      }
    }
  }

  now = millis();

  if ((now - FLASH_UPDATE) > g_lastFlashUpdate) {
    g_lastFlashUpdate = now;
    if (g_flasher == HIGH) {
      g_flasher = LOW;
    } else {
      g_flasher = HIGH;
    }

    if (g_rpm <= 5000) {
      setLights(HIGH, HIGH, HIGH);
    } else if (g_rpm <= 6000) {
      setLights(LOW, HIGH, HIGH);
    } else if (g_rpm <= 6500) {
      setLights(HIGH, LOW, HIGH);
    } else if (g_rpm <= 7000) {
      setLights(HIGH, HIGH, LOW);
    } else {
      setLights(FLASH, FLASH, LOW);
    }
  }

  if (now - DISPLAY_UPDATE > g_lastDisplayUpdate) {
    g_lastDisplayUpdate = now;

    int16_t adc_raw = meter.getSingleConversion();
    float volt = adc_raw * g_voltRes * g_voltCal / 1000.0;
    float raw = (volt - g_vMin) / (g_vMax - g_vMin) * (g_pMax - g_pMin) + g_pMin;
    g_oilPress = 0.7 * g_oilPress + 0.3 * raw;
    //g_oilPress = volt;
    g_oilPressChanged = true;

    if (!display.displayBusy()) {
      updateDisplay();
    }
  }
}