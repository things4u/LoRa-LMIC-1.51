/*******************************************************************************
 * Jan 2016, Modified by Maarten Westenberg to run on ESP8266. Run's on
 * - Wemos D1-mini, with RFM95 and Bee shield
 *
 * Copyright (c) 2015 Matthijs Kooijman, modifications Thomas Telkamp
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 *
 * This the HAL to run LMIC on top of the ESP8266 environment.
 *******************************************************************************/

 // Make hal.cpp hardware dependent
 // NOTE, need to add defines for every architecture supported.
 // So rn2483 might be next candidate...
#if defined(__AVR__)
#include <Arduino.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP.h>
// #include <Base64.h>
#elif defined(__MKL26Z64__)
#include <Arduino.h>
#else
#error unrecognized architecture
#endif

#include <SPI.h>
#include "../lmic.h"
#include "hal.h"

// -----------------------------------------------------------------------------
// I/O

static void hal_io_init () {
    pinMode(pins.nss, OUTPUT);
    pinMode(pins.rxtx, OUTPUT);			// NOT used on ESP8266
    pinMode(pins.rst, OUTPUT);
    pinMode(pins.dio[0], INPUT);
    pinMode(pins.dio[1], INPUT);
    pinMode(pins.dio[2], INPUT);
}

// val == 1  => tx 1
void hal_pin_rxtx (u1_t val) {
    digitalWrite(pins.rxtx, val);
}

// set radio RST pin to given value (or keep floating!)
void hal_pin_rst (u1_t val) {
    if(val == 0 || val == 1) { // drive pin
        pinMode(pins.rst, OUTPUT);
        digitalWrite(pins.rst, val);
    } else { // keep pin floating
        pinMode(pins.rst, INPUT);
    }
}

static bool dio_states[NUM_DIO] = {0};

static void hal_io_check() {
    uint8_t i;
    for (i = 0; i < NUM_DIO; ++i) {
        if (dio_states[i] != digitalRead(pins.dio[i])) {
            dio_states[i] = !dio_states[i];
            if (dio_states[i])
                radio_irq_handler(i);
        }
    }
}

// -----------------------------------------------------------------------------
// SPI

static const SPISettings settings(10E6, MSBFIRST, SPI_MODE0);

static void hal_spi_init () {
    SPI.begin();
}

void hal_pin_nss (u1_t val) {
    if (!val)
        SPI.beginTransaction(settings);
    else
        SPI.endTransaction();

    digitalWrite(pins.nss, val);
}

// perform SPI transaction with radio
u1_t hal_spi (u1_t out) {
    u1_t res = SPI.transfer(out);
    return res;
}

// -----------------------------------------------------------------------------
// TIME

// Keep track of overflow of micros()
u4_t lastmicros=0;
u8_t addticks=0;

static void hal_time_init () {
    lastmicros=0;
    addticks=0;
}

u8_t hal_ticks () {
    // LMIC requires ticks to be 15.5μs - 100 μs long
    // Check for overflow of micros()
    if ( micros()  < lastmicros ) {
        addticks += (u8_t)4294967296 / US_PER_OSTICK;
    }
    lastmicros = micros();
    return ((u8_t)micros() / US_PER_OSTICK) + addticks;
}

// Returns the number of ticks until time. 
static u4_t delta_time(u8_t time) {
      u8_t t = hal_ticks( );
      s4_t d = time - t;
      if (d<=1) { return 0; }
      else {
        return (u4_t)(time - hal_ticks());
      }
}

void hal_waitUntil (u8_t time) {
    u4_t delta = delta_time(time);
    // From delayMicroseconds docs: Currently, the largest value that
    // will produce an accurate delay is 16383.
    while (delta > (16000 / US_PER_OSTICK)) {
        delay(16);
        delta -= (16000 / US_PER_OSTICK);
    }
    if (delta > 0)
        delayMicroseconds(delta * US_PER_OSTICK);
}

// check and rewind for target time
u1_t hal_checkTimer (u8_t time) {
    // No need to schedule wakeup, since we're not sleeping
    return delta_time(time) <= 0;
}

static uint8_t irqlevel = 0;

void hal_disableIRQs () {
    cli();
    irqlevel++;
}

void hal_enableIRQs () {
    if(--irqlevel == 0) {
        sei();

        // Instead of using proper interrupts (which are a bit tricky
        // and/or not available on all pins on AVR), just poll the pin
        // values. Since os_runloop disables and re-enables interrupts,
        // putting this here makes sure we check at least once every
        // loop.
        //
        // As an additional bonus, this prevents the can of worms that
        // we would otherwise get for running SPI transfers inside ISRs
        hal_io_check();
    }
}

void hal_sleep () {
    // Not implemented
}

// -----------------------------------------------------------------------------
// For ESP8266: If the radio is not connected well, we will probably get a
// wdt (watchdog) problem in this routine.
//
void hal_init () {

    // configure radio I/O and interrupt handler
    hal_io_init();
    // configure radio SPI
    hal_spi_init();
    // configure timer and interrupt handler
    hal_time_init();
}

void hal_failed (const char *file, u2_t line) {
    Serial.println("FAILURE");
    Serial.print(file);
    Serial.print(':');
    Serial.println(line);
    Serial.flush();
    hal_disableIRQs();
    while(1);
}

void debug(u4_t n) {Serial.println(n); Serial.flush();}
void debug_str(const char *s) {Serial.println(s); Serial.flush();}
