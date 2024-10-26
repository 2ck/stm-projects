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

#include "SparkFun_GridEYE_Arduino_Library.h"
#include "i2c_stub.h"

void GridEYE_begin()
{
  i2c_begin();
}

/********************************************************
 * Functions for retreiving the temperature of
 * a single pixel.
 ********************************************************
 *
 * getPixelTemperature() - returns float Celsius
 *
 * getPixelTemperatureFahrenheit() - returns float Fahrenheit
 *
 * getPixelTemperatureRaw() - returns int16_t contents of
 *    both pixel temperature registers concatinated
 *
 ********************************************************/

float GridEYE_getPixelTemperature(unsigned char pixelAddr)
{
  // Temperature registers are numbered 128-255
  // Each pixel has a lower and higher register
  unsigned char pixelLowRegister = TEMPERATURE_REGISTER_START + (2 * pixelAddr);
  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(pixelLowRegister, &temperature))
    return -99.0; // Indicate a read error

  return (GridEYE_convertSigned12ToFloat(temperature) * 0.25); // GridEYE_convert to Degrees C. LSB resolution is 0.25C.
}

float GridEYE_getPixelTemperatureFahrenheit(unsigned char pixelAddr)
{
  float DegreesC = GridEYE_getPixelTemperature(pixelAddr);
  if (DegreesC == -99.0)
    return DegreesC;
  return (DegreesC * 1.8 + 32); // GridEYE_convert to Fahrenheit
}

int16_t GridEYE_getPixelTemperatureRaw(unsigned char pixelAddr)
{
  // Temperature registers are numbered 128-255
  // Each pixel has a lower and higher register
  unsigned char pixelLowRegister = TEMPERATURE_REGISTER_START + (2 * pixelAddr);
  uint16_t temperature = 0;
  GridEYE_getRegister16(pixelLowRegister, &temperature);

  return GridEYE_convertUnsignedSigned16(temperature); // Somewhat ambiguous...
}

int16_t GridEYE_getPixelTemperatureSigned(unsigned char pixelAddr)
{
  // Temperature registers are numbered 128-255
  // Each pixel has a lower and higher register
  unsigned char pixelLowRegister = TEMPERATURE_REGISTER_START + (2 * pixelAddr);
  uint16_t temperature = 0;
  if (!GridEYE_getRegister16(pixelLowRegister, &temperature))
    return -99; // Indicate a read error

  // temperature is 12-bit twos complement
  // check if temperature is negative
  if (temperature & (1 << 11))
    temperature |= 0xF000; // Set the other MS bits to 1 to preserve the two's complement
  else
    temperature &= 0x07FF; // Clear the unused bits - just in case

  return GridEYE_convertUnsignedSigned16(temperature); // GridEYE_convert to int16_t without ambiguity
}

/********************************************************
 * Functions for retreiving the temperature of
 * the device according to the embedded thermistor.
 ********************************************************
 *
 * getDeviceTemperature() - returns float Celsius
 *
 * getDeviceTemperatureFahrenheit() - returns float Fahrenheit
 *
 * getDeviceTemperatureRaw() - returns int16_t contents of
 *    both thermistor temperature registers concatinated
 *
 ********************************************************/

float GridEYE_getDeviceTemperature()
{

  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(THERMISTOR_REGISTER_LSB, &temperature))
    return -99.0; // Indicate a read error

  return (GridEYE_convertSigned12ToFloat(temperature) * 0.0625);
}

float GridEYE_getDeviceTemperatureFahrenheit()
{
  float DegreesC = GridEYE_getDeviceTemperature();
  if (DegreesC == -99.0)
    return DegreesC;
  return (DegreesC * 1.8 + 32);
}

int16_t GridEYE_getDeviceTemperatureRaw()
{
  uint16_t temperature = 0;
  
  GridEYE_getRegister16(THERMISTOR_REGISTER_LSB, &temperature);

  return GridEYE_convertUnsignedSigned16(temperature); // Somewhat ambiguous...
}

int16_t GridEYE_getDeviceTemperatureSigned()
{
  uint16_t temperature = 0;
  if (!GridEYE_getRegister16(THERMISTOR_REGISTER_LSB, &temperature))
    return -99; // Indicate a read error

  // temperature is 12-bit twos complement
  // check if temperature is negative
  if (temperature & (1 << 11))
    temperature |= 0xF000; // Set the other MS bits to 1 to preserve the two's complement
  else
    temperature &= 0x07FF; // Clear the unused bits - just in case

  return GridEYE_convertUnsignedSigned16(temperature); // GridEYE_convert to int16_t without ambiguity
}

