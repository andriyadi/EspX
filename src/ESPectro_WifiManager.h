//
// Created by Andri Yadi on 8/25/16.
//

#ifndef XBOARD_ESPECTRO_WIFIMANAGER_H
#define XBOARD_ESPECTRO_WIFIMANAGER_H

#include <Arduino.h>
#include "ESPectro_AppSetting.h"
#include <ESP8266WiFi.h>
#include <functional>
#include <Ticker.h>

extern "C" {
#include "user_interface.h"
#include "include/wl_definitions.h"
}

typedef std::function<void()> WifiConnectionCallback;
typedef std::function<void(boolean)> WifiConnectedCallback;
typedef std::function<void(unsigned long elapsedTime)> WifiConnectingCallback;

class ESPectro_WifiManager {
public:
    ESPectro_WifiManager(ESPectro_AppSetting &setting);
    ~ESPectro_WifiManager();

    void begin();
    void loop();

    //void setConnectedDelegate(ConnectionDelegate connectedDelegate);
    void startSmartConfig();

    void OnWifiConnectStarted(WifiConnectionCallback cb);
    void OnWifiConnected(WifiConnectedCallback cb);
    void OnWifiDisconnected(WifiConnectionCallback cb);
    void OnWifiConnecting(WifiConnectingCallback cb);

    bool isWifiConnected();

    void setWifiConnected(boolean connected);
    void setWifiConnecting();

private:
    ESPectro_AppSetting &setting_;

    //volatile boolean wifiConnectRequested_ = false;
    volatile boolean connectedToWifi_ = false;
    volatile boolean connectingToWifi_ = false;
    volatile unsigned long wifiConnCheckingMillis_ = 0;

    WifiConnectionCallback wifiConnectStartedCallback_;
    WifiConnectedCallback wifiConnectedHandler_;
    WifiConnectionCallback wifiDisconnectedHandler_;
    WifiConnectingCallback wifiConnectingCallback_;

    void tryToConnectWifi();
};


#endif //XBOARD_ESPECTRO_WIFIMANAGER_H
