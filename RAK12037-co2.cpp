/**
 * @file RAK12037-co2.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Initialize and read values from the SCD30 sensor
 * @version 0.1
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "main.h"
#include <SparkFun_SCD30_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_SCD30

/** Sensor instance */
SCD30 scd30;

/**
 * @brief Initialize MQ2 gas sensor
 *
 * @return true success
 * @return false failed
 */
bool init_rak12037(void)
{
	// Enable power
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH); // power on RAK12037

		Wire.begin();
		if (!scd30.begin(Wire))
		{
			// MYLOG("SCD30", "SCD30 not found");
			digitalWrite(WB_IO2, LOW); // power down RAK12004
			return false;
		}

	//**************init SCD30 sensor *****************************************************
	// Change number of seconds between measurements: 2 to 1800 (30 minutes), stored in non-volatile memory of SCD30
	scd30.setMeasurementInterval(10000);

	// Enable self calibration
	scd30.setAutoSelfCalibration(true);

	// Start the measurements
	scd30.beginMeasuring();

	return true;
}

/**
 * @brief Read CO2 sensor data
 *     Data is added to Cayenne LPP payload as channels
 *     LPP_CHANNEL_CO2_2, LPP_CHANNEL_CO2_Temp_2 and LPP_CHANNEL_CO2_HUMID_2
 *
 */
void read_rak12037(void)
{
	time_t start_time = millis();
	while (!scd30.dataAvailable())
	{
		// MYLOG("SCD30", "Waiting for data");
		delay(500);
		if ((millis() - start_time) > 5000)
		{
			// timeout, no data available
			// MYLOG("SCD30", "Timeout");
			return;
		}
	}

	// uint16_t co2_reading = scd30.getCO2();
	// float temp_reading = scd30.getTemperature();
	// float humid_reading = scd30.getHumidity();

	// MYLOG("SCD30", "CO2 level %dppm", co2_reading);
	// MYLOG("SCD30", "Temperature %.2f", temp_reading);
	// MYLOG("SCD30", "Humidity %.2f", humid_reading);

	g_solution_data.addConcentration(LPP_CHANNEL_CO2_2, scd30.getCO2());
	g_solution_data.addTemperature(LPP_CHANNEL_CO2_Temp_2, scd30.getTemperature());
	g_solution_data.addRelativeHumidity(LPP_CHANNEL_CO2_HUMID_2, scd30.getHumidity());
}