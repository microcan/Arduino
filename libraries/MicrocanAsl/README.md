# MicrocanAsl

This is a library for controlling the https://wiki.autosportlabs.com/TireX and https://wiki.autosportlabs.com/ShiftX3 hardware from Arduino ESP32 hardware.  Some of the examples also use the MicrocanGauges library which is M5Stack specific.

# Getting started

Copy the libraries/Microcan, libraries/MicrocanAsl and optionally libraries/MicrocanGauges libraries into your libraries folder.  Restart the IDE andyou should see these three libraries in your libraries browser.

Examples are in the MicrocanAsl/examples folder and should also show up in the IDE examples menu.

1. TireX depends only on ESP32-TWAI-CAN and configures the TireX hardware, reads the temps and prints them to Serial.
2. ShiftX3 depends only on ESP32-TWAI-CAN  and runs the alerts, linear graph, 7-segment display and reads the buttons, and reports through Serial
3. M5TireXDisplay needs M5Stack hardware and displays the tire temperature patches on the display
4. M5TireXShiftX3Display also needs M5Stack hardware. The display is layed out for the M5Tab5 hardware, if you have a small display you will need to mess around with what you want to display.  This example simulated some vehicle sensor valuesand uses them to drive gauges and the ShiftX hardware, while also displaying tire temperature patches.
 