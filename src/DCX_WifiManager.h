//
// Created by Andri Yadi on 8/25/16.
//

#ifndef XBOARD_ESPECTRO_WIFIMANAGER_H
#define XBOARD_ESPECTRO_WIFIMANAGER_H

#include <Arduino.h>
#include "DCX_AppSetting.h"
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

class DCX_WifiManager {
public:
    DCX_WifiManager(DCX_AppSetting &setting);
    ~DCX_WifiManager();

    void begin();
    void loop();

    //void setConnectedDelegate(ConnectionDelegate connectedDelegate);
    void startSmartConfig();

    void onWifiConnectStarted(WifiConnectionCallback cb);
    void onWifiConnected(WifiConnectedCallback cb);
    void onWifiDisconnected(WifiConnectionCallback cb);
    void onWifiConnecting(WifiConnectingCallback cb);

    bool isWifiConnected();

    void setWifiConnected(boolean connected);
    void setWifiConnecting();

private:
    DCX_AppSetting &setting_;

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
