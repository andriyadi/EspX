//
// Created by Andri Yadi on 7/31/16.
//

#ifndef XBOARD_ESPECTRO_H
#define XBOARD_ESPECTRO_H

#include "Arduino.h"
#include "ESPectro_Constants.h"
#include <Ticker.h>
#include <functional>
//#include <exception>

class ESPectro_LED {
public:
    ESPectro_LED(byte pin = ESPECTRO_LED_PIN, boolean activeHigh = true);
    ~ESPectro_LED();

    void begin();
    void turnOn();
    void turnOff();
    boolean isOn();
    byte getPin();
    void blink(int interval = 500);
    void stopBlink();
    void toggle();

private:
    byte pin_;
    boolean activeHigh_;
    Ticker *blinkTicker_ = NULL;
};

class ESPectro {
public:
    ESPectro();
    ~ESPectro();

    //LED convinient methods
    ESPectro_LED &getLED();
    void turnOnLED();
    void turnOffLED();
    void blinkLED(int interval = 500);
    void stopBlinkLED();
    void toggleLED();

private:
    ESPectro_LED *led_ = NULL;

};

enum ESPectro_Button_State { Pressed, Released, LongPressed};
typedef std::function<void()> ButtonActionCallback;

class ESPectro_Button {
public:
    ESPectro_Button(uint8_t pin = ESPECTRO_BUTTON_PIN, boolean activeHigh = false);
    ~ESPectro_Button();

    void begin();
    ESPectro_Button_State getState();
    void loop();
    void onButtonDown(ButtonActionCallback cb);
    void onButtonUp(ButtonActionCallback cb);
    void onPressed(ButtonActionCallback cb);
    void onLongPressed(ButtonActionCallback cb);

private:
    uint8_t pin_;
    boolean activeHigh_;
    ESPectro_Button_State buttonState_ = Released;

    unsigned long lastButtonChangedMillis_  = 0, lastButtonPressedMillis_ = 0;

    ButtonActionCallback btnDownCallback_;
    ButtonActionCallback btnUpCallback_;
    ButtonActionCallback pressedCallback_;
    ButtonActionCallback longPressedCallback_;
};

#endif //XBOARD_ESPECTRO_H
