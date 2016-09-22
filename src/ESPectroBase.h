//
// Created by Andri Yadi on 9/22/16.
//

#ifndef XBOARD_ESPECTRO_BASE_H
#define XBOARD_ESPECTRO_BASE_H

#include "Arduino.h"
#include "ESPectroBase_ADC.h"

class ESPectroBase {
public:
    ESPectroBase();
    ~ESPectroBase();

    void begin();
    int analogRead(uint8_t channel);

private:
    ESPectroBase_ADC *adc_ = NULL;
};


#endif //XBOARD_ESPECTRO_BASE_H
