//
// Created by Andri Yadi on 9/5/16.
// Based on: https://github.com/AllAboutEE/MAX11609EEE-Breakout-Board/tree/master/Software/Arduino/AllAboutEE-MAX11609-Library
//

#ifndef XBOARD_ESPECTROBASE_ADC_H
#define XBOARD_ESPECTROBASE_ADC_H

#include <Arduino.h>
#include <Wire.h>

#define ESPECTRO_BASE_ADC_POT_CHANNEL        0
#define ESPECTRO_BASE_ADC_LDR_CHANNEL        4

class ESPectroBase_ADC {

public:
    ESPectroBase_ADC(uint8_t addr = ADDRESS);
    ~ESPectroBase_ADC();

    void begin(uint8_t sda, uint8_t scl, uint8_t vRef = NULL);
    void begin(uint8_t vRef = REF_VDD);
    void setup(uint8_t data);
    void configuration(uint8_t data);
    uint16_t read(uint8_t channel);
    void scan(uint16_t *buffer);

    static const uint8_t ADDRESS = 0x33,
            REF_EXTERNAL = 0x02, // As defined in Table 6. The external reference can range from 1V to VDD
            REF_INTERNAL = 0x05, // As defined in Table 6. Reference will be 2.048V
            REF_VDD = 0x00;

private:
    uint8_t i2cAddress_;
};


#endif //XBOARD_ESPECTROBASE_ADC_H