/********************************************************
 * Functions for manipulating Framerate
 ********************************************************
 *
 * Internal framerate of the device is always 10fps
 * When operating in 1FPS mode, each frame is an average
 * of 10 readings.
 *
 * setFramerate1FPS() - sets framerate to 1 Frame per Second
 *
 * setFramerate10FPS() - sets framerate to 10 Frames per Second
 *
 * isFramerate10FPS() - returns true if framerate is currently
 *    set to 10 Frames per Second (device default)
 *
 ********************************************************/

void GridEYE_setFramerate1FPS()
{
  GridEYE_setRegister(FRAMERATE_REGISTER, 1);
}

void GridEYE_setFramerate10FPS()
{
  GridEYE_setRegister(FRAMERATE_REGISTER, 0);
}

bool GridEYE_getFramerate(bool *is10FPS)
{
  uint8_t val = 0;

  bool result = GridEYE_getRegister8(FRAMERATE_REGISTER, &val);

  if (result)
    *is10FPS = val == 0; // If val is zero, frame rate it 10FPS

  return result;
}

bool GridEYE_isFramerate10FPS()
{
  bool is10FPS = false;

  GridEYE_getFramerate(&is10FPS);

  return is10FPS;
}

/********************************************************
 * Functions for manipulating Operating Mode
 ********************************************************
 *
 * Device defaults to normal mode on reset.
 * When the device is in standby mode, the temperature
 * register is only updated intermittently.
 *
 * wake() - returns device to normal mode from any
 *    other state.
 *
 * sleep() - puts device into sleep mode, temperature
 *    register is not updated
 *
 * standby60seconds() - puts device into standby mode
 *    with 60 second update frequency
 *
 * standby10seconds() - puts device into standby mode
 *    with 10 second update frequency
 *
 ********************************************************/

void GridEYE_wake()
{

  GridEYE_setRegister(POWER_CONTROL_REGISTER, 0x00);
}

void GridEYE_sleep()
{

  GridEYE_setRegister(POWER_CONTROL_REGISTER, 0x10);
}

void GridEYE_standby60seconds()
{

  GridEYE_setRegister(POWER_CONTROL_REGISTER, 0x20);
}

void GridEYE_standby10seconds()
{

  GridEYE_setRegister(POWER_CONTROL_REGISTER, 0x21);
}

/********************************************************
 * Functions for manipulating Interrupt Control Register
 ********************************************************
 *
 * interruptPinEnable() - Enable INT pin to pull low on
 *    interrupt flag
 *
 * interruptPinDisable() - Put INT pin into Hi-Z state
 *
 * setInterruptModeAbsolute() - Set interrupt mode to
 *    "Absolute Value" mode
 *
 * setInterruptModeDifference() - Set interrupt mode to
 *    "Difference" mode
 *
 * interruptPinEnabled() - returns true if the INT pin
 *    is enabled. Returns false if INT pin is in Hi-Z
 *
 ********************************************************/

void GridEYE_interruptPinEnable()
{
  uint8_t ICRValue = 0;
  
  if (GridEYE_getRegister8(INT_CONTROL_REGISTER, &ICRValue))
  {
    ICRValue |= (1 << 0);

    GridEYE_setRegister(INT_CONTROL_REGISTER, ICRValue);
  }
}

void GridEYE_interruptPinDisable()
{
  uint8_t ICRValue = 0;
  
  if (GridEYE_getRegister8(INT_CONTROL_REGISTER, &ICRValue))
  {
    ICRValue &= ~(1 << 0);

    GridEYE_setRegister(INT_CONTROL_REGISTER, ICRValue);
  }
}

void GridEYE_setInterruptModeAbsolute()
{
  uint8_t ICRValue = 0;
  
  if (GridEYE_getRegister8(INT_CONTROL_REGISTER, &ICRValue))
  {
    ICRValue |= (1 << 1);

    GridEYE_setRegister(INT_CONTROL_REGISTER, ICRValue);
  }
}

void GridEYE_setInterruptModeDifference()
{
  uint8_t ICRValue = 0;
  
  if (GridEYE_getRegister8(INT_CONTROL_REGISTER, &ICRValue))
  {
    ICRValue &= ~(1 << 1);

    GridEYE_setRegister(INT_CONTROL_REGISTER, ICRValue);
  }
}

