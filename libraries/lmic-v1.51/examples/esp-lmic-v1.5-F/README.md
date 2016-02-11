LMiC 1.5 port to ESP8266 with Temperature Sensor
================================================
(c) M. Westenberg, jan 2016 

This sketch implement an temperature sensor for LoRa network.
It contains the full LoRa stack on an ESP8266 (based on LMIC-1.5 port) with a
temperature sensor (DS18B20 Dallas/OneWire).

In this sketch sensor is connected to pin D4

The sketch NEEDS the lmic-2.5 libraries in the libraries directory. 
Make sure you download these to your system as well.

AES Encryption library
----------------------

Also, the AES libraries has changed from the one that is shipped ith the IBM
LMIC sotware: Tat library uses huge static arrays which make it impossible to
run the software on a regular Arduino board. 

The smaller libray that is now integrated in the software is the same as used by
the tiny codebase of Gerben den Hartog (Ideetron). The interfaces have been adapted 
and the AES library runs happily.

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

