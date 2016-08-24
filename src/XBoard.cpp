//
// Created by Andri Yadi on 7/31/16.
//

#include <c_types.h>
#include "XBoard.h"

XBoard_LED::XBoard_LED(byte pin, boolean activeHigh):pin_(pin), activeHigh_(activeHigh) {
}

XBoard_LED::~XBoard_LED() {
    if (blinkTicker_ != NULL) {
        delete blinkTicker_;
        blinkTicker_ = NULL;
    }
}

void XBoard_LED::begin() {
    pinMode(XBOARD_LED_PIN, OUTPUT);
    turnOff();
}

void XBoard_LED::turnOn() {
    digitalWrite(pin_, activeHigh_? HIGH: LOW);
}

void XBoard_LED::turnOff() {
    digitalWrite(pin_, activeHigh_? LOW: HIGH);
}

boolean XBoard_LED::isOn() {
    int val = digitalRead(pin_);
    return activeHigh_? (val == HIGH): (val == LOW);
}

byte XBoard_LED::getPin() {
    return pin_;
}

void doBlink(XBoard_LED *led) {
    boolean isOn = led->isOn();
    if (isOn) {
        led->turnOff();
    }
    else {
        led->turnOn();
    }
}

void XBoard_LED::stopBlink() {
    if (blinkTicker_ != NULL) {
        blinkTicker_->detach();
        delete blinkTicker_;
        blinkTicker_ = NULL;
    }
}

void XBoard_LED::blink(int interval) {

    if (blinkTicker_ == NULL) {
        blinkTicker_ = new Ticker();
    }
    blinkTicker_->detach();
    delay(10);
    blinkTicker_->attach_ms(interval, doBlink, this);
}

XBoard::XBoard() {

}

XBoard::~XBoard() {
    if (led_ != NULL) {
        delete led_;
        led_ = NULL;
    }
}


void XBoard::turnOnLED() {
    getLED().turnOn();
}

void XBoard::turnOffLED() {
    getLED().turnOff();
}

void XBoard::blinkLED(int interval) {
    getLED().blink(interval);
}

XBoard_LED &XBoard::getLED() {
    if (led_ == NULL) {
        led_ = new XBoard_LED();
        led_->begin();
    }

    return *led_;
}

void XBoard::stopBlinkLED() {
    getLED().stopBlink();
}



static volatile boolean XBoard_Button_Value_Changed = false;

XBoard_Button::XBoard_Button(uint8_t pin, boolean activeHigh):
        pin_(pin), activeHigh_(activeHigh)

{
}

XBoard_Button::~XBoard_Button() {

}

void XBoard_Button_Interrupt() {
    XBoard_Button_Value_Changed = true;
    //Serial.printf("Button triggered %d\r\n", digitalRead(XBOARD_BUTTON_PIN));
}

void XBoard_Button::begin() {
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

void XBoard_Button::OnPressed(ButtonActionCallback cb) {
    btnDownCallback_ = cb;
}

void XBoard_Button::OnButtonDown(ButtonActionCallback cb) {
    btnDownCallback_ = cb;
}

void XBoard_Button::OnButtonUp(ButtonActionCallback cb) {
    btnUpCallback_ = cb;
}

void XBoard_Button::OnLongPressed(ButtonActionCallback cb) {
    longPressedCallback_ = cb;
}

XBoard_Button_State XBoard_Button::getState() {
    //return digitalRead(pin_);
    return Released;
}

void XBoard_Button::loop() {
    unsigned long currentMillis = millis();

    if (!XBoard_Button_Value_Changed) {

        if (buttonState_ == Pressed && (currentMillis - lastButtonPressedMillis_ > XBOARD_BUTTON_LONG_PRESS_DURATION_MS)) {
            buttonState_ = LongPressed;
            //Serial.println(F("Considered Long Pressed"));
            if (longPressedCallback_) {
                longPressedCallback_();
            }
        }
    }
    else {

        XBoard_Button_Value_Changed = false;

        if ((currentMillis - lastButtonChangedMillis_) > XBOARD_BUTTON_DEBOUNCE_DURATION_MS) {
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

            if (buttonState_ != LongPressed && currentMillis - lastButtonPressedMillis_ > XBOARD_BUTTON_PRESS_DURATION_MS) {
                buttonState_ = Released;
                //Serial.println(F("Considered Pressed"));
                if (pressedCallback_) {
                    pressedCallback_();
                }
            }
        }
    }
}


