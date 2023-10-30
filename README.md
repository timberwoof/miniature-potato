# miniature-potato
## Arduino circuit and code for a simple transistor checker

A transistor curve tracer will tell you a transistor's electrical parameters
if you know which pin to connect to which test lead. Usually you can figure
this out from reading its type number and looking up its specs, but what fun is that? 
If you have a mysterysistor with no markings, there are six different ways to 
connect it, times two for whether it's PNP or NPN. This project is a 
needlessly complex interface and relays to tell you within seconds
whether a transistor is NPN, PNP, or BAD. 

## Parts List
- Arduino Uno R3
- LCD/Buttons shield (HiLetgo 1602 LCD Keypad Shield)
- breakout board (Prototype Screw Terminal Block Shield Board Kit MEGA-2560 R31 for Arduino)
- PCF8575 I2C interface (ACEIRMC 3pcs PCF8575 IO Expander Board)
- 8-relay board (8CH Relay Module)
- perf board or breadboard
- 12 V voltage regulator
- 5 V voltage regulator
- hookup wire
- three clippy leads
- 4 kOhm resistor
- 1 kOhm resistor

## Theory of Operation
The quick-and dirty check of a transistor with an Ohm-meter is as follows: 
Connect the leads of the Ohm-meter (or DVM in resistance mode) to every possible 
pair of leads on the transistor. Write down or remember which pairs conduct in 
which direction. The pair that shows no conductivity in either direction is 
probably the emitter and collector; the other pin is probably the base. The direction
of conductivity from the base to the other two will tell you whether it's NPN or PNP.
Any pair of leads that conducts in both directions is indicates a short. More than two
pairs of open leads indicates an open. 

The Miniature Potato applies these tests and reports the results in seconds. 

The Arduino is the brains of the test fixture. It runs the program, sets the relays 
via the I2c interface, and measures voltage drop via an analog input. 

The LCD/Buttons shield provides a nice way to output the results, but uses up digital 
outputs that could have been used for relays. One could use the serial termainl instead.

Thus the PCF8575, using the I2C interface, makes it wasy to control a bunch of relays. 

A dead-simple voltage divider consisting of a 4 kOhm and a 1 kOhm resistor provide the 1v
test voltage. 

This design uses relays instead of opto-isolators because relays accomplish extremely
high impedance "off" conditions in ways that no transistor can. I did not ant to connect 
all three leads of the transistor under test to a complicated circuit of transistors
that could affect the measurements of conductivity across the leads. 

Six of the relays inflict test voltage across the legs of the transistors. Three connect
test leads to ground; three connect test leads to +1 V. Two connect to test lead A, 
two to lead B, and two to lead C. The relays get activated two at a time so that 
one lead is +1 V and one lead is ground. The +1V is also connected to the Arduino's A1 line. 
Thus at any time during the test sequence, one lead is +, one lead is -, and the other
is not connected. If the transistor conducts, then the + gets pulled down to roughly half a volt, 
and that gets read by the AD converter. 1V is probably safe for any trasistor. A lower
voltage could be used, but that would require a little amplifier to raise the signal
voltage to something the AD input can reliably read. 

The Arduino checks the number of opens and shorts for an inial pass/fail check. 
It then determines whether the presumbaly good transistor is PNP or NPN. 

## Construction Notes
### Breakout Board
The one I got is for the Arduino Mega, but it works with the Uno R3. 
However, the markings for the I2C interface are wrong when using an Uno. 

### PCF8575
Connect Vcc on the end to Vcc on the side. 
Connect bits in any convenient way to the relay board. 
The bit assignments are set up in the sketch. 

### Relay Board
Connect Vcc to +5 V
Connect JDVcc to +12 V

## Operation
- Open the sketch and download it to your Arduino. 
- Connect the transistor to the clippy leads in any combination. 
- Press a button on the LCD shield. 
- The relays will click through the equence. 
- The LCD will diagnose the transistor. 
- It will report shorts and unusual opens. 
- It will report PNP or NPN and tell which lead is connected to the base. 

Now you can connect the transistor to your curve tracer with a 50/50 chance
of getting emitter and collector right. A ridiculously short set of curves
probably means you reversed E and C. 

## Future Work
Proper circuit diagrams are forthcoming. 

Perhaps one of the leads of the Miniature Potato could make use of the other two relays
and some resistors to act as a primitie curve tracer. Having identified the base, you'd
connect the specific base lead to that and run the trace test. It would then try to 
determine the Hfe or suggest that E and C are reversed. 

## IMG_4331.jpeg
### Arduino, LCD/Buttons shield, breakout board

### PCF8575
On a breadboard. I suppose I could have solderd wires or plugged a ribbon cable to the relay board. 
There's more than one way to do it. 

### Relay Board
Six of the relays are used. You can see the red wires from the voltage regulators on the perf board, 
and the ground wires. 

### Perf Board
At the bottom are the clippy leads A (fancy red), B (brass), C (fancy black). 
I used telephone twisted-pare wire I had lying around to do the interconnects 
between clippy leads and relays. The relays are wired so the make the connection
when activated. 

At the top are the two voltage regulators. The white wire comes from an old
wall-wart supply. The LEDs and their current-limiting resistors are optional. 
The red and black clippy leads go to my DVM. They are across the 1 Ohm resistor
that goes in parallel with the transistor under test, The other two resistors
are 2 kOhm resistors in series because I had them and didn't want to look for 
a 4 kOhm resistor. 

### Transistor under Test
Leads A and B are connected to the outside legs; lead B is clipped to the heat sink. 
From the display on the Arduino we can see that it's a PNP and the red clipply lead
is connected to the base. ~~It is extremely likely that~~ the heat sink and center
conductor are the collector. 
