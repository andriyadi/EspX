//
// Created by Andri Yadi on 8/26/16.
//

#ifndef XBOARD_DCX_CONSTANTS_H
#define XBOARD_DCX_CONSTANTS_H

#define DEBUG_SERIAL(...)   Serial.printf( __VA_ARGS__ )

#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL(...)
#else
#define DEBUG_SERIAL_LN     Serial.println()
#endif

#define SETTING_DEFAULT_AP_NAME             "ESPectro_000"
#define SETTING_DEFAULT_AP_PASS             "11223344"
#define SETTING_FORCE_INIT                  false

#define WIFI_CONNECTING_INTERVAL            500

#define LORA_SUPPORT

#ifdef LORA_SUPPORT

// IMPORTANT
// Change this to avoid conflict
#define LORA_NODE_ADDRESS 				    7

// IMPORTANT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// please uncomment only 1 choice
//
// it seems that both HopeRF and Modtronix board use the PA_BOOST pin and not the RFO. Therefore, for these
// boards we set the initial power to 'x' and not 'M'. This is the purpose of the define statement
//
// uncomment if your radio is an HopeRF RFM92W or RFM95W
#define LORA_RADIO_RFM92_95
// uncomment if your radio is a Modtronix inAir9B (the one with +20dBm features), if inAir9, leave commented
//#define LORA_RADIO_INAIR9B
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// IMPORTANT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// please uncomment only 1 choice
#define LORA_BAND868
//#define LORA_BAND900
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LORA_WITH_APPKEY
#define LORA_DEFAULT_DEST_ADDR 		1
#define LORA_LORAMODE  				4
#define LORA_ACK_TIMEOUT		    3000
#define LORA_RCV_TIMEOUT		    400
#define LORA_MSG_PREFIX             "\\#"

#endif

#endif //XBOARD_DCX_CONSTANTS_H
