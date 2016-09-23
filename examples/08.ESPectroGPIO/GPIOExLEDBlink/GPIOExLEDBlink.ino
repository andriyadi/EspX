/*
  Blink on-board LED on ESPectro Base drove by GPIO expander using ESPectro library

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

    //on for 1000, off for 2000
    gpioEx.blinkLED(1000, 2000); 
}

// the loop function runs over and over again forever
void loop() {
    //do nothing here
}
