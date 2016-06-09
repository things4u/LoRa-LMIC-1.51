#ifndef _lmic_config_h_
#define _lmic_config_h_

#define LOWPOWER 1
// In the original LMIC code, these config values were defined on the
// gcc commandline. Since Arduino does not allow easily modifying the
// compiler commandline, use this file instead. (MK)

#define CFG_eu868 1
//#define CFG_us915 1
//#define CFG_sx1272_radio 1
#define CFG_sx1276_radio 1

#if defined(__AVR__)
#define US_PER_OSTICK 30		// 50 works for Atmega 328/16MHz with little debug messaging. 30 seems to be working better

#elif defined(ARDUINO_ARCH_ESP8266)
#define US_PER_OSTICK 20		// To be determined

#elif defined(__MKL26Z64__) || defined(__MK20DX128__)
#define US_PER_OSTICK 20		// 35 works for mini-AES, 25 Works with original AES mode

#endif

#define OSTICKS_PER_SEC (1000000 / US_PER_OSTICK)

#endif // _lmic_config_h_
