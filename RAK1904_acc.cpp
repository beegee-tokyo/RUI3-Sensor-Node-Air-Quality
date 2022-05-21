// /**
//  * @file RAK1904_acc.cpp
//  * @author Bernd Giesecke (bernd@giesecke.tk)
//  * @brief Initialize and read values from the LIS3DH sensor
//  * @version 0.1
//  * @date 2022-04-11
//  *
//  * @copyright Copyright (c) 2022
//  *
//  */
// #include "main.h"
// #include <SparkFunLIS3DH.h>

// // Forward declarations
// void int_callback_rak1904(void);

// /** Sensor instance using Wire */
// LIS3DH myIMU(I2C_MODE, 0x18);

// /** Interrupt pin, depends on slot */
// uint8_t acc_int_pin = WB_IO5;

// /**
//  * @brief Initialize LIS3DH 3-axis
//  * acceleration sensor
//  *
//  * @return true If sensor was found and is initialized
//  * @return false If sensor initialization failed
//  */
// bool init_rak1904(void)
// {
// 	// Setup interrupt pin
// 	pinMode(acc_int_pin, INPUT);

// 	Wire.begin();

// 	myIMU.settings.accelSampleRate = 10;
// 	myIMU.settings.accelRange = 4;

// 	myIMU.settings.xAccelEnabled = 1;
// 	myIMU.settings.yAccelEnabled = 1;
// 	myIMU.settings.zAccelEnabled = 1;

// 	myIMU.begin();

// 	// Enable interrupts
// 	uint8_t data_to_write = 0;
// 	data_to_write |= 0x20;									  // Z high
// 	data_to_write |= 0x08;									  // Y high
// 	data_to_write |= 0x02;									  // X high
// 	myIMU.writeRegister(LIS3DH_INT1_CFG, data_to_write); // Enable interrupts on high tresholds for x, y and z

// 	// Set interrupt trigger range
// 	data_to_write = 0;
// 	data_to_write |= 0x10;									  // 1/8 range
// 	myIMU.writeRegister(LIS3DH_INT1_THS, data_to_write); // 1/8th range

// 	// Set interrupt signal length
// 	data_to_write = 0;
// 	data_to_write |= 0x01; // 1 * 1/50 s = 20ms
// 	myIMU.writeRegister(LIS3DH_INT1_DURATION, data_to_write);

// 	myIMU.readRegister(&data_to_write, LIS3DH_CTRL_REG5);
// 	data_to_write &= 0xF3;									// Clear bits of interest
// 	data_to_write |= 0x08;									// Latch interrupt (Cleared by reading int1_src)
// 	myIMU.writeRegister(LIS3DH_CTRL_REG5, data_to_write); // Set interrupt to latching

// 	// Select interrupt pin 1
// 	data_to_write = 0;
// 	data_to_write |= 0x40; // AOI1 event (Generator 1 interrupt on pin 1)
// 	data_to_write |= 0x20; // AOI2 event ()
// 	myIMU.writeRegister(LIS3DH_CTRL_REG3, data_to_write);

// 	// No interrupt on pin 2
// 	myIMU.writeRegister(LIS3DH_CTRL_REG6, 0x00);

// 	// Enable high pass filter
// 	myIMU.writeRegister(LIS3DH_CTRL_REG2, 0x01);

// 	// Set low power mode
// 	data_to_write = 0;
// 	myIMU.readRegister(&data_to_write, LIS3DH_CTRL_REG1);
// 	data_to_write |= 0x08;
// 	myIMU.writeRegister(LIS3DH_CTRL_REG1, data_to_write);
// 	delay(100);
// 	data_to_write = 0;
// 	myIMU.readRegister(&data_to_write, 0x1E);
// 	data_to_write |= 0x90;
// 	myIMU.writeRegister(0x1E, data_to_write);
// 	delay(100);

// 	clear_int_rak1904();

// 	// Set the interrupt callback function
// 	attachInterrupt(acc_int_pin, int_callback_rak1904, RISING);

// 	return true;
// }

// /**
//  * @brief Reads the values from the LIS3DH sensor
//  *
//  */
// void read_rak1904(void)
// {
// 	MYLOG("ACC", "Acceleration in g (x,y,z): %f %f %f", myIMU.readFloatAccelX(), myIMU.readFloatAccelY(), myIMU.readFloatAccelZ());
// }

// /**
//  * @brief Assign/reassing interrupt pin
//  *
//  * @param new_irq_pin new GPIO to assign to interrupt
//  */
// void int_assign_rak1904(uint8_t new_irq_pin)
// {
// 	detachInterrupt(acc_int_pin);
// 	acc_int_pin = new_irq_pin;
// 	attachInterrupt(acc_int_pin, int_callback_rak1904, RISING);
// }

// /**
//  * @brief ACC interrupt handler
//  * @note gives semaphore to wake up main loop
//  *
//  */
// void int_callback_rak1904(void)
// {
// 	if ((millis() - last_trigger) > 15000)
// 	{
// 		MYLOG("ACC", "Interrupt triggered");
// 		last_trigger = millis();
// 		// Read the sensors and trigger a packet
// 		sensor_handler(NULL);
// 		// Stop a unified C timer in C language. This API is defined in udrv_timer.h. It will be replaced by api.system.timer.stop() after story #1195 is done.
// 		udrv_timer_stop(TIMER_0);
// 		// Start a unified C timer in C language. This API is defined in udrv_timer.h. It will be replaced by api.system.timer.start() after story #1195 is done.
// 		udrv_timer_start(TIMER_0, 30000, NULL);
// 	}
// 	motion_detected = true;
// 	clear_int_rak1904();
// }

// /**
//  * @brief Clear ACC interrupt register to enable next wakeup
//  *
//  */
// void clear_int_rak1904(void)
// {
// 	uint8_t dataRead;
// 	myIMU.readRegister(&dataRead, LIS3DH_INT1_SRC); // cleared by reading
// }