//
// Created by Andri Yadi on 8/25/16.
//

#ifndef XBOARD_ESPECTRO_CONSTANTS_H_H
#define XBOARD_ESPECTRO_CONSTANTS_H_H

#define DEBUG_SERIAL(...)   Serial.printf( __VA_ARGS__ )

#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL(...)
#else
#define DEBUG_SERIAL_LN     Serial.println()
#endif

//Constants
#define XBOARD_LED_PIN 15

#define XBOARD_BUTTON_PIN 2
#define XBOARD_BUTTON_DEBOUNCE_DURATION_MS      50
#define XBOARD_BUTTON_PRESS_DURATION_MS         100
#define XBOARD_BUTTON_LONG_PRESS_DURATION_MS    700

#define SETTING_DEFAULT_AP_NAME             "ESPectro_000"
#define SETTING_DEFAULT_AP_PASS             "11223344"

#define SETTING_FORCE_INIT                  false

#define WIFI_CHECK_CONNECTION_INTERVAL      5000

#endif //XBOARD_ESPECTRO_CONSTANTS_H_H