bool GridEYE_interruptPinEnabled()
{
  uint8_t ICRValue = 0;
  
  if (!GridEYE_getRegister8(INT_CONTROL_REGISTER, &ICRValue))
    return false; // Somewhat ambiguous...
  
  return (ICRValue & (1 << 0));
}

/********************************************************
 * Functions for manipulating Status/Clear Registers
 ********************************************************
 *
 * interruptFlagSet() - returns true if there is an
 *    interrupt flag in the status register
 *
 * pixelTemperatureOutputOK() - returns false if temperature
 *    output overflow flag is present in status register
 *
 * deviceTemperatureOutputOK() - returns false if thermistor
 *    output overflow flag is present in status register
 *
 * clearInterruptFlag() - clears interrupt flag in the
 *    status register
 *
 * clearPixelTemperatureOverflow() - clears temperature
 *    output overflow flag in status register
 *
 * clearDeviceTemperatureOverflow() - clears thermistor
 *    output overflow flag in status register
 *
 * clearAllOverflow() - clears both thermistor and
 *    temperature overflow flags in status register but
 *    leaves interrupt flag untouched
 *
 * clearAllStatusFlags() - clears all flags in status
 *    register
 *
 ********************************************************/

bool GridEYE_interruptFlagSet()
{
  uint8_t StatRegValue = 0;
  
  if (!GridEYE_getRegister8(STATUS_REGISTER, &StatRegValue))
    return false; // Somewhat ambiguous...
  
  return (StatRegValue & (1 << 1));
}

bool GridEYE_pixelTemperatureOutputOK()
{
  uint8_t StatRegValue = 0;
  
  if (!GridEYE_getRegister8(STATUS_REGISTER, &StatRegValue))
    return false; // Somewhat ambiguous...
  
  return (StatRegValue & (1 << 2));
}

bool GridEYE_deviceTemperatureOutputOK()
{
  uint8_t StatRegValue = 0;
  
  if (!GridEYE_getRegister8(STATUS_REGISTER, &StatRegValue))
    return false; // Somewhat ambiguous...
  
  return (StatRegValue & (1 << 3));
}

void GridEYE_clearInterruptFlag()
{

  GridEYE_setRegister(STATUS_CLEAR_REGISTER, 0x02);
}

void GridEYE_clearPixelTemperatureOverflow()
{

  GridEYE_setRegister(STATUS_CLEAR_REGISTER, 0x04);
}

void GridEYE_clearDeviceTemperatureOverflow()
{

  GridEYE_setRegister(STATUS_CLEAR_REGISTER, 0x08);
}

void GridEYE_clearAllOverflow()
{

  GridEYE_setRegister(STATUS_CLEAR_REGISTER, 0x0C);
}

void GridEYE_clearAllStatusFlags()
{

  GridEYE_setRegister(STATUS_CLEAR_REGISTER, 0x0E);
}

/********************************************************
 * Function for reading Interrupt Table Register
 ********************************************************
 *
 * pixelInterruptSet() - Returns true if interrupt flag
 * is set for the specified pixel
 *
 ********************************************************/

bool GridEYE_pixelInterruptSet(uint8_t pixelAddr)
{
  unsigned char interruptTableRegister = INT_TABLE_REGISTER_INT0 + (pixelAddr / 8);
  uint8_t pixelPosition = (pixelAddr % 8);

  uint8_t interruptTableRow = 0;
  
  if (!GridEYE_getRegister8(interruptTableRegister, &interruptTableRow))
    return false; // Somewhat ambiguous...

  return (interruptTableRow & (1 << pixelPosition));
}

/********************************************************
 * Functions for manipulating Average Register
 ********************************************************
 *
 * Moving Average Mode enable and disable are only
 * referenced in some of the documentation for this
 * device but not in all documentation. Requires writing
 * in sequence to a reserved register. I'm not sure it
 * does anything.
 *
 * movingAverageEnable() - enable "Twice Moving Average"
 *
 * movingAverageDisable() - disable "Twice Moving Average"
 *
 * movingAverageEnabled() - returns true if enabled
 *
 ********************************************************/

void GridEYE_movingAverageEnable()
{

  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x50);
  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x45);
  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x57);
  GridEYE_setRegister(AVERAGE_REGISTER, 0x20);
  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x00);
}

