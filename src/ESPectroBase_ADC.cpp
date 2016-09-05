//
// Created by Andri Yadi on 9/5/16.
//

#include "ESPectroBase_ADC.h"

ESPectroBase_ADC::ESPectroBase_ADC(uint8_t addr): i2cAddress_(addr) {

}

ESPectroBase_ADC::~ESPectroBase_ADC() {

}

void ESPectroBase_ADC::begin(uint8_t sda, uint8_t scl, uint8_t vRef) {
    Wire.begin(sda,scl);
}

void ESPectroBase_ADC::begin(uint8_t vRef) {
    Wire.begin();

    // 0 - don't care
    // 1 - reset configuration register to default
    // 2 - unipolar
    // 3 - internal clock
    // 4 - SEL0 (vRef)
    // 5 - SEL1 (vRef)
    // 6 - SEL2 (vRef)
    setup(vRef & 0xf0); // 0B[vRef2][vRef1][vRef0] 0000

    // 0 - Single Ended
    // 1 to 4 - Channel Select:  7
    // 5 to 6 - Scan Mode: read channels up to selected channel
    configuration(B00001111);
}

void ESPectroBase_ADC::setup(uint8_t data) {
    Wire.beginTransmission(i2cAddress_);
    Wire.write(data | 0x80);
    Wire.endTransmission();
}

void ESPectroBase_ADC::configuration(uint8_t data) {
    Wire.beginTransmission(i2cAddress_);
    Wire.write(data & (~0x80)); // make REG bit 7 = 0 (configuration byte)
    Wire.endTransmission();
}

uint16_t ESPectroBase_ADC::read(uint8_t channel) {
    uint16_t result = 0x0000;

    uint8_t configurationByte = ( (channel<<1) & B00001110) | B01100001;
    configuration(configurationByte);

    Wire.requestFrom(i2cAddress_, 2, false);

    if(Wire.available()==2) // the conversion consists of two bytes per channel
    {

        result = (Wire.read() & 0x03)<<8; // MSB is returned first. [7-2] are high.
        result |= Wire.read()&0x00ff; // read LSB


        return result;
    }
    else
    {
        return -1; // ERROR
    }
}

void ESPectroBase_ADC::scan(uint16_t *buffer) {
    uint8_t configurationByte = B00001111;
    configuration(configurationByte);

    Wire.requestFrom(i2cAddress_, 16, false); // 2 bytes per channel. There are 8 channels.

    if(Wire.available()==16)
    {
        for(uint8_t i = 0;i<8;i++) // read all 8 channels [AIN0-AIN7]
        {

            *(buffer+i) = (Wire.read() & 0x03)<<8; // MSB is returned first. [7-2] are high.
            *(buffer+i) |= Wire.read() & 0x00ff; // read LSB
        }
    }
}

