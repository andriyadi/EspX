/*
  Blink on-board Neopixel RGB LED using ESPectro library
  Turns on-board Neopixel RGB LED on for one second, then off for one second, repeatedly.

  Created by 22 Sep 2016
  by Andri Yadi
*/

#include <ESPectro.h>
#include <DCX_AppSetting.h>
#include <DCX_WifiManager.h>

ESPectro board;
DCX_WifiManager wifiManager(AppSetting);

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
    });

    wifiManager.onWifiConnecting([](unsigned long elapsed) {
        //DEBUG_SERIAL("%d\r\n", elapsed);
        board.toggleLED();
    });

    wifiManager.onWifiDisconnected([](WiFiDisconnectReason reason) {
        DEBUG_SERIAL("WIFI GIVE UP\r\n");
        board.turnOffLED();
    });

    wifiManager.begin("your-ssid-name", "your-ssid-password");

    //Alternatively, you can just begin and use ESP8266 Smart Config iOS/Android app to configure WiFi
    //I recommend to use this Android app: https://play.google.com/store/apps/details?id=com.cmmakerclub.iot.esptouch
    //wifiManager.begin();

}

// the loop function runs over and over again forever
void loop() {
    wifiManager.loop();
}
