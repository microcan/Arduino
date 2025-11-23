#include <ESP32-TWAI-CAN.hpp>
#include <M5GFX.h>
M5GFX display;

#define CAN_TX 17
#define CAN_RX 16

M5Canvas canvas(&display);

CanFrame rxFrame;

// subaru CAN IDs
#define SUB_OIL_COOL 864
#define SUB_RPM_ACC 320
#define ODB_MSG 2024

void setup() {
  int baud = 500;

  display.begin();

  canvas.setColorDepth(1);  // mono color
  canvas.createSprite(display.width(), display.height());
  canvas.setTextSize((float)canvas.width() / 160);
  canvas.setTextScroll(true);
  // Setup serial for debbuging.
  Serial.begin(115200);
  Serial.println("Hello world");

  canvas.printf("CAN test V0.0\r\n");
  canvas.pushSprite(0, 0);

  // or override everything in one command;
  // It is also safe to use .begin() without .end() as it calls it internally

  if (ESP32Can.begin(ESP32Can.convertSpeed(baud), CAN_TX, CAN_RX, 10, 10)) {
    Serial.println("CAN bus started!");
    canvas.printf("CAN bus started %dk \r\n  TX pin %d RX pin %d\r\n", baud, CAN_TX, CAN_RX);
    canvas.pushSprite(0, 0);
  } else {
    Serial.println("CAN bus failed!");
    canvas.printf("CAN bus failed!\r\n");
    canvas.pushSprite(0, 0);
  }
}

void loop() {
  static uint32_t lastStamp = 0;
  uint32_t currentStamp = millis();

  if (currentStamp - lastStamp > 30000) {
    lastStamp = currentStamp;
    Serial.println("In loop");
    canvas.printf("%04d:%s\r\n", currentStamp / 1000, "still alive");
    canvas.pushSprite(0, 0);

    //uint8_t d = 64;
    //d = d<<2;
    //int rpm = d * 64 + 0;
    //canvas.printf("SUB_RPM: %4d \r\n", rpm);
    //canvas.pushSprite(0, 0);
  }

  if (ESP32Can.readFrame(rxFrame, 500)) {
    // Comment out if too many frames
    //Serial.printf("Received frame: %03X  \r\n", rxFrame.identifier);
    //canvas.printf("Received frame: %03X  \r\n", rxFrame.identifier);
    //canvas.pushSprite(0, 0);
    //if (rxFrame.identifier == SUB_OIL_COOL) {
    //  Serial.printf("Coolant temp: %3d°C \r\n", rxFrame.data[3] - 40);
    //  canvas.printf("Coolant temp: %3d°C \r\n", rxFrame.data[3] - 40);
    //  canvas.printf("Oil temp:     %3d°C \r\n", rxFrame.data[2] - 40);
    //  canvas.pushSprite(0, 0);
    //}
    if (rxFrame.identifier == SUB_RPM_ACC) {
      uint8_t hiByte = rxFrame.data[3] << 3;
      int rpm = (hiByte * 32) + rxFrame.data[2];
      canvas.printf("SUB_RPM: %4d %3d %3d\r\n", rpm, hiByte, rxFrame.data[2]);
      canvas.pushSprite(0, 0);
    }
    //if (rxFrame.identifier == ODB_MSG) {
    //  canvas.printf("ODB_RPM: %3d %3d %d %d\r\n", rxFrame.data[0], rxFrame.data[1], rxFrame.data[2], rxFrame.data[3]);
    //  canvas.pushSprite(0, 0);
    //}
  }
}