//
// Created by Andri Yadi on 7/31/16.
//

#ifndef XBOARD_XBOARD_H
#define XBOARD_XBOARD_H

#include "Arduino.h"
#include <Ticker.h>
#include <functional>
//#include <exception>

//Constats
#define XBOARD_LED_PIN 15
#define XBOARD_NEOPIXEL_PIN 15

#define XBOARD_BUTTON_PIN 2
#define XBOARD_BUTTON_DEBOUNCE_DURATION_MS      50
#define XBOARD_BUTTON_PRESS_DURATION_MS         100
#define XBOARD_BUTTON_LONG_PRESS_DURATION_MS    700


class XBoard_LED {
public:
    XBoard_LED(byte pin = XBOARD_LED_PIN, boolean activeHigh = false);
    ~XBoard_LED();

    void begin();
    void turnOn();
    void turnOff();
    boolean isOn();
    byte getPin();
    void blink(int interval = 500);
    void stopBlink();

private:
    byte pin_;
    boolean activeHigh_;
    Ticker *blinkTicker_ = NULL;
};

class XBoard {
public:
    XBoard();
    ~XBoard();

    //LED convinient methods
    XBoard_LED &getLED();
    void turnOnLED();
    void turnOffLED();
    void blinkLED(int interval = 500);
    void stopBlinkLED();

private:
    XBoard_LED *led_ = NULL;

};

enum XBoard_Button_State { Pressed, Released, LongPressed};
typedef std::function<void()> ButtonActionCallback;
class XBoard_Button {
public:
    XBoard_Button(uint8_t pin = XBOARD_BUTTON_PIN, boolean activeHigh = false);
    ~XBoard_Button();

    void begin();
    XBoard_Button_State getState();
    void loop();
    void OnPressed(ButtonActionCallback cb);
    void OnReleased(ButtonActionCallback cb);
    void OnLongPressed(ButtonActionCallback cb);

private:
    uint8_t pin_;
    boolean activeHigh_;
    XBoard_Button_State buttonState_ = Released;

    unsigned long lastButtonChangedMillis_  = 0, lastButtonPressedMillis_ = 0;

    ButtonActionCallback pressedCallback_;
    ButtonActionCallback releasedCallback_;
    ButtonActionCallback longPressedCallback_;
};

#endif //XBOARD_XBOARD_H