void GridEYE_movingAverageDisable()
{

  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x50);
  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x45);
  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x57);
  GridEYE_setRegister(AVERAGE_REGISTER, 0x00);
  GridEYE_setRegister(RESERVED_AVERAGE_REGISTER, 0x00);
}

bool GridEYE_movingAverageEnabled()
{
  uint8_t AVGRegValue = 0;
  
  if (!GridEYE_getRegister8(AVERAGE_REGISTER, &AVGRegValue))
    return false; // Somewhat ambiguous...
  
  return (AVGRegValue & (1 << 5));
}

/********************************************************
 * Functions for manipulating Interrupt Level Register
 ********************************************************
 *
 * setUpperInterruptValue() - accepts float Celsius
 *
 * setUpperInterruptValueRaw() - accepts int16_t register
 *    configuration
 *
 * setUpperInterruptValueFahrenheit() - accepts float
 *    Fahrenheit
 *
 * setLowerInterruptValue() - accepts float Celsius
 *
 * setLowerInterruptValueRaw() - accepts int16_t register
 *    configuration
 *
 * setLowerInterruptValueFahrenheit() - accepts float
 *    Fahrenheit
 *
 * setInterruptHysteresis() - accepts float Celsius
 *
 * setInterruptHysteresisRaw() - accepts int16_t register
 *    configuration
 *
 * setInterruptHysteresisFahrenheit() - accepts float
 *    Fahrenheit
 *
 * getUpperInterruptValue() - returns float Celsius
 *
 * getUpperInterruptValueRaw() - returns int16_t register
 *    contents
 *
 * getUpperInterruptValueFahrenheit() - returns float
 *    Fahrenheit
 *
 * getLowerInterruptValue() - returns float Celsius
 *
 * getLowerInterruptValueRaw() - returns int16_t register
 *    contents
 *
 * getLowerInterruptValueFahrenheit() - returns float
 *    Fahrenheit
 *
 * getInterruptHysteresis() - returns float Celsius
 *
 * getInterruptHysteresisRaw() - returns int16_t register
 *    contents
 *
 * getInterruptHysteresisFahrenheit() - returns float
 *    Fahrenheit
 *
 ********************************************************/

void GridEYE_setUpperInterruptValue(float DegreesC)
{
  uint16_t temperature12 = GridEYE_convertFloatToSigned12(DegreesC * 4); // GridEYE_convert to 12-bit signed with 0.25C LSB resolution

  GridEYE_setRegister(INT_LEVEL_REGISTER_UPPER_LSB, temperature12 & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_UPPER_MSB, temperature12 >> 8);
}

void GridEYE_setUpperInterruptValueRaw(int16_t regValue)
{
  GridEYE_setRegister(INT_LEVEL_REGISTER_UPPER_LSB, regValue & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_UPPER_MSB, regValue >> 8);
}

void GridEYE_setUpperInterruptValueFahrenheit(float DegreesF)
{
  uint16_t temperature12 = GridEYE_convertFloatToSigned12((DegreesF - 32) * 4 / 1.8); // GridEYE_convert to 12-bit signed

  GridEYE_setRegister(INT_LEVEL_REGISTER_UPPER_LSB, temperature12 & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_UPPER_MSB, temperature12 >> 8);
}

void GridEYE_setLowerInterruptValue(float DegreesC)
{
  uint16_t temperature12 = GridEYE_convertFloatToSigned12(DegreesC * 4); // GridEYE_convert to 12-bit signed with 0.25C LSB resolution

  GridEYE_setRegister(INT_LEVEL_REGISTER_LOWER_LSB, temperature12 & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_LOWER_MSB, temperature12 >> 8);
}

void GridEYE_setLowerInterruptValueRaw(int16_t regValue)
{
  GridEYE_setRegister(INT_LEVEL_REGISTER_LOWER_LSB, regValue & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_LOWER_MSB, regValue >> 8);
}

void GridEYE_setLowerInterruptValueFahrenheit(float DegreesF)
{
  uint16_t temperature12 = GridEYE_convertFloatToSigned12((DegreesF - 32) * 4 / 1.8); // GridEYE_convert to 12-bit signed

  GridEYE_setRegister(INT_LEVEL_REGISTER_LOWER_LSB, temperature12 & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_LOWER_MSB, temperature12 >> 8);
}

