/*
  Blink on-board Neopixel RGB LED using ESPectro library
  Turns on-board Neopixel RGB LED on for one second, then off for one second, repeatedly.

  Created by 22 Sep 2016
  by Andri Yadi
*/

#include <ESPectro.h>
#include <ESPectroBase.h>

ESPectro board;
ESPectroBase base;

int lastAdcLdr = 0;
const int LED_THRESHOLD = 500;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  //Wait Serial to be ready
  while(!Serial);
  
  base.begin();
}

// the loop function runs over and over again forever
void loop() {
  int ldr = base.analogRead(ESPECTRO_BASE_ADC_LDR_CHANNEL);
  if (ldr != -1 && abs(ldr - lastAdcLdr) > 10) {
      lastAdcLdr = ldr;
      Serial.printf("Pot value: %d\r\n", ldr);

      if (ldr < LED_THRESHOLD) {
        board.turnOnLED();
      }
      else {
        board.turnOffLED();
      }
  }
  
  delay(500);
}
