//
// Created by Andri Yadi on 8/1/16.
//

#include "ESPectroBase_GpioEx.h"

ESPectroBase_GpioEx::ESPectroBase_GpioEx(uint8_t address):
SX1508(address)
{
    i2cDeviceAddress_ = address;
}

ESPectroBase_GpioEx::~ESPectroBase_GpioEx() {

}

byte ESPectroBase_GpioEx::begin() {
    pinMode(ESPECTRO_BASE_GPIOEX_LED_PIN, OUTPUT);

    clock(INTERNAL_CLOCK_2MHZ, 4);
    return SX1508::begin(i2cDeviceAddress_);
}

void ESPectroBase_GpioEx::turnOnLED() {
    this->digitalWrite(ESPECTRO_BASE_GPIOEX_LED_PIN, LOW);
}

void ESPectroBase_GpioEx::turnOffLED() {
    this->digitalWrite(ESPECTRO_BASE_GPIOEX_LED_PIN, HIGH);
}

void ESPectroBase_GpioEx::blinkLED(unsigned long tOn, unsigned long tOff, byte onIntensity, byte offIntensity) {
    blink(ESPECTRO_BASE_GPIOEX_LED_PIN, tOn, tOff, onIntensity, offIntensity);
}
