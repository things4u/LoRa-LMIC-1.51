/*******************************************************************************
 * Copytight (c) 2016 Maarten Westenberg based on work of
 * Thomas Telkamp and Matthijs Kooijman porting the LMIC stack to Arduino IDE
 * and Gerben den Hartog for his tiny stack implementation with the AES library
 * that we used in the LMIC stack.
 *
 * Permission is hereby granted, free of charge, to anyone
 * obtaining a copy of this document and accompanying files,
 * to do whatever they want with them without any restriction,
 * including, but not limited to, copying, modification and redistribution.
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 *
 * This example sends a valid LoRaWAN packet with sensor values read.
 * If no sensor is connected the payload is '{"Hello":"World"}', that
 * will be processed by The Things Network server.
 *
 * Note: LoRaWAN per sub-band duty-cycle limitation is enforced (1% in g1, 
 *  0.1% in g2). 
 *
 * Change DevAddr to a unique address for your node 
 * See http://thethingsnetwork.org/wiki/AddressSpace
 *
 * Do not forget to define the radio type correctly in config.h, default is:
 *   #define CFG_sx1272_radio 1
 * for SX1272 and RFM92, but change to:
 *   #define CFG_sx1276_radio 1
 * for SX1276 and RFM95.
 *
 *******************************************************************************/

#define WAIT_SECS 120


#if defined(__AVR__)
#include <avr/pgmspace.h>
#include <Arduino.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP.h>
#elif defined(__MKL26Z64__)
#include <Arduino.h>
#else
#error Unknown architecture in aes.cpp
#endif

#include "lmic.h"
#include "hal/hal.h"
#include <SPI.h>

//---------------------------------------------------------
// Sensor declarations
//---------------------------------------------------------


// Frame Counter
int count=0;

// LoRaWAN Application identifier (AppEUI)
// Not used in this example
static const u1_t APPEUI[8] PROGMEM = { 0x02, 0x00, 0x00, 0x00, 0x00, 0xEE, 0xFF, 0xC0 };

// LoRaWAN DevEUI, unique device ID (LSBF)
// Not used in this example
static const u1_t DEVEUI[8] PROGMEM  = { 0x42, 0x42, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

// LoRaWAN NwkSKey, network session key 
// Use this key for The Things Network
unsigned char NwkSkey[16] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// LoRaWAN AppSKey, application session key
// Use this key to get your data decrypted by The Things Network
unsigned char AppSkey[16] =	{ 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace

#define msbf4_read(p)   (u4_t)((u4_t)(p)[0]<<24 | (u4_t)(p)[1]<<16 | (p)[2]<<8 | (p)[3])
unsigned char DevAddr[4] = { 0x01, 0x01, 0x01, 0x01 };


// ----------------------------------------------------------------------------
// APPLICATION CALLBACKS
// ----------------------------------------------------------------------------


// provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
    memcpy(buf, DEVEUI, 8);
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) {
    memcpy(buf, NwkSkey, 16);
}

int debug=1;
uint8_t mydata[64];
static osjob_t sendjob;

// Pin mapping
// These settings should be set to the GPIO pins of the device
// you want to run the LMIC stack on.
//
lmic_pinmap pins = {
  .nss = 15,			// Connected to pin D
  .rxtx = 2, 			// For placeholder only, Do not connected on RFM92/RFM95 
  .rst = 0,  			// Needed on RFM92/RFM95? (probably not) D0/GPIO16
  .dio = {5, 4, 3},		// Specify pin numbers for DIO0, 1, 2
						// connected to D5, D4, D3 
};

void onEvent (ev_t ev) {
    //debug_event(ev);

    switch(ev) {
      // scheduled data sent (optionally data received)
      // note: this includes the receive window!
      case EV_TXCOMPLETE:
          // use this event to keep track of actual transmissions
          Serial.print("EV_TXCOMPLETE, time: ");
          Serial.println(millis() / 1000);
          if(LMIC.dataLen) { // data received in rx slot after tx
              //debug_buf(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
              Serial.println("Data Received");
          }
          break;
       default:
          break;
    }
}

void do_send(osjob_t* j){
	  delay(1);													// XXX delay is added for Serial
      Serial.print("Time: ");
      Serial.println(millis() / 1000);
      // Show TX channel (channel numbers are local to LMIC)
      Serial.print("Send, txCnhl: ");
      Serial.println(LMIC.txChnl);
      Serial.print("Opmode check: ");
      // Check if there is not a current TX/RX job running
    if (LMIC.opmode & (1 << 7)) {
      Serial.println("OP_TXRXPEND, not sending");
    } 
	else {
	

	  strcpy((char *) mydata,"{\"Hello\":\"World\"}");

	  Serial.print("ready to send: "); 
	  Serial.println((char *)mydata);
	  LMIC_setTxData2(1, mydata, strlen((char *)mydata), 0);
    }
    // Schedule a timed job to run at the given timestamp (absolute system time)
    os_setTimedCallback(j, os_getTime()+sec2osticks(WAIT_SECS), do_send);
         
}

// ====================================================================
// SETUP
//
void setup() {
  Serial.begin(115200);
  
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();
  // Set static session parameters. Instead of dynamically establishing a session 
  // by joining the network, precomputed session parameters are be provided.
  LMIC_setSession (0x1, msbf4_read(DevAddr), (uint8_t*)NwkSkey, (uint8_t*)AppSkey);
  // Disable data rate adaptation
  LMIC_setAdrMode(0);
  // Disable link check validation
  LMIC_setLinkCheckMode(0);
  // Disable beacon tracking
  LMIC_disableTracking ();
  // Stop listening for downstream data (periodical reception)
  LMIC_stopPingable();
  // Set data rate and transmit power (note: txpow seems to be ignored by the library)
  LMIC_setDrTxpow(DR_SF7,14);
  //
  //Serial.flush();
  Serial.print(F("A: ")); Serial.println(msbf4_read(DevAddr),HEX);
}

// ================================================================
// LOOP
//
void loop() {

	do_send(&sendjob);
	while(1) {
		os_runloop_once();
		yield();
	}
}

