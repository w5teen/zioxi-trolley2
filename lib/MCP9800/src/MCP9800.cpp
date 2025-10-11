/*******************************************************************************
 * file     MCP9800.cpp
 * author   W Steen
 * 
 * Library for Particle P2 to support the use of MCP9800 I2C temperature sensor
 * 
 * versions
 * v1.0 - First release 26/04/17
 * v1.1 - Modified and stripped down for mcp980X oneshot 9bit resolution only 27/11/17
 * v1.2 - Added isEnabled() in begin() and WITH_LOCK(Wire){} 24/12/20
 * v1.3 - Modified as a library for the P2-PDU-base project added check for Wire already started 03/09/24 
 * v1.4 - added support for interrupt mode alert active low asserted when Ta > Tset and <Thyst
 * v1.5 - changed begin from void to uint8_t to indicate success or failure
********************************************************************************/
#include "MCP9800.h"

// Initialiser with address
// addr uint8_t 0-7 device address selection (pins 7,6,5) MCP9801
// addr uint8_t 0 for MCP9800/A0
// Begin the I2C bus
uint8_t MCP9800::begin(uint8_t addr)
{
	uint8_t code = 0;					//V1.5
    if (addr > 7) addr = 7;             //range limit
    i2caddr = MCP9800_ADDRESS | addr;   //add to device base address
 	
	if (!Wire.isEnabled()) Wire.begin();//don't repeat Wire.begin() if already active V1.3

	WITH_LOCK(Wire)						//V1.2
	{
		Wire.beginTransmission(i2caddr);
		Wire.write((byte) MCP9800_CONFIG_REG);
		Wire.write((byte) ONESHOT9BIT); //configure as oneshot mode and 9 bits resolution
		code = Wire.endTransmission();	//V1.5
	}
	_isAlertOn = false;					//V1.4
	return code;						//V1.5
}

// Read temperature from sensor
// returns Temperature in Celsius (int) or 0 if unsuccessful or -1 if alert active
int MCP9800::readTemperature()
{
    int tempint = 0;
	if (_isAlertOn) return MCP9800_ALERTACTIVE;             //if alert is on return special value and do not reset config values V1.4
    write(i2caddr, MCP9800_CONFIG_REG, SHUTDOWN);           //single read needs to be in shutdown
    write(i2caddr, MCP9800_CONFIG_REG, STARTONESHOT);       //start oneshort read
    delay(31);                                              //9 bit resolution read requires delay of >30mS
	int16_t temp16 = 0;
	uint8_t temp[2] = {0, 0};
	read(i2caddr, MCP9800_TEMPA_REG, temp, 2);
	temp16 = (temp[0] << 8) | temp[1];
	int8_t highByte = (temp16 >> 8);
	uint8_t lowByte = (temp16 & 0xFF);
	tempint = ((highByte << 4) | (lowByte >> 4))/16;
	return tempint;
}

// Set temperature for alert to assert and hysteresis for alert to assert
// power on default tSet = 80C, tHyst = 75C, active low when asserted
// fault queue = 1, 9-bit resolution, continuous conversion mode
// _on bool true to enable alert by setting tSet and tHyst, false to disable by setting to defaults
bool MCP9800::setTempAlert(bool _on, int16_t _tSet, int16_t _tHyst)
{
	bool result = false;
	uint8_t config = 0;				//default config
	uint8_t tSet[2] = {0x50, 0}; 	//tSet is 2 bytes 2 complement default 80C
	uint8_t tHyst[2] = {0x4B, 0}; 	//tHyst is 2 bytes 2 complement default 75C

	if (_tSet < -55 || _tSet > 125) return result; 		//range check return false if out of range
	if (_tHyst < -55 || _tHyst > 125) return result; 	//range check return false if out of range


	if (_on)
	{
		_isAlertOn = true;
		config = ALERTMODE; 							//active low interrupt alert since alert line has pull-up
		tSet[0] = (_tSet >> 8) & 0xFF;
		tSet[1] = 0;
		tHyst[0] = (_tHyst >> 8) & 0xFF;
		tHyst[1] = 0;
	}
	else
	{
		_isAlertOn = false;
		config = ONESHOT9BIT; 							//default config
	}

	write(i2caddr, MCP9800_CONFIG_REG, config);
	write16(i2caddr, MCP9800_TSET_REG, tSet[0], tSet[1]);
	write16(i2caddr, MCP9800_THYST_REG + 1, tHyst[0], tHyst[1]);
	result = true;
	return result;
}

// return true if alert mode is active
bool MCP9800::hasAlertOn(void)
{
	return _isAlertOn;
}

// Write byte to sensor
// addr i2CAddress
// reg Register of sensor
// data byte
void MCP9800::write(uint8_t addr, uint8_t reg, uint8_t data)
{
	WITH_LOCK(Wire)					//V1.2
	{
		Wire.beginTransmission(addr);
		Wire.write(reg);
		Wire.write(data);
		Wire.endTransmission();
	}
}

// Write 2 bytes to sensor V1.4
// addr i2CAddress
// reg Register of sensor
// data0 MS byte, data1 LS byte
void MCP9800::write16(uint8_t addr, uint8_t reg, uint8_t data0, uint8_t data1)
{
	WITH_LOCK(Wire)					//V1.2
	{
		Wire.beginTransmission(addr);
		Wire.write(reg);
		Wire.write(data0);
		Wire.write(data1);
		Wire.endTransmission();
	}
}

// Read byte(s) from sensor
// addr i2CAddress
// reg Register of sensor
// buffer Pointer to buffer
// length Length of data to read
void MCP9800::read(uint8_t addr, uint8_t reg, uint8_t *buffer, uint8_t length)
{
	WITH_LOCK(Wire)					//V1.2
	{
		Wire.beginTransmission(addr);
		Wire.write(reg);
		Wire.endTransmission();
		Wire.requestFrom(addr, length);
		while (Wire.available() < length);
		while (length--) *buffer++ = Wire.read();
	}
}