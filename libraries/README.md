LMIC 1.5 port to ESP8266 and Arduino (the normal one)
=====================================================
(c) M. Westenberg, Feb 2016 

This library implements the stack for a LoRa network.
It contains the full LoRa stack and works on Teensy platform, ESP8266 en the Ardino Atmega328

The Library is a port of two existing libraries:

1. The LMIC-1.5 library of IBM whih was ported to Arduino IDE by Thomas Telkamp 
   and Matthijs Kooijman 
2. The tiny implementation for Arduino made by Gerben den Hartog for Ideetron.


AES Encryption library
----------------------
After analysing the existing LMIC-1.5 code of IBM for AES encryption which is far too complex 
(and too large) and therefore another AES library was ported to the environment. 
Of course there are several available, but I alread had experience with the library of Ideetron.

Still, after all optimizations the code is large for the small Arduino Nano or ProMini,
so there probably is a practical limit to what you can do there.


Conditional Compile
-------------------
The code uses #defines to make conditional compilations for regular Arduino's, the Teensy
family and the ESP8266 family

Do not forget to use the dedicated ESP8266 OneWire library (which is named OneWireESP8266)
instead of the standard OneWire library shipped with the Arduino IDE. The latter is not
fully compatible with 64-bit MCU's.

Supported ESP8266 MCU's
------------------------
In principle the Library and the example sketch will work on every ESP8266, as until 
today all of my sketches work on NodeMCU 0.9 and NodeMCU 1.0 systems and lookalikes.
I do however have less favourable experience with bare ESP-xx implementations.

The sensor has been tested on my Wemos D1-mini MCU, but more to follow.

Supported Arduino MCU's
-----------------------
Tested on my small Arduino ProMini it runs OK with an I2C temperature/humidity sensor
(I2C bus)

Notes
-----
At least in the first version no WiFi functions are switched on. Handling the WiFi
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

