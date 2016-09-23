//
// Created by Andri Yadi on 9/22/16.
//

#include "ESPectroBase.h"

ESPectroBase::ESPectroBase() {

}

ESPectroBase::~ESPectroBase() {
    if (adc_ != NULL) {
        delete adc_;
        adc_ = NULL;
    }

    if (bmp280_ != NULL) {
        delete bmp280_;
        bmp280_ = NULL;
    }
}

void ESPectroBase::beginADC() {

    adc_ = new ESPectroBase_ADC();
    adc_->begin();
}

int ESPectroBase::analogRead(uint8_t channel) {
    if (adc_ == NULL) {
        Serial.println(F("ADC is NULL"));
        return -1;
    }

    return (int)adc_->read(channel);
}

char ESPectroBase::beginTemperaturePressureSensor() {

    bmp280_ = new ESPectroBase_BMP280();

    char res = bmp280_->begin();
    if (res > 0) {
        bmp280_->setOversampling(4);
    }

    return res;
}

char ESPectroBase::readTemperatureAndPressure(double &T, double &P) {
    if (bmp280_ == NULL) {
        Serial.println(F("BMP280 is NULL"));
        return 0;
    }

    char result = bmp280_->startMeasurment();

    if(result!=0){
        delay(result); //blocking!!!
        result = bmp280_->getTemperatureAndPressure(T, P);
        return result;
    }
    else {
        return result;
    }
}

double ESPectroBase::estimateAltitude(double P, double P0) {
    if (bmp280_ == NULL) {
        Serial.println(F("BMP280 is NULL"));
        return 0;
    }

    return bmp280_->altitude(P,P0);
}
