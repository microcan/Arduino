# Arduino

Arduino code mainly for M5Stack hardware.  A library to control the AutoSportsLabs TireX ans ShiftX3 hardware.  A library to draw scaleabe analog gauges and tire temperature patches on M5 displays.  Prototypes and programs to run data displays in a Subaru BRZ.

## libraries

I've cached all the libraries in my repo, but the actual developments ones are:

### libraries/Microcan

Some common functionality for the following libraries

### libraries/MicrocanAsl

Classes to configure, set and read data from:
1. The https://wiki.autosportlabs.com/ShiftX3 hardware
2. The https://wiki.autosportlabs.com/TireX hardware
This library should only depend on ESP32-TWAI-CAN so you should be able to run it on any ESP hardware.  Some of the examples also use the MicrocanGauges library to display the data on M5Stack hardware.

### libraries/MicrocanGauges

Classes to render scaleable anolog gauges and tire temperature patched on the display of M5Stack hardware.  Some of the examples show lots of gauges and tire patches, so are geared at something like the M5Tab5 that has lots of pixels.

## programs

Miscellaneous M5Stack programs.

### countdown_repeat_timer

A simple M5Dial program to help with your physio exercises.  Counts down, beeps, and counts your reps.  Turn the encoder ring to set the timer, click the ring to start or restart the timer.  

## prototypes

Test and development code, mainly around running the M5Stack core display, reading data from the CAN bus and using GPIO to run an LED shift light strip

## subaru_brz

Arduino sketches for M5 stack for Subaru BRZ

### brz_console

Old - This is the sketch for the M5Stack core used as a center console gauge readout in the Subaru BRZ.  The gauge values are mainly read from CAN bus, except Oil Pressure, were an aftermarket pressure sender is powered from the M5Stack and read using the M5Stack Voltmeter accessory.  Additionally, an old LED strip is controlled by GPIO to provide progressive shift lights with flashing at high RPM.

