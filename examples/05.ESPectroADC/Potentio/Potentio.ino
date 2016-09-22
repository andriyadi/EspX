/*
  Blink on-board Neopixel RGB LED using ESPectro library
  Turns on-board Neopixel RGB LED on for one second, then off for one second, repeatedly.

  Created by 22 Sep 2016
  by Andri Yadi
*/

#include <ESPectroBase.h>

ESPectroBase base;

// the setup function runs once when you press reset or power the board
void setup() {
  base.begin();
}

// the loop function runs over and over again forever
void loop() {
  base.analogRead(ESPECTRO_BASE_ADC_LDR_CHANNEL);
  delay(500);
}