void GridEYE_setInterruptHysteresis(float DegreesC)
{
  uint16_t temperature12 = GridEYE_convertFloatToSigned12(DegreesC * 4); // GridEYE_convert to 12-bit signed with 0.25C LSB resolution

  GridEYE_setRegister(INT_LEVEL_REGISTER_HYST_LSB, temperature12 & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_HYST_MSB, temperature12 >> 8);
}

void GridEYE_setInterruptHysteresisRaw(int16_t regValue)
{
  GridEYE_setRegister(INT_LEVEL_REGISTER_HYST_LSB, regValue & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_HYST_MSB, regValue >> 8);
}

void GridEYE_setInterruptHysteresisFahrenheit(float DegreesF)
{
  uint16_t temperature12 = GridEYE_convertFloatToSigned12((DegreesF - 32) * 4 / 1.8); // GridEYE_convert to 12-bit signed

  GridEYE_setRegister(INT_LEVEL_REGISTER_HYST_LSB, temperature12 & 0xFF);
  GridEYE_setRegister(INT_LEVEL_REGISTER_HYST_MSB, temperature12 >> 8);
}

float GridEYE_getUpperInterruptValue()
{
  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_UPPER_LSB, &temperature))
    return -99.0; // Indicate a read error

  return ((GridEYE_convertSigned12ToFloat(temperature)) * 0.25); // GridEYE_convert to Degrees C. LSB resolution is 0.25C.
}

float GridEYE_getUpperInterruptValueFahrenheit()
{
  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_UPPER_LSB, &temperature))
    return -99.0; // Indicate a read error

  return (((GridEYE_convertSigned12ToFloat(temperature)) * 0.25 * 1.8) + 32); // GridEYE_convert to Degrees F
}

int16_t GridEYE_getUpperInterruptValueRaw()
{
  uint16_t val = 0;
  GridEYE_getRegister16(INT_LEVEL_REGISTER_UPPER_LSB, &val);
  return GridEYE_convertUnsignedSigned16(val);
}

int16_t GridEYE_getUpperInterruptValueSigned()
{
  uint16_t temperature = 0;
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_UPPER_LSB, &temperature))
    return -99; // Indicate a read error

  // temperature is 12-bit twos complement
  // check if temperature is negative
  if (temperature & (1 << 11))
    temperature |= 0xF000; // Set the other MS bits to 1 to preserve the two's complement
  else
    temperature &= 0x07FF; // Clear the unused bits - just in case

  return GridEYE_convertUnsignedSigned16(temperature); // GridEYE_convert to int16_t without ambiguity
}

float GridEYE_getLowerInterruptValue()
{
  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_LOWER_LSB, &temperature))
    return -99.0; // Indicate a read error

  return ((GridEYE_convertSigned12ToFloat(temperature)) * 0.25); // GridEYE_convert to Degrees C. LSB resolution is 0.25C.
}

float GridEYE_getLowerInterruptValueFahrenheit()
{
  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_LOWER_LSB, &temperature))
    return -99.0; // Indicate a read error

  return (((GridEYE_convertSigned12ToFloat(temperature)) * 0.25 * 1.8) + 32); // GridEYE_convert to Degrees F
}

int16_t GridEYE_getLowerInterruptValueRaw()
{
  uint16_t val = 0;
  GridEYE_getRegister16(INT_LEVEL_REGISTER_LOWER_LSB, &val);
  return GridEYE_convertUnsignedSigned16(val);
}

int16_t GridEYE_getLowerInterruptValueSigned()
{
  uint16_t temperature = 0;
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_LOWER_LSB, &temperature))
    return -99; // Indicate a read error

  // temperature is 12-bit twos complement
  // check if temperature is negative
  if (temperature & (1 << 11))
    temperature |= 0xF000; // Set the other MS bits to 1 to preserve the two's complement
  else
    temperature &= 0x07FF; // Clear the unused bits - just in case

  return GridEYE_convertUnsignedSigned16(temperature); // GridEYE_convert to int16_t without ambiguity
}

float GridEYE_getInterruptHysteresis()
{
  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_HYST_LSB, &temperature))
    return -99.0; // Indicate a read error

  return ((GridEYE_convertSigned12ToFloat(temperature)) * 0.25); // GridEYE_convert to Degrees C. LSB resolution is 0.25C.
}

