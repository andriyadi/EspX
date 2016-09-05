//
// Created by Andri Yadi on 8/1/16.
// Adapted from: https://github.com/skywodd/pcf8574_arduino_library
//

#ifndef XBOARD_BASE_GPIOEX_H
#define XBOARD_BASE_GPIOEX_H

#include "Arduino.h"
#include <Wire.h>

#define GPIOEX_OK          0x00
#define GPIOEX_PIN_ERROR   0x81
#define GPIOEX_I2C_ERROR   0x82

//Registers
#define MAX7315_REG_INPUTS			0x00
#define MAX7315_REG_BLINK0			0x01
#define MAX7315_REG_BLINK1			0x09
#define MAX7315_REG_PORT_CONFIG		0x03
#define MAX7315_REG_MASTER			0x0E
#define MAX7315_REG_CONFIG			0x0F
#define MAX7315_REG_INT10			0x10
#define MAX7315_REG_INT32			0x11
#define MAX7315_REG_INT54			0x12
#define MAX7315_REG_INT76			0x13

#define ESPECTRO_BASE_GPIOEX_BUTTON_PIN     1
#define ESPECTRO_BASE_GPIOEX_LED_PIN        2

class ESPectroBase_GpioEx {
public:
    ESPectroBase_GpioEx(uint8_t address = 0x20);
    ~ESPectroBase_GpioEx();

    /**
    * Start the I2C controller and store the PCF8574 chip address
    */
    boolean begin();

    int lastError();

    /**
     * Set the direction of a pin (OUTPUT, INPUT or INPUT_PULLUP)
     *
     * @param pin The pin to set
     * @param mode The new mode of the pin
     * @remarks INPUT_PULLUP does physicaly the same thing as INPUT (no software pull-up resistors available) but is REQUIRED if you use external pull-up resistor
     */
    void pinMode(uint8_t pin, uint8_t mode);

    /**
     * Set the state of a pin (HIGH or LOW)
     *
     * @param pin The pin to set
     * @param value The new state of the pin
     * @remarks Software pull-up resistors are not available on the PCF8574
     */
    void digitalWrite(uint8_t pin, uint8_t value);

    /**
     * Read the state of a pin
     *
     * @param pin The pin to read back
     * @return
     */
    uint8_t digitalRead(uint8_t pin);

    /**
     * Set the state of all pins in one go
     *
     * @param value The new value of all pins (1 bit = 1 pin, '1' = HIGH, '0' = LOW)
     */
    void write(uint8_t value);

    /**
     * Read the state of all pins in one go
     *
     * @return The current value of all pins (1 bit = 1 pin, '1' = HIGH, '0' = LOW)
     */
    uint8_t read();

    /**
     * Exactly like write(0x00), set all pins to LOW
     */
    void clear();

    /**
	 * Exactly like write(0xFF), set all pins to HIGH
	 */
    void set();

    /**
	 * Toggle the state of a pin
	 */
    void toggle(uint8_t pin);

    /**
	 * Make a pin blink N times for T duration
	 *
	 * @warning Blocking function, not recommended for new code
	 * @deprecated
	 * @param pin The pin to blink
	 * @param count The number of ON/OFF couples to execute
	 * @param duration The duration of the whole blink action in milliseconds
	 */
    void blink(uint8_t pin, uint16_t count, uint32_t duration);

protected:

    uint8_t i2cAddress_ = 0x20;
    int lastError_ = 0;

    /** Output pins values */
    volatile uint8_t PORT_;

    /** Current input pins values */
    volatile uint8_t PIN_;

    /** Pins modes values (OUTPUT or INPUT) */
    volatile uint8_t DDR_;

    /**
	 * Read GPIO states and store them in PIN_ variable
	 *
	 * @remarks Before reading current GPIO states, current PIN_ variable value is moved to _oldPIN variable
	 */
    void readGPIO();

    /**
     * Write value of PORT_ variable to the GPIO
     *
     * @remarks Only pin marked as OUTPUT are set, for INPUT pins their value are unchanged
     * @warning To work properly (and avoid any states conflicts) readGPIO() MUST be called before call this function !
     */
    void updateGPIO();

    uint8_t writeReg(uint8_t reg, unsigned char *values, char length);
    uint8_t writeReg(uint8_t reg, uint8_t value);
    uint8_t readReg(uint8_t reg, uint8_t *value, uint8_t length);
};


#endif //XBOARD_BASE_GPIOEX_H
