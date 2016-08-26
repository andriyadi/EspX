//
// Created by Andri Yadi on 8/26/16.
//

#ifndef XBOARD_DCX_LORASERVICE_H
#define XBOARD_DCX_LORASERVICE_H

#include "Arduino.h"
#include "DCX_Constants.h"
#include "SX1272.h"
#include "DCX_AppSetting.h"
#include <functional>

typedef std::function<void()> LoraClientConnectedCallback;
typedef std::function<void(bool, String)> LoraMessageAcknowledgedCallback;
typedef std::function<void(String)> LoraMessageAvailableCallback;

class DCX_LoRaService {
public:
    DCX_LoRaService(DCX_AppSetting& setting);
    virtual ~DCX_LoRaService();

    void begin();
    void loop();
    void onConnected(LoraClientConnectedCallback callback);
    void onMessageAcknowledged(LoraMessageAcknowledgedCallback callback);
    void onMessageAvailable(LoraMessageAvailableCallback callback);
    void publish(String topic, String payload, boolean shouldRetry = false, boolean withAck = true);
    void publish(String payload, boolean shouldRetry = false, boolean withAck = true);
    void subscribe(String topic);
    void setMessagePrefix(String msg);

    //convenience
    void sendRawMessage(uint8_t dest, const char *msg, boolean withAck = true);

    boolean isPublishing() {
        return publishing;
    }

private:
    DCX_AppSetting &settings_;
    uint8_t message_[110];
    String messagePrefix_;

    unsigned long lastTransmissionTime_ = 0;
    volatile boolean networkReady = false, publishing = false;

    void doSendMessage(uint8_t dest, const char *msg, boolean withAck = true);

    void handleAcknowledge(uint8_t dest, boolean success, const char *origMsg);

    LoraClientConnectedCallback connectedCallback_;
    LoraMessageAvailableCallback msgAvailableCallback_;
    LoraMessageAcknowledgedCallback ackCallback_;
};


#endif //XBOARD_DCX_LORASERVICE_H
