//
// Created by Andri Yadi on 8/26/16.
//

#include "DCX_LoRaService.h"

#ifdef ESP8266
#include "Esp.h"
#endif

uint8_t my_appKey[4] = { 5, 6, 7, 8 };

DCX_LoRaService::DCX_LoRaService(DCX_AppSetting &setting): settings_(setting)
{
}

DCX_LoRaService::~DCX_LoRaService() {

}

void DCX_LoRaService::begin() {

    messagePrefix_ = String(LORA_MSG_PREFIX);

    // Power ON the module
    sx1272.ON();

    //using namespace std::placeholders;
    //svc.setOnMotionDetectedHandler(std::bind(&LoRaService::processDetectedActivity, this, _1, _2, _3, _4));

    int e;

    // Set transmission mode and print the result
    e = sx1272.setMode(settings_.loraMode);

#ifdef DEBUG_SERIAL
    Serial.print(F("Setting Mode: state "));
    Serial.println(e, DEC);
#endif

    // enable carrier sense
    sx1272._enableCarrierSense = true;

#ifdef LORA_BAND868
    // Select frequency channel
	e = sx1272.setChannel(CH_10_868);
#else // assuming #defined BAND900
    // Select frequency channel
    e = sx1272.setChannel(CH_05_900);
#endif

#ifdef DEBUG_SERIAL
    Serial.print(F("Setting Channel: state "));
    Serial.println(e, DEC);
#endif

#if defined LORA_RADIO_RFM92_95 || defined LORA_RADIO_INAIR9B
    e = sx1272.setPower('x');
#else
    e = sx1272.setPower('M');
#endif

#ifdef DEBUG_SERIAL
    Serial.print(F("Setting Power: state "));
    Serial.println(e);
#endif

    // Set the node address and print the result
    e = sx1272.setNodeAddress(settings_.loraNodeAddress);
#ifdef DEBUG_SERIAL
    Serial.print(F("Setting node addr: state "));
    Serial.println(e, DEC);
#endif

#ifdef DEBUG_SERIAL
    // Print a success message
    Serial.println(F("SX1272 successfully configured"));
#endif

    delay(500);

    //TODO: Make sure it's ready
    networkReady = true;
    if (connectedCallback_) {
        connectedCallback_();
    }

#ifdef DEBUG_SERIAL
#endif

}

void DCX_LoRaService::loop() {
    if (!networkReady) {
        return;
    }

    if (publishing) {
        return;
    }

    //TODO: Keep Alive code

    int e = sx1272.receivePacketTimeout(LORA_RCV_TIMEOUT);
    if (!e) {
        uint8_t tmp_length;
        tmp_length = sx1272._payloadlength;

#ifdef DEBUG_SERIAL
        Serial.print("Received a packet with length: ");
        Serial.println(tmp_length);
#endif

        String receivedStr = "";
        for (int a = 0; a < tmp_length; a++) {
            //Serial.print((char )sx1272.packet_received.data[a]);
            receivedStr += String((char )sx1272.packet_received.data[a]);
        }

#ifdef DEBUG_SERIAL
        Serial.println();
		Serial.print(F("Got: "));
		Serial.println(receivedStr);
#endif

        if (msgAvailableCallback_) {
            msgAvailableCallback_(receivedStr);
        }
    }
}

void DCX_LoRaService::onConnected(LoraClientConnectedCallback callback) {
    connectedCallback_ = callback;
}

void DCX_LoRaService::onMessageAcknowledged(LoraMessageAcknowledgedCallback callback) {
    ackCallback_ = callback;
}

void DCX_LoRaService::onMessageAvailable(LoraMessageAvailableCallback callback) {
    msgAvailableCallback_ = callback;
}

