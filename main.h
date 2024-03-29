/**
 * @file main.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Globals and Includes
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#ifndef _MAIN_H_
#define _MAIN_H_

// Debug
// Debug output set to 0 to disable app debug output
#ifndef MY_DEBUG
#define MY_DEBUG 1
#endif

#if MY_DEBUG > 0
#define MYLOG(tag, ...)                  \
	do                                   \
	{                                    \
		if (tag)                         \
			Serial.printf("[%s] ", tag); \
		Serial.printf(__VA_ARGS__);      \
		Serial.printf("\n");             \
	} while (0);                         \
	delay(100)
#else
#define MYLOG(...)
#endif

#ifndef RAK_REGION_AS923_2
#define RAK_REGION_AS923_2 9
#endif
#ifndef RAK_REGION_AS923_3
#define RAK_REGION_AS923_3 10
#endif
#ifndef RAK_REGION_AS923_4
#define RAK_REGION_AS923_4 11
#endif

// Globals
extern char g_dev_name[];
extern bool g_has_rak15001;

/** Settings valid marker */
#define LORAWAN_DATA_MARKER 0x55

/** Structure for the device setup */
struct s_lorawan_settings
{
#ifdef _VARIANT_RAK3172_
#if RUI_DEV == 1
	// OTAA Device EUI MSB  // ac1f09fff8052F72
	uint8_t node_device_eui[8] = {0xac, 0x1f, 0x09, 0xff, 0xf8, 0x05, 0x2F, 0x72};
#else
	// OTAA Device EUI MSB  // ac1f09fffe0537a1
	uint8_t node_device_eui[8] = {0xac, 0x1f, 0x09, 0xff, 0xfe, 0x05, 0x37, 0xa1};
#endif
	// OTAA Application EUI MSB
	uint8_t node_app_eui[8] = {0xac, 0x1f, 0x09, 0xff, 0xf8, 0x68, 0x31, 0x72};
	// OTAA Application Key MSB  // efadff29c77b4829acf71e1a6e76f713
	uint8_t node_app_key[16] = {0xef, 0xad, 0xff, 0x29, 0xc7, 0x7b, 0x48, 0x29, 0xac, 0xf7, 0x1e, 0x1a, 0x6e, 0x76, 0xf7, 0x13};
#endif
#ifdef _VARIANT_RAK4630_
	// OTAA Device EUI MSB // ac1f09fffe057110
	uint8_t node_device_eui[8] = {0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x05, 0x71, 0x10};
	// OTAA Application EUI MSB
	uint8_t node_app_eui[8] = {0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x05, 0x71, 0x10};
	// OTAA Application Key MSB // 2B84E0B09B68E5CB42176FE753DCEE79
	uint8_t node_app_key[16] = {0x2B, 0x84, 0xE0, 0xB0, 0x9B, 0x68, 0xE5, 0xCB, 0x42, 0x17, 0x6F, 0xE7, 0x53, 0xDC, 0xEE, 0x79};
#endif
	// Flag for OTAA or ABP
	bool otaa_enabled = true;
	// Flag for ADR on or off
	bool adr_enabled = false;
	// send interval, default is off
	uint32_t send_repeat_time = 0;
	// TX power 0 .. 10
	uint8_t tx_power = 0;
	// Data rate 0 .. 15 (validity depnends on Region)
	uint8_t data_rate = 3;
	// Subband channel selection 1 .. 9
	uint8_t subband_channels = 1;
	// Flag to enable confirmed messages
	bool confirmed_msg_enabled = false;
#ifdef _VARIANT_RAK3172_
	// Fixed LoRaWAN lorawan_region
	uint8_t lora_region = RAK_REGION_AS923_3;
#endif
#ifdef _VARIANT_RAK4630_
	// Fixed LoRaWAN lorawan_region
	uint8_t lora_region = RAK_REGION_EU868;
#endif
};

extern s_lorawan_settings g_lorawan_settings;
void log_settings(void);

/** Module stuff */
#include "module_handler.h"
#endif // _MAIN_H_
