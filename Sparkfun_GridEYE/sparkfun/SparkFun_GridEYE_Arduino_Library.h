/*
  This is a library written for the Panasonic Grid-EYE AMG88
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14568

  Written by Nick Poole @ SparkFun Electronics, January 11th, 2018

  The GridEYE from Panasonic is an 8 by 8 thermopile array capable
  of detecting temperature remotely at 64 discrete points.

  This library handles communication with the GridEYE and provides
  methods for manipulating temperature registers in Celsius,
  Fahrenheit and raw values.

  https://github.com/sparkfun/SparkFun_GridEYE_Arduino_Library

  Development environment specifics:
  Arduino IDE 1.8.3

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

// The default I2C address for the THING on the SparkX breakout is 0x69. 0x68 is also possible.
#define DEFAULT_ADDRESS 0x69

// Platform specific configurations

// Define the size of the I2C buffer based on the platform the user has

// The catch-all default is 32
#define I2C_BUFFER_LENGTH 32


// Registers
#define POWER_CONTROL_REGISTER 0x00
#define RESET_REGISTER 0x01
#define FRAMERATE_REGISTER 0x02
#define INT_CONTROL_REGISTER 0x03
#define STATUS_REGISTER 0x04
#define STATUS_CLEAR_REGISTER 0x05
#define AVERAGE_REGISTER 0x07
#define INT_LEVEL_REGISTER_UPPER_LSB 0x08
#define INT_LEVEL_REGISTER_UPPER_MSB 0x09
#define INT_LEVEL_REGISTER_LOWER_LSB 0x0A
#define INT_LEVEL_REGISTER_LOWER_MSB 0x0B
#define INT_LEVEL_REGISTER_HYST_LSB 0x0C
#define INT_LEVEL_REGISTER_HYST_MSB 0x0D
#define THERMISTOR_REGISTER_LSB 0x0E
#define THERMISTOR_REGISTER_MSB 0x0F
#define INT_TABLE_REGISTER_INT0 0x10
#define RESERVED_AVERAGE_REGISTER 0x1F
#define TEMPERATURE_REGISTER_START 0x80

void GridEYE_begin();

float GridEYE_getPixelTemperature(unsigned char pixelAddr);
int16_t GridEYE_getPixelTemperatureRaw(unsigned char pixelAddr); // The return value is somewhat ambiguous. Use getPixelTemperatureSigned for a better experience...
int16_t GridEYE_getPixelTemperatureSigned(unsigned char pixelAddr);
float GridEYE_getPixelTemperatureFahrenheit(unsigned char pixelAddr);

float GridEYE_getDeviceTemperature();
int16_t GridEYE_getDeviceTemperatureRaw(); // The return value is somewhat ambiguous. Use getDeviceTemperatureSigned for a better experience...
int16_t GridEYE_getDeviceTemperatureSigned();
float GridEYE_getDeviceTemperatureFahrenheit();

void GridEYE_setFramerate1FPS();
void GridEYE_setFramerate10FPS();
bool GridEYE_isFramerate10FPS();
bool GridEYE_getFramerate(bool *is10FPS);

void GridEYE_wake();
void GridEYE_sleep();
void GridEYE_standby60seconds();
void GridEYE_standby10seconds();

void GridEYE_interruptPinEnable();
void GridEYE_interruptPinDisable();
void GridEYE_setInterruptModeAbsolute();
void GridEYE_setInterruptModeDifference();
bool GridEYE_interruptPinEnabled();

bool GridEYE_interruptFlagSet();
bool GridEYE_pixelTemperatureOutputOK();
bool GridEYE_deviceTemperatureOutputOK();
void GridEYE_clearInterruptFlag();
void GridEYE_clearPixelTemperatureOverflow();
void GridEYE_clearDeviceTemperatureOverflow();
void GridEYE_clearAllOverflow();
void GridEYE_clearAllStatusFlags();

bool GridEYE_pixelInterruptSet(uint8_t pixelAddr);

void GridEYE_movingAverageEnable();
void GridEYE_movingAverageDisable();
bool GridEYE_movingAverageEnabled();

void GridEYE_setUpperInterruptValue(float DegreesC);
void GridEYE_setUpperInterruptValueRaw(int16_t regValue);
void GridEYE_setUpperInterruptValueFahrenheit(float DegreesF);

void GridEYE_setLowerInterruptValue(float DegreesC);
void GridEYE_setLowerInterruptValueRaw(int16_t regValue);
void GridEYE_setLowerInterruptValueFahrenheit(float DegreesF);

void GridEYE_setInterruptHysteresis(float DegreesC);
void GridEYE_setInterruptHysteresisRaw(int16_t regValue);
void GridEYE_setInterruptHysteresisFahrenheit(float DegreesF);

float GridEYE_getUpperInterruptValue();
int16_t GridEYE_getUpperInterruptValueRaw(); // The return value is somewhat ambiguous. Use getUpperInterruptValueSigned for a better experience...
int16_t GridEYE_getUpperInterruptValueSigned();
float GridEYE_getUpperInterruptValueFahrenheit();

float GridEYE_getLowerInterruptValue();
int16_t GridEYE_getLowerInterruptValueRaw(); // The return value is somewhat ambiguous. Use getLowerInterruptValueSigned for a better experience...
int16_t GridEYE_getLowerInterruptValueSigned();
float GridEYE_getLowerInterruptValueFahrenheit();

float GridEYE_getInterruptHysteresis();
int16_t GridEYE_getInterruptHysteresisRaw(); // The return value is somewhat ambiguous. Use getInterruptHysteresisSigned for a better experience...
int16_t GridEYE_getInterruptHysteresisSigned();
float GridEYE_getInterruptHysteresisFahrenheit();

bool GridEYE_setRegister(unsigned char reg, unsigned char val);
int16_t GridEYE_getRegister(unsigned char reg, int8_t len); // Provided for backward compatibility only. Not recommended...
bool GridEYE_getRegister8(unsigned char reg, uint8_t *val);
bool GridEYE_getRegister16(unsigned char reg, uint16_t *val); // Note: this returns an unsigned val. Use convertUnsignedSigned to convert to int16_t
int16_t GridEYE_convertUnsignedSigned16(uint16_t val);
uint16_t GridEYE_convertSignedUnsigned16(int16_t val);
float GridEYE_convertSigned12ToFloat(uint16_t val);
uint16_t GridEYE_convertFloatToSigned12(float val);
