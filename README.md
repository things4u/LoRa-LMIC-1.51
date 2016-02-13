LMIC 1.51 port to ESP8266 and Arduino (Atmega 328)
=====================================================
(c) M. Westenberg, Feb 11, 2016 

This library implements the LoRaWAN stack for a LoRa network.
It contains the full LoRa stack and works on Teensy platform, ESP8266 and the Ardino Atmega328

The Library is a port of LMIC-1.5 LoRa library to Arduino/Atmega 328 and the ESP8266, 
and especially the AES functions are a melt of two existing libraries:

1. The LMIC-1.5 library of IBM which was ported to Arduino (Teensy) IDE by Thomas Telkamp 
   and Matthijs Kooijman 
2. The tiny implementation for Arduino made by Gerben den Hartog for Ideetron.


AES Encryption library
----------------------
After analysing the existing LMIC-1.5 code of IBM for AES encryption I came to the conslusion 
that the encryption part of LMIC-1.5 is far too complex and too large.
And as I did not fit in the Atmega 328 another AES library was ported to the environment. 
Of course there are several AES implementations available, but since I already had experience 
with the library of Ideetron, I decided to give that a try. The result should be that the complete 
LMIC stack would be running on a regular Atmega 328 based MCU and there still be some room left for 
sensor sketches.

The port was successful, and the sketch now fits on the Arduino Pro-Mini leaving some room for 
a DALLAS or I2C ibrary and sensor. Simpler interfaces making direct use of the GPIO pins
will probably fit as well

Still, after all optimizations the code is large for the small Arduino Nano or ProMini,
so there probably is a practical limit to what you can do there.

Conditional Compile
-------------------
The code uses #defines to make conditional compilations for regular Arduino's, the Teensy
family and the ESP8266 family

Do not forget to use the dedicated ESP8266 OneWire library (which is named OneWireESP8266
on this github site) instead of the standard OneWire library shipped with the Arduino IDE. 
At writing the sketches, the latter was not fully compatible with 64-bit MCU's.

Supported ESP8266 MCU's
------------------------
In principle the Library and the example sketch will work on every ESP8266, as until 
today all of my sketches work on NodeMCU 0.9 and NodeMCU 1.0 systems and lookalikes.
I do however have less favourable experience with a bare ESP-xx implementations.

The code has been tested on my Wemos D1-mini MCU, but more to follow.

Supported Arduino MCU's
-----------------------
Tested on my small Arduino ProMini it runs OK with an I2C temperature/humidity sensor
(I2C bus). However, make sure to remove ALL Serial.print statements when going in production,
the library does pass around a lot of data on the stack, and this will make your node less reliable

Notes
-----
At least in the first ESP8266 version no WiFi functions are switched on. Handling the WiFi
stack takes the MCU a great deal of time and may result in Watchdog resets, mostly
because the user applications run too long and the MCU cannot handle the WiFi stack 
anymore.

Solution is to keep function as short as possible, avoid unnecessary Serial communication,
do not loop in the loop() section, and insert some delay() statements to give way
to the system processing tasks.

Documentation
-------------
Some material has been put together to help build this node. 
You can find the documentation at http://things4u.github.io in hardware section

