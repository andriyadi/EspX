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

void doBlink(ESPectro_LED *led) {
    boolean isOn = led->isOn();
    if (isOn) {
        led->turnOff();
    }
    else {
        led->turnOn();
    }
}

void ESPectro_LED::stopBlink() {
    if (blinkTicker_ != NULL) {
        blinkTicker_->detach();
        delete blinkTicker_;
        blinkTicker_ = NULL;
    }
}

void ESPectro_LED::blink(int interval) {

    if (blinkTicker_ == NULL) {
        blinkTicker_ = new Ticker();
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



ESPectro::ESPectro() {

}

ESPectro::~ESPectro() {
    if (led_ != NULL) {
        delete led_;
        led_ = NULL;
    }
}


void ESPectro::turnOnLED() {
    getLED().turnOn();
}

void ESPectro::turnOffLED() {
    getLED().turnOff();
}

void ESPectro::blinkLED(int interval) {
    getLED().blink(interval);
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


static volatile boolean XBoard_Button_Value_Changed = false;

ESPectro_Button::ESPectro_Button(uint8_t pin, boolean activeHigh):
        pin_(pin), activeHigh_(activeHigh)

{
}

ESPectro_Button::~ESPectro_Button() {

}

void XBoard_Button_Interrupt() {
    XBoard_Button_Value_Changed = true;
    //Serial.printf("Button triggered %d\r\n", digitalRead(XBOARD_BUTTON_PIN));
}

void ESPectro_Button::begin() {
    pinMode(pin_, activeHigh_? INPUT_PULLDOWN_16: INPUT_PULLUP);
    if (pin_ != 16) {
        attachInterrupt(pin_, XBoard_Button_Interrupt, CHANGE);
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
    //return digitalRead(pin_);
    return Released;
}

void ESPectro_Button::loop() {
    unsigned long currentMillis = millis();

    if (!XBoard_Button_Value_Changed) {

        if (buttonState_ == Pressed && (currentMillis - lastButtonPressedMillis_ > ESPECTRO_BUTTON_LONG_PRESS_DURATION_MS)) {
            buttonState_ = LongPressed;
            //Serial.println(F("Considered Long Pressed"));
            if (longPressedCallback_) {
                longPressedCallback_();
            }
        }
    }
    else {

        XBoard_Button_Value_Changed = false;

        if ((currentMillis - lastButtonChangedMillis_) > ESPECTRO_BUTTON_DEBOUNCE_DURATION_MS) {
            lastButtonChangedMillis_ = currentMillis;
        }
        else {
            //Serial.println(F("Debounced"));
            return;
        }

        int buttonState = digitalRead(pin_);
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


