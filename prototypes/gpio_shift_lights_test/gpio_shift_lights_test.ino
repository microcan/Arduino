#include <M5Unified.h>

uint greenPin = 2;   // green
uint yellowPin = 5;  // yellow
uint redPin = 1;     // red
uint count = 0;
uint ticker = 0;

uint off = HIGH;
uint on = LOW;
uint flash = 192;

uint flasher = LOW;

uint red = off;
uint yellow = off;
uint green = off;

uint isOn(uint state) {
  if (state == flash) {
    return flasher;
  } else {
    return state;
  }
}

void setLights() {
  digitalWrite(redPin, isOn(red));
  digitalWrite(greenPin, isOn(green));
  digitalWrite(yellowPin, isOn(yellow));
}

void setup() {
  M5.begin();  // Initialize M5Stack

  // Set the LED pin as an output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);

  setLights();
}



void loop() {
  // put your main code here, to run repeatedly:
  delay(50);

  if (flasher == LOW) {
    flasher = HIGH;
  } else {
    flasher = LOW;
  }

  setLights();

  ticker++;
  if (ticker == 10) {
    if (count == 0) {
      green = on;
      yellow = off;
      red = off;
    } else if (count == 1) {
      green = off;
      yellow = on;
      red = off;
    } else if (count == 2) {
      green = off;
      yellow = off;
      red = on;
    } else if (count == 3) {
      green = flash;
      yellow = flash;
      red = on;
    } else if (count == 4) {
      green = off;
      yellow = off;
      red = off;
    }
    ticker = 0;
    count++;
    if (count > 5) {
      count = 0;
    }
  }
}