void DCX_LoRaService::publish(String topic, String payload, boolean shouldRetry, boolean withAck) {
    //String payloadStr = String(LORA_MSG_PREFIX) + "T=" + topic + "&" + payload;

//#ifdef ESP8266
//    uint16_t battMV = ESP.getVcc();
//    Serial.print(F("VBAT: "));
//    Serial.println(battMV);
//#endif

//    uint8_t message[160];
//    uint8_t r_size;
//    //r_size = sprintf((char*)message, "%sT=%s&%s&N=%d&V=%d", messagePrefix_.c_str(), topic.c_str(), payload.c_str(), settings_.loraNodeAddress, battMV);
//    r_size = sprintf((char*)message, "%sT=%s&%s&N=%d", messagePrefix_.c_str(), topic.c_str(), payload.c_str(), settings_.loraNodeAddress);
//
//    settings_.loraRetryCount = shouldRetry? 1: 0;
//
//    sendRawMessage(settings_.loraDestAddress, reinterpret_cast<char*>(message), withAck);

    String sentPayload = messagePrefix_ + "T=" + topic + "&" + payload + "&N=" + String(settings_.loraNodeAddress);
    uint8_t buffer[sentPayload.length() + 1];

    sentPayload.getBytes(buffer, sentPayload.length()+1);
    sendRawMessage(settings_.loraDestAddress, buffer, sentPayload.length(), withAck);
}

void DCX_LoRaService::publish(String payload, boolean shouldRetry, boolean withAck) {
    //String payloadStr = String(LORA_MSG_PREFIX) + "T=" + topic + "&" + payload;

//#ifdef ESP8266
//    uint16_t battMV = ESP.getVcc();
//    Serial.print(F("VBAT: "));
//    Serial.println(battMV);
//#endif

//    uint8_t message[160];
//    uint8_t r_size;
//    //r_size = sprintf((char*)message, "%s%s&N=%d&V=%d", messagePrefix_.c_str(), payload.c_str(), settings_.loraNodeAddress, battMV);
//    r_size = sprintf((char*)message, "%s%s&N=%d", messagePrefix_.c_str(), payload.c_str(), settings_.loraNodeAddress);
//
//    settings_.loraRetryCount = shouldRetry? 1: 0;
//
//    sendRawMessage(settings_.loraDestAddress, reinterpret_cast<char*>(message), withAck);

    String sentPayload = messagePrefix_ + payload + "&N=" + String(settings_.loraNodeAddress);
    uint8_t buffer[sentPayload.length() + 1];

    sentPayload.getBytes(buffer, sentPayload.length()+1);
    sendRawMessage(settings_.loraDestAddress, buffer, sentPayload.length(), withAck);
}

void DCX_LoRaService::subscribe(String topic) {
}

//void DCX_LoRaService::handleAcknowledge(uint8_t dest, boolean success, const char *origMsg, uint8_t packetNum) {
void DCX_LoRaService::handleAcknowledge(uint8_t dest, boolean success, uint8_t *origPayload, uint16_t origPayloadLength, uint8_t packetNum) {

    if (ackCallback_) {
        //if (!success && retryLastMessage) {
        if (!success && settings_.loraRetryCount > 0) {
            //retryLastMessage = false;
            settings_.loraRetryCount--;
            Serial.println("RETRYING last message");
            //doSendMessage(dest, origMsg);
            doSendMessage(dest, origPayload, origPayloadLength);
        }
        else {

#ifdef LORA_WITH_APPKEY
            String origMsg = String((char*)(origPayload + sizeof(my_appKey)));
#else
            String origMsg = String((char*) message);
#endif

            ackCallback_(success, origMsg, packetNum);
        }
    }
}

//void DCX_LoRaService::sendRawMessage(uint8_t dest, const char *payloadStr, boolean withAck) {
void DCX_LoRaService::sendRawMessage(uint8_t dest, uint8_t *payload, uint16_t pl, boolean withAck) {

//    uint8_t app_key_offset = 0;
//
//    uint8_t message[170];
//
//#ifdef LORA_WITH_APPKEY
//    //TODO: APP_KEY on setting
//    app_key_offset = sizeof(my_appKey);
//	// set the app key in the payload
//	memcpy(message, my_appKey, app_key_offset);
//#endif
//
////#ifdef WITH_AES
////    // leave room for the real payload length byte
////	app_key_offset++;
////#endif
//
//    int r_size;
//    r_size = sprintf((char*) message + app_key_offset, "%s", payloadStr);
//
//#ifdef DEBUG_SERIAL
//    Serial.print(F("Sending "));
//    Serial.println((char*) (message + app_key_offset));
//
//    Serial.print(F("Real payload size is "));
//    Serial.println(r_size);
//#endif
//
//    int pl = r_size + app_key_offset;
//
//    doSendMessage(dest, reinterpret_cast<char*>(message), withAck);

    uint8_t app_key_offset = 0;

#ifdef LORA_WITH_APPKEY

    uint8_t message[pl+sizeof(my_appKey)];

    //TODO: APP_KEY on setting
    app_key_offset = sizeof(my_appKey);
    // set the app key in the payload
    memcpy(message, my_appKey, app_key_offset);
#else
    uint8_t message[pl];

#endif

//#ifdef WITH_AES
//    // leave room for the real payload length byte
//	app_key_offset++;
//#endif

    memcpy(message + app_key_offset, payload, pl);

#ifdef DEBUG_SERIAL
    Serial.print(F("Sending "));
#ifdef LORA_WITH_APPKEY
    Serial.println((char*) (message + app_key_offset));
#else
    Serial.println((char*) message);
#endif

    Serial.print(F("Real payload size is "));
    Serial.println(pl);
#endif

    int allPayloadLength = pl + app_key_offset;

    doSendMessage(dest, message, allPayloadLength, withAck);
}

