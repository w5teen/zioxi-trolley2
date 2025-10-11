/*******************************************************************************
 * file     MCP9800.h
 * author   W Steen
 * 
 * Library for Particle P2 to support the use of MCP9800 I2C temperature sensor
 * 
 * versions
 * v1.0 - First release 26/04/17
 * v1.1 - Modified and stripped down for mcp980X oneshot 9bit resolution only 27/11/17
 * v1.2 - Added isEnabled() in begin() and WITH_LOCK(Wire){} 24/12/20
 * v1.3 - Modified as a library for the P2-PDU-base project 03/09/24
 * v1.4 - added support for interrupt mode alert active low asserted when Ta > Tset and <Thyst
 * v1.5 - changed begin from void to uint8_t to indicate success or failure
*
********************************************************************************/
#ifndef MCP9800_H
#define MCP9800_H

#include <Particle.h>

#define ONESHOT9BIT         0x80
#define SHUTDOWN            0x01
#define STARTONESHOT        0x81	//data sheet says 0x81 but 0x80 works V1.4
#define ALERTMODE           0x02

#define MCP9800_TEMPA_REG   0x00
#define MCP9800_CONFIG_REG  0x01
#define MCP9800_THYST_REG   0x02
#define MCP9800_TSET_REG    0x03

#define MCP9800_ADDRESS     0x48
#define MCP9800_ALERTACTIVE -1

class MCP9800 {
public:

	uint8_t begin(uint8_t address);
	int readTemperature();
	bool setTempAlert(bool on, int16_t tSet, int16_t tHyst); //V1.4
	bool hasAlertOn(); //V1.4

private:

	void write(uint8_t addr, uint8_t reg, uint8_t data);
	void write16(uint8_t addr, uint8_t reg, uint8_t data0, uint8_t data1);
	void read(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);
	uint8_t i2caddr;
	bool _isAlertOn = false;
};

#endif  //