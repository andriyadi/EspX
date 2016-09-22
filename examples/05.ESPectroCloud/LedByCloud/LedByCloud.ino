/*
  Blink on-board Neopixel RGB LED using ESPectro library
  Turns on-board Neopixel RGB LED on for one second, then off for one second, repeatedly.

  Created by 22 Sep 2016
  by Andri Yadi
*/

#include <ESPectro.h>
#include <IoTHubClient.h>
#include <DCX_AppSetting.h>
#include <DCX_WifiManager.h>

ESPectro board;
IoTHubClient ioTHubClient("andri", "vByOlaA1UZyv8Iw7O9T64F55d9hH7iXZ1o1xkZVogPwVDjK42alDsFInJVb4gGL4", "procode");

DCX_WifiManager wifiManager(AppSetting);

int lastAdcLdr = 0;
const int LED_THRESHOLD = 500;

void onMqttMessage(char* topic, char* payload, uint8_t qos, size_t len, size_t index, size_t total) {
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

void onMqttConnect() {
    Serial.println("** Connected to the broker **");

//    IoTHubSubscribedTopicMessageCallback subsCallback = [=](const String topic, const String payload) {
//        Serial.print("incoming: ");
//        Serial.print(topic);
//        Serial.print(" - ");
//        Serial.print(payload);
//        Serial.println();
//    };
//
//    ioTHubClient.subscribeWithCallback("control", subsCallback);

    IoTHubSubscribedPropertyCallback propsCallback = [=](const String prop, const String value) {
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
    };

    ioTHubClient.subscribeProperty("switch", propsCallback);
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
  while(!Serial);
  
  DEBUG_SERIAL("\r\nInitializing...\r\n\r\n");
  //WiFi.forceSleepBegin();

  AppSetting.load();
  AppSetting.debugPrintTo(Serial);

  wifiManager.onWifiConnectStarted([]() {
      DEBUG_SERIAL("WIFI CONNECTING STARTED\r\n");
      board.turnOnLED();
  });

  wifiManager.onWifiConnected([](boolean newConn) {
      DEBUG_SERIAL("WIFI CONNECTED\r\n");

      board.turnOffLED();

      ioTHubClient.onConnect(onMqttConnect);
      ioTHubClient.onDisconnect(onMqttDisconnect);
      ioTHubClient.onSubscribe(onMqttSubscribe);
      //ioTHubClient.onMessage(onMqttMessage);

      ioTHubClient.connect();
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
  wifiManager.begin("DyWare-AP4", "p@ssw0rd");
}

// the loop function runs over and over again forever
void loop() {
  wifiManager.loop();
}
