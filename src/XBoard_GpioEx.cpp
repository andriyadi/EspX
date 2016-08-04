//
// Created by Andri Yadi on 8/1/16.
//

#include "XBoard_GpioEx.h"

XBoard_GpioEx::XBoard_GpioEx(uint8_t address):i2cAddress_(address), PORT_(0), PIN_(0), DDR_(0) {

}

XBoard_GpioEx::~XBoard_GpioEx() {

}

void XBoard_GpioEx::begin() {
    Wire.begin();
    readGPIO();
    clear();
}

int XBoard_GpioEx::lastError() {
    int e = lastError_;
    lastError_ = PCF8574_OK;
    return e;
}

void XBoard_GpioEx::pinMode(uint8_t pin, uint8_t mode) {
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

void XBoard_GpioEx::digitalWrite(uint8_t pin, uint8_t value) {
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

uint8_t XBoard_GpioEx::digitalRead(uint8_t pin) {
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

void XBoard_GpioEx::write(uint8_t value) {
    /* Store pins values and apply */
    PORT_ = value;

    /* Update GPIO values */
    updateGPIO();
}

uint8_t XBoard_GpioEx::read() {
    /* Read GPIO */
    readGPIO();

    /* Return current pins values */
    return PIN_;
}

void XBoard_GpioEx::clear() {
    write(0x00);
}

void XBoard_GpioEx::set() {

    /* User friendly wrapper for write() */
    write(0xFF);
}

void XBoard_GpioEx::toggle(uint8_t pin) {

    /* Toggle pin state */
    PORT_ ^= (1 << pin);

    /* Update GPIO values */
    updateGPIO();
}

void XBoard_GpioEx::blink(uint8_t pin, uint16_t count, uint32_t duration) {

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

void XBoard_GpioEx::readGPIO() {
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

void XBoard_GpioEx::updateGPIO() {
    /* Compute new GPIO states */
    //uint8_t value = ((PIN_ & ~DDR_) & ~(~DDR_ & PORT_)) | PORT_; // Experimental
    uint8_t value = (PIN_ & ~DDR_) | PORT_;

    /* Start communication and send GPIO values as byte */
    Wire.beginTransmission(i2cAddress_);
    Wire.write(value);
    lastError_ = Wire.endTransmission();
}
