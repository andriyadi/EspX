//
// Created by Andri Yadi on 7/30/16.
//

#include "MakestroCloudClient.h"

MakestroCloudClient::MakestroCloudClient(const char *username, const char *userkey, const char* projectName, const char* deviceId):
        username_(username), userkey_(userkey), projectName_(projectName), deviceId_(deviceId), AsyncMqttClient() {

    char* tempClientId = (char*) malloc(sizeof(char) * 100);

    if (!deviceId) {
        sprintf(tempClientId, "%s-%s-default", username_, projectName_);
    } else {
        strcpy(tempClientId, deviceId);
    }

    setServer(IOTHUB_HOST, IOTHUB_PORT);
    setCredentials(username_, userkey_);
    setClientId(tempClientId);

    setKeepAlive(10);
    setCleanSession(true);

//    subscribedTopics_.reserve(10);
//    subscribedCallbacks_.reserve(10);

//    using namespace std::placeholders;
//    AsyncMqttClient::onMessage(std::bind(&MakestroCloudClient::_onMakestroCloudMessageUserCallback, this, _1, _2, _3, _4, _5, _6));
//    AsyncMqttClient::onMessage(_onMakestroCloudMessageUserCallback);

    AsyncMqttClientInternals::OnMessageUserCallback cb = [=](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        _onIoTHubMqttMessage(topic, payload, properties, len, index, total);
        //Serial.printf("Got topic %s\r\n", topic);
    };
    AsyncMqttClient::onMessage(cb);
}

MakestroCloudClient::MakestroCloudClient() {

}

MakestroCloudClient::~MakestroCloudClient() {

}

void MakestroCloudClient::publish(String topic, String payload) {
    if (username_ != NULL && !topic.startsWith(String(username_))) {
        String project = String(projectName_);
        //project.toLowerCase();
        project.replace(" ", "");
        topic = String(username_) + "/" + project + "/" + topic;
    }

    AsyncMqttClient::publish(topic.c_str(), IOTHUB_DEFAULT_PUBLISH_QOS, true, payload.c_str(), payload.length());
}

void MakestroCloudClient::publishData(String payload) {
    publish("data", payload);
}

void MakestroCloudClient::subscribeWithCallback(String topic, MakestroCloudSubscribedTopicMessageCallback callback) {
    if (username_ != NULL && !topic.startsWith(String(username_))) {
        String project = String(projectName_);
        //project.toLowerCase();
        project.replace(" ", "");
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

void MakestroCloudClient::subscribePropertyWithTopic(String endTopic, String property, MakestroCloudSubscribedPropertyCallback callback) {

    parseMessageAsJson_ = true;

    //only subscribe when no subscribed properties
    if (subscribedProperties_.size() == 0) {
        String project = String(projectName_);
        //project.toLowerCase();
        project.replace(" ", "");
        String topic = String(username_) + "/" + project + "/" + endTopic;

        AsyncMqttClient::subscribe(topic.c_str(), IOTHUB_DEFAULT_SUBSCRIBE_QOS);
    }

    subscribedProperties_[property] = callback;
}

void MakestroCloudClient::subscribeProperty(String property, MakestroCloudSubscribedPropertyCallback callback) {

//    parseMessageAsJson_ = true;
//
//    //only subscribe when no subscribed properties
//    if (subscribedProperties_.size() == 0) {
//        String project = String(projectName_);
//        project.toLowerCase();
//        String topic = String(username_) + "/" + project + "/control";
//
//        AsyncMqttClient::subscribe(topic.c_str(), IOTHUB_DEFAULT_SUBSCRIBE_QOS);
//    }
//
//    subscribedProperties_[property] = callback;

    subscribePropertyWithTopic("control", property, callback);
}

AsyncMqttClient &MakestroCloudClient::onMessage(AsyncMqttClientInternals::OnMessageUserCallback callback) {
    _onMakestroCloudMessageUserCallback = callback;
    return *this;
}

void MakestroCloudClient::_onIoTHubMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {

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
//            MakestroCloudSubscribedTopicMessageCallback cb = subscribedCallbacks_.at(pos);
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
            MakestroCloudSubscribedTopicMessageCallback cb = pos->second;

            String realPayload = "";
            realPayload.reserve(len);
            for (uint32_t i = 0; i < len; i++) {
                realPayload += (char) payload[i];
            }

            cb(String(topic), realPayload);
        }
    }

    if (_onMakestroCloudMessageUserCallback) {
        _onMakestroCloudMessageUserCallback(topic, payload, properties, len, index, total);
    }

    if (parseMessageAsJson_ && subscribedProperties_.size() > 0) {
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(payload);
        if (json.success()) {
            for ( const auto &myPair : subscribedProperties_ ) {
                if (!json.containsKey(myPair.first)) {
                    continue;
                }

                MakestroCloudSubscribedPropertyCallback cb = myPair.second;
                String val = json[myPair.first].asString();
                cb(myPair.first, val);
            }
        }
    }
}

//template<typename Value>
//void MakestroCloudClient::publishKeyValue(String key, Value val) {
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

void MakestroCloudClient::publishMap(JsonKeyValueMap keyValMap, const char *iftttEvent, const char *iftttKey) {
    if (keyValMap.size() == 0) {
        return;
    }

    const int BUFFER_SIZE = JSON_OBJECT_SIZE(20);
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

void MakestroCloudClient::triggerIFTTTEvent(const char *iftttEvent, const char *iftttKey) {
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
