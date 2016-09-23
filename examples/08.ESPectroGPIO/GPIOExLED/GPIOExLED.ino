/*
  turn-on on-board LED on ESPectro Base drove by GPIO expander using ESPectro library

  Created by 24 Sep 2016
  by Andri Yadi
*/

#include <ESPectroBase_GpioEx.h>

ESPectroBase_GpioEx gpioEx;

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    //Wait Serial to be ready
    while (!Serial);

    //Should explicitely call this to activate GPIO
    if (gpioEx.begin()) {
        Serial.println(F("GPIO EX WORKS!"));
    }
    else {
        Serial.println(F("GPIO EX DOOM!"));
    }
}

// the loop function runs over and over again forever
void loop() {
    gpioEx.turnOnLED();
    delay(1000);
    gpioEx.turnOffLED();
    delay(2000);
}