float GridEYE_getInterruptHysteresisFahrenheit()
{
  uint16_t temperature = 0;
  
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_HYST_LSB, &temperature))
    return -99.0; // Indicate a read error

  return (((GridEYE_convertSigned12ToFloat(temperature)) * 0.25 * 1.8) + 32); // GridEYE_convert to Degrees F
}

int16_t GridEYE_getInterruptHysteresisRaw()
{
  uint16_t val = 0;
  GridEYE_getRegister16(INT_LEVEL_REGISTER_HYST_LSB, &val);
  return GridEYE_convertUnsignedSigned16(val);
}

int16_t GridEYE_getInterruptHysteresisSigned()
{
  uint16_t temperature = 0;
  if (!GridEYE_getRegister16(INT_LEVEL_REGISTER_HYST_LSB, &temperature))
    return -99; // Indicate a read error

  // temperature is 12-bit twos complement
  // check if temperature is negative
  if (temperature & (1 << 11))
    temperature |= 0xF000; // Set the other MS bits to 1 to preserve the two's complement
  else
    temperature &= 0x07FF; // Clear the unused bits - just in case

  return GridEYE_convertUnsignedSigned16(temperature); // GridEYE_convert to int16_t without ambiguity
}

/********************************************************
 * Functions for setting and getting registers over I2C
 ********************************************************
 *
 * GridEYE_setRegister() - set unsigned char value at unsigned char register
 *
 * GridEYE_getRegister8() - get unsigned char value from unsigned char register
 *
 * GridEYE_getRegister16() - get uint16_t value from unsigned char register
 *
 * GridEYE_getRegister() - get up to INT16 value from unsigned char register
 *
 ********************************************************/

bool GridEYE_setRegister(unsigned char reg, unsigned char val)
{

  return i2c_write2(DEFAULT_ADDRESS, reg, val);
}

bool GridEYE_getRegister8(unsigned char reg, uint8_t *val)
{
  i2c_write(DEFAULT_ADDRESS, reg);
  *val = i2c_read(DEFAULT_ADDRESS);
  return true;
}

bool GridEYE_getRegister16(unsigned char reg, uint16_t *val)
{
  i2c_write(DEFAULT_ADDRESS, reg);
  *val = i2c_read2(DEFAULT_ADDRESS);
  return true;
}

// Provided for backward compatibility only. Not recommended...
int16_t GridEYE_getRegister(unsigned char reg, int8_t len)
{
  if (len == 2)
  {
    uint16_t result = 0;
    GridEYE_getRegister16(reg, &result);
    return GridEYE_convertUnsignedSigned16(result);
  }
  else
  {
    uint8_t result = 0;
    GridEYE_getRegister8(reg, &result);
    return GridEYE_convertUnsignedSigned16((uint16_t)result);
  }
}

// Avoid any ambiguity when casting uint16_t to int16_t
int16_t GridEYE_convertUnsignedSigned16(uint16_t val)
{
  union
  {
    int16_t signed16;
    uint16_t unsigned16;
  } signedUnsigned16;

  signedUnsigned16.unsigned16 = val;

  return signedUnsigned16.signed16;
}

// Avoid any ambiguity when casting int16_t to uint16_t
uint16_t GridEYE_convertSignedUnsigned16(int16_t val)
{
  union
  {
    int16_t signed16;
    uint16_t unsigned16;
  } signedUnsigned16;

  signedUnsigned16.signed16 = val;

  return signedUnsigned16.unsigned16;
}

float GridEYE_convertSigned12ToFloat(uint16_t val)
{
  // val is 12-bit twos complement
  // check if val is negative
  if (val & (1 << 11))
    val |= 0xF000; // Set the other MS bits to 1 to preserve the two's complement
  else
    val &= 0x07FF; // Clear the unused bits - just in case

  return ((float)GridEYE_convertUnsignedSigned16(val)); // GridEYE_convert to int16_t without ambiguity. Cast to float.
}

uint16_t GridEYE_convertFloatToSigned12(float val)
{
  /* int16_t signedVal = round(val); */
  int16_t signedVal = (int16_t)val;
  uint16_t unsignedVal = GridEYE_convertSignedUnsigned16(signedVal); // GridEYE_convert without ambiguity

  if (unsignedVal | (1 << 15)) // If the two's complement value is negative
    return ((unsignedVal & 0x0FFF) | (1 << 11)); // Limit to 12-bits
  else
    return(unsignedVal & 0x07FF);
}

