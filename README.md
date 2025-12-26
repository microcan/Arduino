# Arduino

Arduino sketches, mainly M5Stack apps for reading and displaying Subaru CAN bus info

## prototypes

Test and developmentcode, mainly around running the M4Stack core display, reading data from the CAN bus and using GPIO to run an LED shift light strip

## subaru_brz

Arduine sketches for M5 stack for Subaru BRZ

### brz_console

This is the sketch for the M5Stack core used as a center console guage readout in the Subaru BRZ.  The guage values are mainly read from CAN bus, except Oil Pressure, where and aftermarket pressure sender is powered from the M5Stack and read using the M5Stack Voltmeter accessory.  Additionally, an old LED strip is controlled by GPIO to provide progressive shift lights with flashing at high RPM.

