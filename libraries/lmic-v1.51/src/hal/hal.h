/*******************************************************************************
 * Copyright (c) 2015 Matthijs Kooijman
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * This the HAL to run LMIC on top of the Arduino environment.
 *******************************************************************************/
#ifndef _hal_hal_h_
#define _hal_hal_h_

static const int NUM_DIO = 3;

struct lmic_pinmap {
    u1_t nss;
    u1_t rxtx;					// No used by ESP8266 and Aduino
    u1_t rst;
    u1_t dio[NUM_DIO];			// No all a needed bij ESP8266 and Arduino
};

// Declared here, to be defined an initialized by the application
extern lmic_pinmap pins;

#endif // _hal_hal_h_
