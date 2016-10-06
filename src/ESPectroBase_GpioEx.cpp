//
// Created by Andri Yadi on 8/1/16.
//

#include "ESPectroBase_GpioEx.h"

ESPectroBase_GpioEx::ESPectroBase_GpioEx(uint8_t address):
SX1508(address), i2cDeviceAddress_(address)
{
}

ESPectroBase_GpioEx::~ESPectroBase_GpioEx() {

}

byte ESPectroBase_GpioEx::begin(byte address, byte resetPin) {

    byte addr = (address != ESPECTRO_BASE_GPIOEX_ADDRESS)? address: i2cDeviceAddress_;
    byte res = SX1508::begin(addr, resetPin);
    if (res) {
        SX1508::pinMode(ESPECTRO_BASE_GPIOEX_LED_PIN, OUTPUT);
        clock(INTERNAL_CLOCK_2MHZ, 4);
    }

    return res;
}

void ESPectroBase_GpioEx::turnOnLED() {
    SX1508::digitalWrite(ESPECTRO_BASE_GPIOEX_LED_PIN, LOW);
}

void ESPectroBase_GpioEx::turnOffLED() {
    SX1508::digitalWrite(ESPECTRO_BASE_GPIOEX_LED_PIN, HIGH);
}

void ESPectroBase_GpioEx::blinkLED(unsigned long tOn, unsigned long tOff, byte onIntensity, byte offIntensity) {
    blink(ESPECTRO_BASE_GPIOEX_LED_PIN, tOn, tOff, onIntensity, offIntensity);
}