void DCX_LoRaService::doSendMessage(uint8_t dest, uint8_t *payload, uint16_t payloadLen, boolean withAck) {

    publishing = true;

//#ifdef DEBUG_SERIAL
//    Serial.print(F("Raw: "));
//    Serial.println(reinterpret_cast<char*>(payload));
//#endif

//    int pl;
//    pl = sprintf((char*)message_, "%s", payloadStr);

    long startSend;
    long endSend;
    int e = -1;

    sx1272.CarrierSense();

    startSend = millis();

    sx1272.setPacketType(PKT_TYPE_DATA | PKT_FLAG_DATA_WAPPKEY);

    // Send message to the gateway and print the result
    // with the app key if this feature is enabled
    //e = sx1272.sendPacketTimeout(DEFAULT_DEST_ADDR, message_, pl);

    if (withAck) {
        //e = sx1272.sendPacketTimeoutACK(dest, message_, pl, LORA_ACK_TIMEOUT);//10000);
        e = sx1272.sendPacketTimeoutACK(dest, payload, payloadLen, LORA_ACK_TIMEOUT);//10000);
    } else {
        //e = sx1272.sendPacketTimeout(dest, message_, pl);
        e = sx1272.sendPacketTimeout(dest, payload, payloadLen);
    }

    endSend = millis();

    // save packet number for next packet in case of reboot
    settings_.loraPackageNumber = sx1272._packetNumber;

#ifdef DEBUG_SERIAL
    Serial.print(F("LoRa pkt seq "));
    Serial.println(sx1272.packet_sent.packnum);
#endif

    if (withAck) {
        if (!e) {
            sx1272.getSNR();
            sx1272.getRSSIpacket();

#ifdef DEBUG_SERIAL
            char sprintf_buf[200];
            sprintf(sprintf_buf, "--- rxlora ACK. SNR=%d RSSIpkt=%d\n",
                    sx1272._SNR, sx1272._RSSIpacket);

            Serial.println(sprintf_buf);
#endif

            //handleAcknowledge(dest, true, payloadStr, sx1272._packetNumber);
            handleAcknowledge(dest, true, payload, payloadLen, sx1272._packetNumber);
        }
        else {
            if (e == 3) {
#ifdef DEBUG_SERIAL
                Serial.println(F("NO ACK received!"));
#endif
                //handleAcknowledge(dest, false, payloadStr, sx1272._packetNumber);
                handleAcknowledge(dest, false, payload, payloadLen, sx1272._packetNumber);
            }
        }
#ifdef DEBUG_SERIAL
        Serial.print(F("LoRa (ACK) Sent in: "));
        Serial.println(endSend - startSend);
#endif
    }
    else {
#ifdef DEBUG_SERIAL
        Serial.print(F("LoRa Sent in: "));
        Serial.println(endSend - startSend);
#endif
    }

#ifdef DEBUG_SERIAL

    Serial.print(F("LoRa Sent w/CAD in "));
    Serial.println(endSend - sx1272._startDoCad);

    Serial.print(F("Packet sent, state: "));
    Serial.println(e);

#endif

    publishing = false;
}

void DCX_LoRaService::setMessagePrefix(String msg) {
    messagePrefix_ = msg;
}
