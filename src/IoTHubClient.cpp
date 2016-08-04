//
// Created by Andri Yadi on 7/30/16.
//

#include "IoTHubClient.h"

IoTHubClient::IoTHubClient(const char *username, const char *userkey, const char* projectName):
        username_(username), userkey_(userkey), projectName_(projectName), AsyncMqttClient() {

    setServer(IOTHUB_HOST, IOTHUB_PORT);
    setCredentials(username_, userkey_);
    setKeepAlive(5);

//    subscribedTopics_.reserve(10);
//    subscribedCallbacks_.reserve(10);

//    using namespace std::placeholders;
//    AsyncMqttClient::onMessage(std::bind(&IoTHubClient::_onIotHubMessageUserCallback, this, _1, _2, _3, _4, _5, _6));
//    AsyncMqttClient::onMessage(_onIotHubMessageUserCallback);

    AsyncMqttClientInternals::OnMessageUserCallback cb = [=](char* topic, char* payload, uint8_t qos, size_t len, size_t index, size_t total) {
        _onIoTHubMqttMessage(topic, payload, qos, len, index, total);
        //Serial.printf("Got topic %s\r\n", topic);
    };
    AsyncMqttClient::onMessage(cb);
}

IoTHubClient::IoTHubClient() {

}

IoTHubClient::~IoTHubClient() {

}

void IoTHubClient::publish(String topic, String payload) {
    if (username_ != NULL && !topic.startsWith(String(username_))) {
        String project = String(projectName_);
        project.toLowerCase();
        topic = String(username_) + "/" + project + "/" + topic;
    }

    AsyncMqttClient::publish(topic.c_str(), IOTHUB_DEFAULT_PUBLISH_QOS, true, payload.c_str(), payload.length());
}

void IoTHubClient::publishData(String payload) {
    publish("data", payload);
}

void IoTHubClient::subscribeWithHandler(String topic, IoTHubSubscribedTopicMessageCallback callback) {
    if (username_ != NULL && !topic.startsWith(String(username_))) {
        String project = String(projectName_);
        project.toLowerCase();
        topic = String(username_) + "/" + project + "/" + topic;
    }

//    subscribedTopics_.push_back(topic);
//    subscribedCallbacks_.push_back(callback);

    subscribedTopics_[topic] = callback;

//    AsyncMqttClientInternals::OnMessageUserCallback cb = [=](char* topic, char* payload, uint8_t qos, size_t len, size_t index, size_t total) {
//        //if (strcmp())
//    };

    AsyncMqttClient::subscribe(topic.c_str(), IOTHUB_DEFAULT_SUBSCRIBE_QOS);
}

void IoTHubClient::subscribeProperty(String property, IoTHubSubscribedPropertyCallback callback) {

    parseMessageAsJson_ = true;

    //only subscribe when no subscribed properties
    if (subscribedProperties_.size() == 0) {
        String project = String(projectName_);
        project.toLowerCase();
        String topic = String(username_) + "/" + project + "/control";

        AsyncMqttClient::subscribe(topic.c_str(), IOTHUB_DEFAULT_SUBSCRIBE_QOS);
    }

    subscribedProperties_[property] = callback;
}

AsyncMqttClient &IoTHubClient::onMessage(AsyncMqttClientInternals::OnMessageUserCallback callback) {
    _onIotHubMessageUserCallback = callback;
    return *this;
}

void IoTHubClient::_onIoTHubMqttMessage(char* topic, char* payload, uint8_t qos, size_t len, size_t index, size_t total) {

    Serial.printf("Got topic %s\r\n", topic);

//    if (subscribedCallbacks_.capacity() > 0) {
//
//        int pos = -1;
////    for (std::vector<String>::iterator it = subscribedTopics_.begin(); it != subscribedTopics_.end(); ++it) {
////
////    }
//        int idx = 0;
//        for (auto i: subscribedTopics_) {
//            if (i.equals(topic)) {
//                pos = idx;
//                break;
//            }
//            idx++;
//        }
//
//        Serial.printf("Found topic at idx: %d\r\n", pos);
//
//        if (pos != -1) {
//            IoTHubSubscribedTopicMessageCallback cb = subscribedCallbacks_.at(pos);
//            String realPayload = "";
//            realPayload.reserve(len);
//            for (uint32_t i = 0; i < len; i++) {
//                realPayload += (char) payload[i];
//            }
//
//            cb(String(topic), realPayload);
//        }
//    }

    if (subscribedTopics_.size() > 0) {
        TopicsHandlerMap_t::const_iterator pos = subscribedTopics_.find(topic);
        if (pos == subscribedTopics_.end()) {
            //not found
            Serial.println(F("Topic not found"));
        }
        else {
            IoTHubSubscribedTopicMessageCallback cb = pos->second;

            String realPayload = "";
            realPayload.reserve(len);
            for (uint32_t i = 0; i < len; i++) {
                realPayload += (char) payload[i];
            }

            cb(String(topic), realPayload);
        }
    }

    if (_onIotHubMessageUserCallback) {
        _onIotHubMessageUserCallback(topic, payload, qos, len, index, total);
    }

    if (parseMessageAsJson_ && subscribedProperties_.size() > 0) {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(payload);
        if (json.success()) {
            for ( const auto &myPair : subscribedProperties_ ) {
                if (!json.containsKey(myPair.first)) {
                    continue;
                }

                IoTHubSubscribedPropertyCallback cb = myPair.second;
                String val = json[myPair.first].asString();
                cb(myPair.first, val);
            }
        }
    }
}

//template<typename Value>
//void IoTHubClient::publishKeyValue(String key, Value val) {
//    const int BUFFER_SIZE = JSON_OBJECT_SIZE(1);
//    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
//    JsonObject &root = jsonBuffer.createObject();
//    root[key] = val;
//
//    String jsonStr;
//    root.printTo(jsonStr);
//    Serial.println(jsonStr);
//
//    publishData(jsonStr);
//}

void IoTHubClient::publishMap(JsonKeyValueMap keyValMap, const char *iftttEvent, const char *iftttKey) {
    if (keyValMap.size() == 0) {
        return;
    }

    const int BUFFER_SIZE = JSON_OBJECT_SIZE(10);
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();

    for (const auto &keyVal: keyValMap) {
        root[keyVal.first] = keyVal.second;
    }

    if (iftttEvent != NULL) {
        root["ifttt_event"] = iftttEvent;
    }

    if (iftttKey != NULL) {
        root["ifttt_key"] = iftttKey;
    }

    String jsonStr;
    root.printTo(jsonStr);
    Serial.println(jsonStr);

    publishData(jsonStr);
}

void IoTHubClient::triggerIFTTTEvent(const char *iftttEvent, const char *iftttKey) {
    const int BUFFER_SIZE = JSON_OBJECT_SIZE(3);
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["ifttt_event"] = iftttEvent;

    if (iftttKey != NULL) {
        root["ifttt_key"] = iftttKey;
    }

    String jsonStr;
    root.printTo(jsonStr);
    Serial.println(jsonStr);

    publishData(jsonStr);
}
