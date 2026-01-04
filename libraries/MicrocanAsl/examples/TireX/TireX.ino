#include <MicrocanAsl.h>

// These are good dip switches to set in PWRCAN for Core basic
// Core Basic
//#define CAN_TX 17
//#define CAN_RX 16

// On the Tab5, the same PWRCAN setting give you these pins
#define CAN_TX 6
#define CAN_RX 7

// CanManager coordinates CAN messages to and from ShiftX and TireX
CanManager ASL;

// 1, 2, 4 or 8 tire zones?  The library doesnt support 16 yet
int zones = 4;

void setup() {
  Serial.begin(9600);  
  Serial.println("Start of setup.");

  // Set some TireX preferences, zones and frequency
  ASL.TireX.SetPrefs(zones, 4);

  if (ASL.Connect(CAN_TX, CAN_RX)) {
    Serial.printf("CAN manager connected.\n");
  } else {
    Serial.printf("Error connecting CAN wrapper\n");
  }
}

unsigned long last = 0;

void loop() {
  // process all the CAN traffic
  ASL.Update();

  // print out the temps at 2Hz
  unsigned long now = millis();
  if (now - last > 500) {
    last = now;

    for (int i = 0; i < 4; i++) {
      Serial.printf(" %s: ", AlsTireLabel(i));
      for (int j = 0; j < zones; j++) {
        Serial.printf("%03d", ASL.TireX.Temps[i][j] / 2);
        if (j < (zones - 1)) Serial.printf(", ");
      }
    }
    Serial.printf("\n");
  }
}
