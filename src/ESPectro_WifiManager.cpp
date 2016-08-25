//
// Created by Andri Yadi on 8/25/16.
//

#include "ESPectro_WifiManager.h"

Ticker *tickerCheckConn = NULL, *tickerSmartConfig = NULL;
volatile byte wifiConnTrial_ = 0;
volatile boolean smartConfigRequested_ = false;

void cleanupTickerCheckConn() {
    if (tickerCheckConn != NULL) {
        tickerCheckConn->detach();

        delete tickerCheckConn;
        tickerCheckConn = NULL;
    }
}

void cleanupTickerSmartConfig() {
    if (tickerSmartConfig != NULL) {
        tickerSmartConfig->detach();

        delete tickerSmartConfig;
        tickerSmartConfig = NULL;
    }
}

void checkWifiConn(ESPectro_WifiManager *wifiMgr) {
    bool _giveUp = false;

    int timeout = !smartConfigRequested_? (30000 / (500)): (120000 / (500));

    if (wifiConnTrial_ > timeout) {
#ifdef DEBUG_SERIAL
        Serial.println(F("\r\nWIFI connection is timeout\r\n"));
#endif
        _giveUp = true;
    }

    if (smartConfigRequested_) {
        //Serial.print("x");
        if (WiFi.smartConfigDone()){
#ifdef DEBUG_SERIAL
            Serial.println(F("SmartConfig Success"));
#endif
            wifiMgr->setWifiConnected(true);
            cleanupTickerSmartConfig();

            return;
        }
    }
    else {
        //uint8_t stat = WiFi.waitForConnectResult();
        uint8_t stat = WiFi.status();

        /*if (stat == WL_DISCONNECTED) {
            DEBUG_SERIAL("Disconnected!\r\n");
            _giveUp = true;
        }
        else */
        if (stat == WL_NO_SSID_AVAIL) {
            DEBUG_SERIAL("SSID is GONE!\r\n");
            WiFi.disconnect();
            _giveUp = true;
        }
        else if (stat == WL_CONNECTED) {

#ifdef DEBUG_SERIAL
            Serial.println(F("\r\nYay!!! WIFI is connected\r\n"));
#endif
            wifiMgr->setWifiConnected(true);
            cleanupTickerCheckConn();

            return;
        }
    }

    if (_giveUp) {
        if (smartConfigRequested_) {
            WiFi.stopSmartConfig();
            cleanupTickerSmartConfig();
        }
        else {
            cleanupTickerCheckConn();
        }

        wifiMgr->setWifiConnected(false);

    }
    else {
        DEBUG_SERIAL(".");
        wifiConnTrial_++;
        wifiMgr->setWifiConnecting();
    }
}

ESPectro_WifiManager::ESPectro_WifiManager(ESPectro_AppSetting &setting):
        setting_(setting) {

}

ESPectro_WifiManager::~ESPectro_WifiManager() {
    cleanupTickerCheckConn();
    cleanupTickerSmartConfig();
}

void ESPectro_WifiManager::begin() {
    WiFi.mode(WIFI_STA);

    if (!setting_.exist() || !setting_.wifiConfigured || SETTING_FORCE_INIT)
    {
        startSmartConfig();
    }
    else {// Run our method when station was connected to AP (or not connected)
        tryToConnectWifi();
    }
}

void ESPectro_WifiManager::loop() {

    if (connectingToWifi_ && !connectedToWifi_) {
        Serial.print("X");
        if (wifiConnectingCallback_) {
            wifiConnectingCallback_(wifiConnTrial_*500);
        }
        return;
    }

    if (millis() - wifiConnCheckingMillis_ > WIFI_CHECK_CONNECTION_INTERVAL && setting_.wifiConfigured) {
        connectedToWifi_ = WiFi.isConnected();
        wifiConnCheckingMillis_ = millis();

        if (!connectedToWifi_) {
            //wifiConnectRequested_ = true;
            tryToConnectWifi();
        }
    }
}

