//
// Created by Andri Yadi on 8/1/16.
//

#include "ESPectro_GpioEx.h"

ESPectro_GpioEx::ESPectro_GpioEx(uint8_t address): i2cAddress_(address), PORT_(0), PIN_(0), DDR_(0) {

}

ESPectro_GpioEx::~ESPectro_GpioEx() {

}

void ESPectro_GpioEx::begin() {
    Wire.begin();
    readGPIO();
    clear();
}

int ESPectro_GpioEx::lastError() {
    int e = lastError_;
    lastError_ = PCF8574_OK;
    return e;
}

void ESPectro_GpioEx::pinMode(uint8_t pin, uint8_t mode) {
    /* Switch according mode */
    switch (mode) {
        case INPUT:
            DDR_ &= ~(1 << pin);
            PORT_ &= ~(1 << pin);
            break;

        case INPUT_PULLUP:
            DDR_ &= ~(1 << pin);
            PORT_ |= (1 << pin);
            break;

        case OUTPUT:
            DDR_ |= (1 << pin);
            PORT_ &= ~(1 << pin);
            break;

        default:
            break;
    }

    /* Update GPIO values */
    updateGPIO();
}

void ESPectro_GpioEx::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin > 7)
    {
        lastError_ = PCF8574_PIN_ERROR;
    }

    /* Set PORT bit value */
    if (value)
        PORT_ |= (1 << pin);
    else
        PORT_ &= ~(1 << pin);

    /* Update GPIO values */
    updateGPIO();
}

uint8_t ESPectro_GpioEx::digitalRead(uint8_t pin) {
    if (pin > 7)
    {
        lastError_ = PCF8574_PIN_ERROR;
        return 0;
    }

    /* Read GPIO */
    readGPIO();

    /* Read and return the pin state */
    return (uint8_t) (PIN_ & (1 << pin)) ? HIGH : LOW;
}

void ESPectro_GpioEx::write(uint8_t value) {
    /* Store pins values and apply */
    PORT_ = value;

    /* Update GPIO values */
    updateGPIO();
}

uint8_t ESPectro_GpioEx::read() {
    /* Read GPIO */
    readGPIO();

    /* Return current pins values */
    return PIN_;
}

void ESPectro_GpioEx::clear() {
    write(0x00);
}

void ESPectro_GpioEx::set() {

    /* User friendly wrapper for write() */
    write(0xFF);
}

void ESPectro_GpioEx::toggle(uint8_t pin) {

    /* Toggle pin state */
    PORT_ ^= (1 << pin);

    /* Update GPIO values */
    updateGPIO();
}

void ESPectro_GpioEx::blink(uint8_t pin, uint16_t count, uint32_t duration) {

    /* Compute steps duration */
    duration /= count * 2;

    /* Loop n times */
    while (count--) {

        /* Toggle pin 2 times */
        toggle(pin);
        delay(duration);
        toggle(pin);
        delay(duration);
    }
}

void ESPectro_GpioEx::readGPIO() {
    /* Start request, wait for data and receive GPIO values as byte */
    //Wire.requestFrom(i2cAddress_, (uint8_t) 0x01);
    if (Wire.requestFrom(i2cAddress_, (uint8_t)1) != 1)
    {
        lastError_ = PCF8574_I2C_ERROR;
    }

    while (Wire.available() < 1)
        ;
    PIN_ =  Wire.read();
}

void ESPectro_GpioEx::updateGPIO() {
    /* Compute new GPIO states */
    //uint8_t value = ((PIN_ & ~DDR_) & ~(~DDR_ & PORT_)) | PORT_; // Experimental
    uint8_t value = (PIN_ & ~DDR_) | PORT_;

    /* Start communication and send GPIO values as byte */
    Wire.beginTransmission(i2cAddress_);
    Wire.write(value);
    lastError_ = Wire.endTransmission();
}
