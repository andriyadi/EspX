//
// Created by Andri Yadi on 9/22/16.
//

#ifndef XBOARD_ESPECTRO_BASE_H
#define XBOARD_ESPECTRO_BASE_H

#include "Arduino.h"
#include "ESPectroBase_ADC.h"
#include "ESPectroBase_BMP280.h"

class ESPectroBase {
public:
    ESPectroBase();
    ~ESPectroBase();

    void beginADC();
    int analogRead(uint8_t channel);

    char beginTemperaturePressureSensor();
    char readTemperatureAndPressure(double &T, double &P);
    double estimateAltitude(double P, double P0);

private:
    ESPectroBase_ADC *adc_ = NULL;
    ESPectroBase_BMP280 *bmp280_ = NULL;
};


#endif //XBOARD_ESPECTRO_BASE_H
