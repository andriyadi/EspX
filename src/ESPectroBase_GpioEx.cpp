//
// Created by Andri Yadi on 8/1/16.
//

#include "ESPectroBase_GpioEx.h"

ESPectroBase_GpioEx::ESPectroBase_GpioEx(uint8_t address): i2cAddress_(address), PORT_(0x00), PIN_(0x00), DDR_(0xFF) {

}

ESPectroBase_GpioEx::~ESPectroBase_GpioEx() {

}

boolean ESPectroBase_GpioEx::begin() {
    Wire.begin();

    uint8_t stat;

//    stat = writeReg(MAX7315_REG_PORT_CONFIG, 0xF8);		//Ports 0, 1, and 2 are outputs
//    if(!stat)
//    {
//        Serial.printf("CANNOT CONFIG 1. Error %d\r\n", lastError_);
//        return false;
//    }

    stat = writeReg(MAX7315_REG_CONFIG, 0x08);			//Blink disabled, global intensity control disabled, INT is enabled
    if(!stat)
    {
        Serial.printf("CANNOT CONFIG 2. Error %d\r\n", lastError_);
        return false;
    }

    stat = writeReg(MAX7315_REG_BLINK0, 0xFF);
    if(!stat)
    {
        Serial.printf("CANNOT CONFIG 3. Error %d\r\n", lastError_);
        return false;
    }

    stat = readReg(MAX7315_REG_INPUTS, NULL, 0);
    if(!stat)
    {
        Serial.printf("CANNOT CONFIG 4. Error %d\r\n", lastError_);
        return false;
    }

//    writeReg(0x0E, 0xE);

    readGPIO();
//    clear();

    return true;
}

int ESPectroBase_GpioEx::lastError() {
    int e = lastError_;
    lastError_ = GPIOEX_OK;
    return e;
}

void ESPectroBase_GpioEx::pinMode(uint8_t pin, uint8_t mode) {
    /* Switch according mode */
    switch (mode) {
        case INPUT:
            DDR_ |= (1 << pin); //DDR_ &= ~(1 << pin);
            //PORT_ &= ~(1 << pin);
            break;

        case INPUT_PULLUP:
            DDR_ |= (1 << pin); //DDR_ &= ~(1 << pin);
            //PORT_ |= (1 << pin);
            break;

        case OUTPUT:
            DDR_ &= ~(1 << pin); //DDR_ |= (1 << pin);
            //PORT_ &= ~(1 << pin);
            break;

        default:
            break;
    }

    /* Update GPIO values */
    //updateGPIO();

    uint8_t stat = writeReg(MAX7315_REG_PORT_CONFIG, DDR_);
    if(!stat)
    {
        Serial.printf("CANNOT PINMODE. Error %d\r\n", lastError_);
    }
}

void ESPectroBase_GpioEx::digitalWrite(uint8_t pin, uint8_t value) {
    if (pin > 7)
    {
        lastError_ = GPIOEX_PIN_ERROR;
        return;
    }

    /* Set PORT bit value */
    if (value)
        PORT_ |= (1 << pin);
    else
        PORT_ &= ~(1 << pin);

    /* Update GPIO values */
    updateGPIO();
}

uint8_t ESPectroBase_GpioEx::digitalRead(uint8_t pin) {
    if (pin > 7)
    {
        lastError_ = GPIOEX_PIN_ERROR;
        return 0;
    }

    /* Read GPIO */
    readGPIO();

    /* Read and return the pin state */
    return (uint8_t) (PIN_ & (1 << pin)) ? HIGH : LOW;
}

void ESPectroBase_GpioEx::write(uint8_t value) {
    /* Store pins values and apply */
    PORT_ = value;

    /* Update GPIO values */
    updateGPIO();
}

uint8_t ESPectroBase_GpioEx::read() {
    /* Read GPIO */
    readGPIO();

    /* Return current pins values */
    return PIN_;
}

void ESPectroBase_GpioEx::clear() {
    write(0x00);
}

void ESPectroBase_GpioEx::set() {

    /* User friendly wrapper for write() */
    write(0xFF);
}

void ESPectroBase_GpioEx::toggle(uint8_t pin) {

    /* Toggle pin state */
    PORT_ ^= (1 << pin);

    /* Update GPIO values */
    updateGPIO();
}

void ESPectroBase_GpioEx::blink(uint8_t pin, uint16_t count, uint32_t duration) {

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

void ESPectroBase_GpioEx::readGPIO() {
    /* Start request, wait for data and receive GPIO values as byte */
    //Wire.requestFrom(i2cAddress_, (uint8_t) 0x01);
//    if (Wire.requestFrom(i2cAddress_, (uint8_t)1) != 1)
//    {
//        lastError_ = GPIOEX_I2C_ERROR;
//    }
//
//    while (Wire.available() < 1)
//        ;
//    PIN_ =  Wire.read();

    uint8_t data[1];

    uint8_t stat = readReg(MAX7315_REG_INPUTS, data, 1);
    if(!stat)
    {
        Serial.printf("Cannot read GPIO. Error %d\r\n", lastError_);
        return;
    }

    PIN_ = data[0];
}

void ESPectroBase_GpioEx::updateGPIO() {
    /* Compute new GPIO states */
    //uint8_t value = ((PIN_ & ~DDR_) & ~(~DDR_ & PORT_)) | PORT_; // Experimental

//    uint8_t value = (PIN_ & ~DDR_) | PORT_;
//
//    /* Start communication and send GPIO values as byte */
//    Wire.beginTransmission(i2cAddress_);
//    Wire.write(value);
//    lastError_ = Wire.endTransmission();

    Serial.print("DDR: ");
    Serial.println(DDR_, BIN);
    Serial.print("PORT: ");
    Serial.println(PORT_, BIN);

    uint8_t stat = writeReg(MAX7315_REG_BLINK0, PORT_);
    if(!stat)
    {
        Serial.printf("CANNOT OUTPUT. Error %d\r\n", lastError_);
    }
}

uint8_t ESPectroBase_GpioEx::writeReg(uint8_t reg, unsigned char *values, char length)
// Write an array of bytes to device
// values: external array of data to write. Put starting register in values[0].
// length: number of bytes to write
{
    Wire.beginTransmission(i2cAddress_);
    Wire.write(reg);
    Wire.write(values,length);
    lastError_ = Wire.endTransmission();
    return (lastError_ == 0)? 1: 0;
}

uint8_t ESPectroBase_GpioEx::writeReg(uint8_t reg, uint8_t value)
// Write an array of bytes to device
// values: external array of data to write. Put starting register in values[0].
// length: number of bytes to write
{
    Wire.beginTransmission(i2cAddress_);
    Wire.write(reg);
    Wire.write(value);
    lastError_ = Wire.endTransmission();
    return (lastError_ == 0)? 1: 0;
}

uint8_t ESPectroBase_GpioEx::readReg(uint8_t reg, uint8_t *values, uint8_t length) {

    Wire.beginTransmission(i2cAddress_);
    Wire.write(reg);
    uint8_t _err = Wire.endTransmission();
    if (_err > 0) {
        return 0;
    }

    if (values == NULL) {
        return 1;
    }

    Wire.requestFrom(i2cAddress_, length, (uint8_t)1);

    while(Wire.available() != length) ; // wait until bytes are ready
    for(uint16_t x=0; x<length; x++)
    {
        values[x] = Wire.read();
    }

    return 1;
}
