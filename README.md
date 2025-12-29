# Arduino

Arduino sketches, mainly M5Stack apps for reading and displaying Subaru CAN bus info

## programs

Miscellaneous M5Stack programs

### countdown_repeat_timer

A simple M5Dial program to help with your physio exercises.  Counts down, beeps, and counts your reps.  Turn the encoder ring to set the timer, click the ring to start or restart the timer.  

## prototypes

Test and development code, mainly around running the M4Stack core display, reading data from the CAN bus and using GPIO to run an LED shift light strip

## subaru_brz

Arduino sketches for M5 stack for Subaru BRZ

### brz_console

This is the sketch for the M5Stack core used as a center console gauge readout in the Subaru BRZ.  The gauge values are mainly read from CAN bus, except Oil Pressure, were an aftermarket pressure sender is powered from the M5Stack and read using the M5Stack Voltmeter accessory.  Additionally, an old LED strip is controlled by GPIO to provide progressive shift lights with flashing at high RPM.

