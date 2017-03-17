//
// Created by Andri Yadi on 7/31/16.
//

#include <c_types.h>
#include "ESPectro.h"

ESPectro_LED::ESPectro_LED(byte pin, boolean activeHigh): pin_(pin), activeHigh_(activeHigh) {
}

ESPectro_LED::~ESPectro_LED() {
    if (blinkTicker_ != NULL) {
        delete blinkTicker_;
        blinkTicker_ = NULL;
    }
}

void ESPectro_LED::begin() {
    pinMode(ESPECTRO_LED_PIN, OUTPUT);
    turnOff();
}

void ESPectro_LED::turnOn() {
    digitalWrite(pin_, activeHigh_? HIGH: LOW);
}

void ESPectro_LED::turnOff() {
    digitalWrite(pin_, activeHigh_? LOW: HIGH);
}

boolean ESPectro_LED::isOn() {
    int val = digitalRead(pin_);
    return activeHigh_? (val == HIGH): (val == LOW);
}

byte ESPectro_LED::getPin() {
    return pin_;
}

void ESPectro_LED::performBlink() {
    if (blinkMaxCount_ > 0 && blinkCount_ >= 2*blinkMaxCount_) {
        stopBlink();
        return;
    }

    boolean _isOn = isOn();
    if (_isOn) {
        turnOff();
    }
    else {
        turnOn();
    }

    blinkCount_++;
}

void doBlink(ESPectro_LED *led) {
    led->performBlink();
}

void ESPectro_LED::stopBlink() {
    if (blinkTicker_ != NULL) {
        blinkTicker_->detach();
        delete blinkTicker_;
        blinkTicker_ = NULL;
    }
}

void ESPectro_LED::blink(int interval, int count) {

    if (blinkTicker_ == NULL) {
        blinkTicker_ = new Ticker();
    }
    if (count > 0) {
        blinkMaxCount_ = count;
        blinkCount_ = 0;
    }
    else {
        blinkMaxCount_ = 0;
    }

    blinkTicker_->detach();
    delay(10);
    blinkTicker_->attach_ms(interval, doBlink, this);
}

void ESPectro_LED::toggle() {
    if (isOn()) {
        turnOff();
    }
    else {
        turnOn();
    }
}



ESPectro::ESPectro(ESPectro_Version v):version_(v) {

}

ESPectro::~ESPectro() {
    if (led_ != NULL) {
        delete led_;
        led_ = NULL;
    }

    if (neopixel_ != NULL) {
        delete neopixel_;
        neopixel_ = NULL;
    }
}


void ESPectro::turnOnLED() {
    getLED().turnOn();
}

void ESPectro::turnOffLED() {
    getLED().turnOff();
}

void ESPectro::blinkLED(int interval, int count) {
    getLED().blink(interval, count);
}

ESPectro_LED &ESPectro::getLED() {
    if (led_ == NULL) {
        led_ = new ESPectro_LED();
        led_->begin();
    }

    return *led_;
}

void ESPectro::stopBlinkLED() {
    getLED().stopBlink();
}

void ESPectro::toggleLED() {
    getLED().toggle();
}

ESPectro_Neopixel_Default &ESPectro::getNeopixel() {
    if (neopixel_ == NULL) {
        if (version_ >= ESPectro_V3) {
            neopixel_ = new ESPectro_Neopixel_Default(3, ESPECTRO_NEOPIXEL_PIN_V3);
        } else {
            neopixel_ = new ESPectro_Neopixel_Default(1, ESPECTRO_NEOPIXEL_PIN);
        }
        neopixel_->Begin();
    }

    return *neopixel_;
}

void ESPectro::turnOnNeopixel(NeoGrbFeature::ColorObject colorObject, uint16_t pixelNo) {
    getNeopixel().turnOn(colorObject, pixelNo);
}

void ESPectro::turnOffNeopixel(uint16_t pixelNo) {
    getNeopixel().turnOff(pixelNo);
}

void ESPectro::turnOffAllNeopixel() {
    for(uint16_t i = 0; i < getNeopixel().PixelCount(); i++) {
        getNeopixel().turnOff(i);
    }
}

