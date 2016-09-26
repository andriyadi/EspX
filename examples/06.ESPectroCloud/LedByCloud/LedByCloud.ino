/*
  Demoing how easy to access cloud (Makestro Cloud in this case) using provided library.
  This sketch demoes how to connect to WiFi, connect to the Cloud, then turning on/off on-board LED from the Cloud, like a boss.

  Created by 22 Sep 2016
  by Andri Yadi
*/

#include <ESPectro.h>
#include <MakestroCloudClient.h>
#include <DCX_AppSetting.h>
#include <DCX_WifiManager.h>

ESPectro board;
MakestroCloudClient makestroCloudClient("[your_username]", "[your_long_token]", "[your_project_name]");

DCX_WifiManager wifiManager(AppSetting);

void onMqttMessage(char *topic, char *payload, uint8_t qos, size_t len, size_t index, size_t total) {
    Serial.println("** Publish received **");
    Serial.print("  topic: ");
    Serial.println(topic);
    Serial.print("  qos: ");
    Serial.println(qos);
    Serial.print("  len: ");
    Serial.println(len);
    Serial.print("  index: ");
    Serial.println(index);
    Serial.print("  total: ");
    Serial.println(total);
}

void onSubscribedPropertyCallback(const String prop, const String value) {
    Serial.print("incoming: ");
    Serial.print(prop);
    Serial.print(" = ");
    Serial.print(value);
    Serial.println();

    if (value.equals("1")) {
        board.turnOnLED();
    }
    else {
        board.turnOffLED();
    }
}

void onMqttConnect() {
    Serial.println("** Connected to the broker **");

//    MakestroCloudSubscribedTopicMessageCallback subsCallback = [=](const String topic, const String payload) {
//        Serial.print("incoming: ");
//        Serial.print(topic);
//        Serial.print(" - ");
//        Serial.print(payload);
//        Serial.println();
//    };
//
//    makestroCloudClient.subscribeWithCallback("control", subsCallback);

//    MakestroCloudSubscribedPropertyCallback propsCallback = [=](const String prop, const String value) {
//        Serial.print("incoming: ");
//        Serial.print(prop);
//        Serial.print(" = ");
//        Serial.print(value);
//        Serial.println();
//
//        if (value.equals("1")) {
//            board.turnOnLED();
//        }
//        else {
//            board.turnOffLED();
//        }
//    };

    //makestroCloudClient.subscribeProperty("switch", propsCallback);
    makestroCloudClient.subscribeProperty("switch", onSubscribedPropertyCallback);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println("** Disconnected from the broker **");
    Serial.println("Reconnecting to MQTT...");
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
    Serial.println("** Subscribe acknowledged **");
    Serial.print("  packetId: ");
    Serial.println(packetId);
    Serial.print("  qos: ");
    Serial.println(qos);
}

// the setup function runs once when you press reset or power the board
void setup() {
    Serial.begin(115200);
    //Wait Serial to be ready
    while (!Serial);

    DEBUG_SERIAL("\r\nInitializing...\r\n\r\n");

    AppSetting.load();
    AppSetting.debugPrintTo(Serial);

    wifiManager.onWifiConnectStarted([]() {
        DEBUG_SERIAL("WIFI CONNECTING STARTED\r\n");
        board.turnOnLED();
    });

    wifiManager.onWifiConnected([](boolean newConn) {
        DEBUG_SERIAL("WIFI CONNECTED\r\n");

        board.turnOffLED();

        makestroCloudClient.onConnect(onMqttConnect);
        makestroCloudClient.onDisconnect(onMqttDisconnect);
        makestroCloudClient.onSubscribe(onMqttSubscribe);
        //makestroCloudClient.onMessage(onMqttMessage);

        makestroCloudClient.connect();
    });

    wifiManager.onWifiConnecting([](unsigned long elapsed) {
        //DEBUG_SERIAL("%d\r\n", elapsed);
        board.toggleLED();
    });

    wifiManager.onWifiDisconnected([](WiFiDisconnectReason reason) {
        DEBUG_SERIAL("WIFI GIVE UP\r\n");
        board.turnOffLED();
    });

    //wifiManager.begin();
    wifiManager.begin("your-ssid-name", "your-ssid-password");
}

// the loop function runs over and over again forever
void loop() {
    wifiManager.loop();
}
