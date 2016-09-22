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
}

void ESPectroBase::begin() {

    adc_ = new ESPectroBase_ADC();
    adc_->begin();
}

int ESPectroBase::analogRead(uint8_t channel) {
    if (adc_ == NULL) {
        Serial.println("ADC is NULL");
        return -1;
    }

    return (int)adc_->read(channel);
}
