//
// Created by Andri Yadi on 8/25/16.
//

#ifndef DYCODEX_APPSETTING_H
#define DYCODEX_APPSETTING_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <IPAddress.h>
#include "DCX_Constants.h"

#define APP_SETTINGS_FILE "/espectro_setting.json" // leading point for security reasons :)

struct DCX_AppSetting
{
    String ssidName;
    String ssidPass;
    IPAddress ipAddr;

    bool wifiConfigured = false;
    //bool configInited = false;
    String apName = String(SETTING_DEFAULT_AP_NAME);
    String apPass = String(SETTING_DEFAULT_AP_PASS);
    uint16_t utc = 7;

    boolean load()
    {
        if (!SPIFFS.begin()) {
            DEBUG_SERIAL("SPIFFS init failed!\r\n");
            return false;
        }

        if (SETTING_FORCE_INIT) {
            DEBUG_SERIAL("Removing setting file\r\n");
            SPIFFS.remove(APP_SETTINGS_FILE);
        }

        if (exist())
        {
            File f = SPIFFS.open(APP_SETTINGS_FILE, "r");
            if (!f) {
                DEBUG_SERIAL("File open failed\r\n");
                return false;
            }

//            const int BUFFER_SIZE = JSON_OBJECT_SIZE(8);
//            StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
            DynamicJsonBuffer jsonBuffer;

            String jsonString = f.readString();
            JsonObject& root = jsonBuffer.parseObject(jsonString);

            JsonObject& config = root["config"];

            ssidName = config["ssidName"].as<char*>();
            ssidPass = config["ssidPass"].as<char*>();
            ipAddr = IPAddress();
            ipAddr.fromString(config["ipAddr"].as<char*>());

            wifiConfigured = config["wifiConfigured"];
            //configInited = config["configInited"];
            apName = config["apName"].as<char*>();
            apPass = config["apPass"].as<char*>();

            utc = config["utc"];

            //delete[] jsonString;
        }
        else {
            save();
        }

        return true;
    }

    boolean save()
    {
//        const int BUFFER_SIZE = JSON_OBJECT_SIZE(8);
//        StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
        DynamicJsonBuffer jsonBuffer;

        JsonObject& root = jsonBuffer.createObject();

        JsonObject& config = jsonBuffer.createObject();
        root["config"] = config;
        config["ssidName"] = ssidName.c_str();
        config["ssidPass"] = ssidPass.c_str();
        config["ipAddr"] = ipAddr.toString();
        config["wifiConfigured"] = wifiConfigured;
        //config["configInited"] = configInited;
        config["apName"] = apName.c_str();
        config["apPass"] = apPass.c_str();
        config["utc"] = utc;

        //TODO: add direct file stream writing
        String rootString;
        root.printTo(rootString);

        //fileSetContent(APP_SETTINGS_FILE, rootString);

        File out = SPIFFS.open(APP_SETTINGS_FILE, "w+");
        if (!out) {
            DEBUG_SERIAL("Failed to open setting file for writing!\r\n");
            return false;
        }

        out.print(rootString);

        return true;
    }

    void debugPrintTo(Print &p) {
        p.printf("wifiConfigured: %d\r\n", wifiConfigured);
        //p.printf("ssidName: %s\r\n", ssidName.c_str());

#ifdef LORA_SUPPORT
        p.printf("loraNodeAddress: %d\r\n", loraNodeAddress);
        p.printf("loraMode: %d\r\n", loraMode);
#endif

    }

    void parse(String jsonMessage)
    {
//        const int BUFFER_SIZE = JSON_OBJECT_SIZE(8);
//        StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
        DynamicJsonBuffer jsonBuffer;

        JsonObject& root = jsonBuffer.parseObject(jsonMessage);

        JsonObject& config = root["config"];

        if (config.containsKey("ssidName")) {
            ssidName = config["ssidName"].as<char*>();
        }
        if (config.containsKey("utc")) {
            utc = config["utc"];
        }
    }

    bool exist() { return SPIFFS.exists(APP_SETTINGS_FILE); }
};

static DCX_AppSetting AppSetting;

#endif //DYCODEX_APPSETTING_H