/* the global instance pointer */
ESPectro_Button *ESPectro_Button::pESPButton = NULL;
static volatile bool ESPectro_Button_Value_Changed = false;
static volatile bool ESPectro_Button_RunAlreadyCalled_ = false;

ESPectro_Button::ESPectro_Button(ESPectro_Version v, uint8_t gpio, boolean activeHigh):
        gpioNumber_(gpio), activeHigh_(activeHigh), version_(v)

{
    if (gpio == ESPECTRO_BUTTON_PIN && v == ESPectro_V3) {
        gpioNumber_ = ESPECTRO_BUTTON_PIN_V3;
    }
}

ESPectro_Button::~ESPectro_Button() {

}

void ESPectro_Button_Interrupt() {
    ESPectro_Button_Value_Changed = true;
    //Serial.printf("Button triggered %d\r\n", digitalRead(ESPECTRO_BUTTON_PIN));

    if (!ESPectro_Button_RunAlreadyCalled_) {
        ESPectro_Button *const pESPButton = ESPectro_Button::GetInstance();
        //ASSERT(pESPButton != NULL);

        pESPButton->run();
    }
}

void ESPectro_Button::begin() {

    //ASSERT(ESPectro_Button::pESPButton == this || ESPectro_Button::pESPButton == NULL);
    ESPectro_Button::pESPButton = this;

    //Serial.printf("Button gpio %d\r\n", gpioNumber_);

    pinMode(gpioNumber_, activeHigh_ ? INPUT_PULLDOWN_16 : INPUT_PULLUP);
    if (gpioNumber_ != 16) {
        attachInterrupt(gpioNumber_, ESPectro_Button_Interrupt, CHANGE);
    }
    else {
        Serial.println(F("Unsupported pin for interrupt"));
    }
//    else {
//        throw std::runtime_error("fatal error");
//    }
}

void ESPectro_Button::onPressed(ButtonActionCallback cb) {
    btnDownCallback_ = cb;
}

void ESPectro_Button::onButtonDown(ButtonActionCallback cb) {
    btnDownCallback_ = cb;
}

void ESPectro_Button::onButtonUp(ButtonActionCallback cb) {
    btnUpCallback_ = cb;
}

void ESPectro_Button::onLongPressed(ButtonActionCallback cb) {
    longPressedCallback_ = cb;
}

ESPectro_Button_State ESPectro_Button::getState() {
    return Released;
}

void ESPectro_Button::run() {

    ESPectro_Button_RunAlreadyCalled_ = true;
    unsigned long currentMillis = millis();

    if (!ESPectro_Button_Value_Changed) {

        if (buttonState_ == Pressed && (currentMillis - lastButtonPressedMillis_ > ESPECTRO_BUTTON_LONG_PRESS_DURATION_MS)) {
            buttonState_ = LongPressed;
            //Serial.println(F("Considered Long Pressed"));
            if (longPressedCallback_) {
                longPressedCallback_();
            }
        }
    }
    else {

        ESPectro_Button_Value_Changed = false;

        if ((currentMillis - lastButtonChangedMillis_) > ESPECTRO_BUTTON_DEBOUNCE_DURATION_MS) {
            lastButtonChangedMillis_ = currentMillis;
        }
        else {
            //Serial.println(F("Debounced"));
            return;
        }

        int buttonState = digitalRead(gpioNumber_);
        boolean pressed = activeHigh_ ? buttonState == HIGH : buttonState == LOW;
        if (pressed) {
            lastButtonPressedMillis_ = currentMillis;
            buttonState_ = Pressed;
            //Serial.println(F("Pressed"));
            if (btnDownCallback_) {
                btnDownCallback_();
            }
        }
        else {
            //lastButtonReleasedMillis_ = currentMillis;
            //Serial.println(F("Released"));

            if (btnUpCallback_) {
                btnUpCallback_();
            }

            if (buttonState_ != LongPressed && currentMillis - lastButtonPressedMillis_ > ESPECTRO_BUTTON_PRESS_DURATION_MS) {
                buttonState_ = Released;
                //Serial.println(F("Considered Pressed"));
                if (pressedCallback_) {
                    pressedCallback_();
                }
            }
        }
    }
}


