//
// Created by Andri Yadi on 8/25/16.
//

#ifndef XBOARD_ESPECTRO_NEOPIXEL_H
#define XBOARD_ESPECTRO_NEOPIXEL_H

#include "Arduino.h"
#include <NeoPixelBus.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
//#include <NeoPixelAnimator.h>

#define ESPECTRO_NEOPIXEL_PIN 15

template<typename T_COLOR_FEATURE, typename T_METHOD>
class ESPectro_Neopixel: public NeoPixelBus<T_COLOR_FEATURE, T_METHOD> {
public:
    ESPectro_Neopixel(uint16_t countPixels, uint8_t pin);

    ~ESPectro_Neopixel() {
//        if (myAnimator_ != NULL) {
//            if (myAnimator_->IsAnimating()) {
//                myAnimator_->StopAnimation(0);
//            }
//
//            delete myAnimator_;
//            myAnimator_ = NULL;
//        }
    }

    void turnOn(typename T_COLOR_FEATURE::ColorObject colorObject) {
        if (this->PixelCount() == 0) {
            return;
        }

        this->SetPixelColor(0, colorObject);
        this->Show();
    }

    void turnOff() {
        if (this->PixelCount() == 0) {
            return;
        }

        this->SetPixelColor(0, RgbColor(0));
        this->Show();
    };

    /*
    void loop() {
        if (myAnimator_ != NULL && myAnimator_->IsAnimating())
        {
            // the normal loop just needs these two to run the active animations
            myAnimator_->UpdateAnimations();
            this->Show();
        }
    }

    void startPulsingAnimation(HtmlColor toColor, uint16_t interval = 800) {

        if (this->PixelCount() == 0) {
            return;
        }

        if (myAnimator_ == NULL) {
            myAnimator_ = new NeoPixelAnimator(this->PixelCount(), NEO_MILLISECONDS);
        }

        if (myAnimator_->IsAnimating()) {
            myAnimator_->StopAnimation(0);
        }

        //Serial.println("Animation STARTED");

        AnimEaseFunction easing = NeoEase::Linear;

        RgbColor original = this->GetPixelColor(0);
        RgbColor target = (pulsingAnimationState_ == 0)? RgbColor(toColor): RgbColor(0);

        //strip.SetPixelColor(i, color);

        ESPectro_Neopixel *mySelf = this;

        AnimUpdateCallback animUpdate = [=](const AnimationParam& param)
        {
            // progress will start at 0.0 and end at 1.0
            // we convert to the curve we want
            float progress = easing(param.progress);

            // use the curve value to apply to the animation
            RgbColor updatedColor = RgbColor::LinearBlend(original, target, progress);
            mySelf->SetPixelColor(0, updatedColor);

            if (progress >= 0.9) {
                //Serial.println("Animation DONE");

//                if (stripBlinkCount > 0) {
//                    animateToColor(toColor);
//                }
//                else {
//                    //if (stripEffectState_ == 0) {
//                    //Serial.println(F("COLOR CORRECTED"));
//                    strip.SetPixelColor(NEOPIXEL_STATUS_LED_INDEX, RgbColor(0));
//                    strip.Show();
//                    //}
//                }
//                stripBlinkCount--;

                //Repeat
                startPulsingAnimation(toColor);
            }
        };

        myAnimator_->StartAnimation(0, interval, animUpdate);
        pulsingAnimationState_ = (pulsingAnimationState_ + 1) % 2;
    }

    void stopPulsingAnimation() {

        if (this->PixelCount() == 0) {
            return;
        }

        this->SetPixelColor(0, RgbColor(0));
        this->Show();

        if (myAnimator_ != NULL) {
            if (myAnimator_->IsAnimating()) {
                myAnimator_->StopAnimation(0);
            }

            delete myAnimator_;
            myAnimator_ = NULL;
        }
    }

private:
    NeoPixelAnimator *myAnimator_ = NULL;
    uint16_t pulsingAnimationState_ = 0;
     */

};


class ESPectro_Neopixel_Default : public ESPectro_Neopixel<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> {
public:
    ESPectro_Neopixel_Default(uint16_t countPixels = 1, uint8_t pin = ESPECTRO_NEOPIXEL_PIN);
    ~ESPectro_Neopixel_Default();
};

class ESPectro_Neopixel_UART : public ESPectro_Neopixel<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> {
public:
    ESPectro_Neopixel_UART(uint16_t countPixels = 1, uint8_t pin = 2);
    ~ESPectro_Neopixel_UART();
};

class ESPectro_Neopixel_DMA : public ESPectro_Neopixel<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> {
public:
    ESPectro_Neopixel_DMA(uint16_t countPixels = 1, uint8_t pin = 3);
    ~ESPectro_Neopixel_DMA();
};

#endif //XBOARD_ESPECTRO_NEOPIXEL_H