void ESPectro_WifiManager::startSmartConfig() {

    DEBUG_SERIAL("DAMN, RECONFIG WIFI\n");

    WiFi.disconnect();

    setting_.wifiConfigured = false; //make sure

    bool success = WiFi.beginSmartConfig();
    if (!success) {
        DEBUG_SERIAL("DAMN, SMART CONFIG FAILED\n");
    }
    else {
        smartConfigRequested_ = true;
        wifiConnTrial_ = 0;

        if (tickerSmartConfig == NULL) {
            tickerSmartConfig = new Ticker();
        }

        tickerSmartConfig->detach();
        delay(10);

        if (wifiConnectStartedCallback_) {
            wifiConnectStartedCallback_();
        }

        tickerSmartConfig->attach(0.5, checkWifiConn, this);
    }
}

void ESPectro_WifiManager::OnWifiConnectStarted(WifiConnectionCallback cb) {
    wifiConnectStartedCallback_ = cb;
}

void ESPectro_WifiManager::OnWifiConnected(WifiConnectedCallback cb) {
    wifiConnectedHandler_ = cb;
}

void ESPectro_WifiManager::OnWifiDisconnected(WifiConnectionCallback cb) {
    wifiDisconnectedHandler_ = cb;
}

void ESPectro_WifiManager::OnWifiConnecting(WifiConnectingCallback cb) {
    wifiConnectingCallback_ = cb;
}


void ESPectro_WifiManager::setWifiConnecting() {
    if (wifiConnectingCallback_) {
        wifiConnectingCallback_(wifiConnTrial_*500);
    }
}

void ESPectro_WifiManager::setWifiConnected(boolean connected) {

    if (connected) {

        connectedToWifi_ = true;
        connectingToWifi_ = false;
        smartConfigRequested_ = false;

#ifdef DEBUG_SERIAL
        WiFi.printDiag(Serial);
#endif

        boolean newConnection = (setting_.wifiConfigured == 0);

        IPAddress _localIP = WiFi.localIP();
        setting_.ipAddr = _localIP;
        setting_.wifiConfigured = true;

        static struct station_config conf;
        wifi_station_get_config(&conf);
        const char* ssid = reinterpret_cast<const char*>(conf.ssid);
        const char* passphrase = reinterpret_cast<const char*>(conf.password);

#ifdef DEBUG_SERIAL
        Serial.printf("WiFi SSID: %s, Pass: %s\r\n", ssid, passphrase);
#endif

        setting_.ssidName = String(ssid);
        setting_.ssidPass = String(passphrase);

        setting_.save();

        if (wifiConnectedHandler_) {
            wifiConnectedHandler_(newConnection);
        }
    }
    else {

        connectingToWifi_ = false;
        connectedToWifi_ = false;
        smartConfigRequested_ = false;

        if (wifiDisconnectedHandler_) {
            wifiDisconnectedHandler_();
        }

        startSmartConfig();
    }
}

void ESPectro_WifiManager::tryToConnectWifi() {

    DEBUG_SERIAL("YUHU, WIFI CONFIG READY! %s:%s\n", setting_.ssidName.c_str(), setting_.ssidPass.c_str());
    //WiFiMulti.addAP(settings_.ssidName.c_str(), settings_.ssidPass.c_str()); // Put you SSID and Password here
    //WifiStation.waitConnection(connectedDelegate_, 30, notConnectedDelegate_); // We recommend 20+ seconds at start

    connectedToWifi_ = false;
    connectingToWifi_ = true;
    wifiConnTrial_ = 0;

    WiFi.begin(setting_.ssidName.c_str(), setting_.ssidPass.c_str());


    if (tickerCheckConn == NULL) {
        tickerCheckConn = new Ticker();
    }

    tickerCheckConn->detach();
    delay(10);

    if (wifiConnectStartedCallback_) {
        wifiConnectStartedCallback_();
    }
    tickerCheckConn->attach(0.5, checkWifiConn, this);
}

bool ESPectro_WifiManager::isWifiConnected() {
    return connectedToWifi_;
}


