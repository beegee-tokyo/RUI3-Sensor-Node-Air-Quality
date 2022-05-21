/**
 * @file custom_at.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief
 * @version 0.1
 * @date 2022-05-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "main.h"

// Forward declarations
int freq_send_handler(SERIAL_PORT port, char *cmd, stParam *param);

/**
 * @brief Add send-frequency AT command
 *
 * @return true if success
 * @return false if failed
 */
bool init_frequency_at(void)
{
	return api.system.atMode.add((char *)"SENDFREQ",
								 (char *)"Set/Get the frequent automatic sending time values in seconds 0 = off, max 2,147,483 seconds",
								 (char *)"SENDFREQ", freq_send_handler);
}

/**
 * @brief Handler for send frequency AT commands
 *
 * @param port Serial port used
 * @param cmd char array with the received AT command
 * @param param char array with the received AT command parameters
 * @return int result of command parsing
 * 			AT_OK AT command & parameters valid
 * 			AT_PARAM_ERROR command or parameters invalid
 */
int freq_send_handler(SERIAL_PORT port, char *cmd, stParam *param)
{
	if (param->argc == 1 && !strcmp(param->argv[0], "?"))
	{
		Serial.print(cmd);
		Serial.printf("=%lds\r\n", g_lorawan_settings.send_repeat_time / 1000);
	}
	else if (param->argc == 1)
	{
		MYLOG("AT_CMD", "param->argv[0] >> %s", param->argv[0]);
		for (int i = 0; i < strlen(param->argv[0]); i++)
		{
			if (!isdigit(*(param->argv[0] + i)))
			{
				MYLOG("AT_CMD", "%d is no digit", i);
				return AT_PARAM_ERROR;
			}
		}

		uint32_t new_send_freq = strtoul(param->argv[0], NULL, 10);

		MYLOG("AT_CMD", "Requested frequency %ld", new_send_freq);

		g_lorawan_settings.send_repeat_time = new_send_freq * 1000;

		MYLOG("AT_CMD", "New frequency %ld", g_lorawan_settings.send_repeat_time);
		// Stop the timer
		udrv_timer_stop(TIMER_0);
		if (g_lorawan_settings.send_repeat_time != 0)
		{
			// Restart the timer
			udrv_timer_start(TIMER_0, g_lorawan_settings.send_repeat_time, NULL);
		}
		// Save custom settings
		save_at_setting(SEND_FREQ_OFFSET);
	}
	else
	{
		return AT_PARAM_ERROR;
	}

	return AT_OK;
}

/**
 * @brief Get setting from flash
 *
 * @param setting_type type of setting, valid values
 * 			GNSS_OFFSET for GNSS precision and data format
 * @return true read from flash was successful
 * @return false read from flash failed or invalid settings type
 */
bool get_at_setting(uint32_t setting_type)
{
	uint8_t flash_value[16];
	switch (setting_type)
	{
	// case GNSS_OFFSET:
	// 	if (!api.system.flash.get(GNSS_OFFSET, flash_value, 2))
	// 	{
	// 		MYLOG("AT_CMD", "Failed to read GNSS value from Flash");
	// 		return false;
	// 	}
	// 	if (flash_value[1] != 0xAA)
	// 	{
	// 		MYLOG("AT_CMD", "Invalid GNSS settings, using default");
	// 		gnss_format = 0;
	// 		save_at_setting(GNSS_OFFSET);
	// 	}
	// 	if (flash_value[0] > HELIUM_MAPPER)
	// 	{
	// 		MYLOG("AT_CMD", "No valid GNSS value found, set to default, read 0X%0X", flash_value[0]);
	// 		gnss_format = 0;
	// 		return false;
	// 	}
	// 	gnss_format = flash_value[0];
	// 	MYLOG("AT_CMD", "Found GNSS format to %d", flash_value[0]);
	// 	return true;
	// 	break;
	case SEND_FREQ_OFFSET:
		if (!api.system.flash.get(SEND_FREQ_OFFSET, flash_value, 5))
		{
			// MYLOG("AT_CMD", "Failed to read send frequency from Flash");
			return false;
		}
		if (flash_value[4] != 0xAA)
		{
			// MYLOG("AT_CMD", "No valid send frequency found, set to default, read 0X%02X 0X%02X 0X%02X 0X%02X",
			// 	  flash_value[0], flash_value[1],
			// 	  flash_value[2], flash_value[3]);
			g_lorawan_settings.send_repeat_time = 0;
			save_at_setting(SEND_FREQ_OFFSET);
			return false;
		}
		// MYLOG("AT_CMD", "Read send frequency 0X%02X 0X%02X 0X%02X 0X%02X",
		// 	  flash_value[0], flash_value[1],
		// 	  flash_value[2], flash_value[3]);
		g_lorawan_settings.send_repeat_time = 0;
		g_lorawan_settings.send_repeat_time |= flash_value[0] << 0;
		g_lorawan_settings.send_repeat_time |= flash_value[1] << 8;
		g_lorawan_settings.send_repeat_time |= flash_value[2] << 16;
		g_lorawan_settings.send_repeat_time |= flash_value[3] << 24;
		// MYLOG("AT_CMD", "Send frequency found %ld", g_lorawan_settings.send_repeat_time);
		return true;
		break;
	default:
		return false;
	}
}

/**
 * @brief Save setting to flash
 *
 * @param setting_type type of setting, valid values
 * 			GNSS_OFFSET for GNSS precision and data format
 * @return true write to flash was successful
 * @return false write to flash failed or invalid settings type
 */
bool save_at_setting(uint32_t setting_type)
{
	uint8_t flash_value[16] = {0};
	bool wr_result = false;
	switch (setting_type)
	{
	// case GNSS_OFFSET:
	// 	flash_value[0] = gnss_format;
	// 	flash_value[1] = 0xAA;
	// 	return api.system.flash.set(GNSS_OFFSET, flash_value, 2);
	// 	break;
	case SEND_FREQ_OFFSET:
		flash_value[0] = (uint8_t)(g_lorawan_settings.send_repeat_time >> 0);
		flash_value[1] = (uint8_t)(g_lorawan_settings.send_repeat_time >> 8);
		flash_value[2] = (uint8_t)(g_lorawan_settings.send_repeat_time >> 16);
		flash_value[3] = (uint8_t)(g_lorawan_settings.send_repeat_time >> 24);
		flash_value[4] = 0xAA;
		// MYLOG("AT_CMD", "Writing send frequency 0X%02X 0X%02X 0X%02X 0X%02X ",
		// 	  flash_value[0], flash_value[1],
		// 	  flash_value[2], flash_value[3]);
		wr_result = api.system.flash.set(SEND_FREQ_OFFSET, flash_value, 5);
		// MYLOG("AT_CMD", "Writing %s", wr_result ? "Success" : "Fail");
		return wr_result;
		break;
	default:
		return false;
		break;
	}
	return false;
}
