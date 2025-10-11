/**********************************************************************************
 * MCP7940.h - Library Header for the MCP7940M and MCP7940N Real-Time Clock devices
 * 
 * Class definition header for the MCP7940 from Microchip. Both the MCP7940N (with battery 
 * backup pin and the MCP7940M are supported with this library and they both use the same I2C address.
 * This chip is a Real-Time-Clock with an I2C interface. The data sheet located at: 
 * http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf describes the functionality used in
 * this library.
 **********************************************************************************/

#ifndef _MCP7940_h
#define _MCP7940_h      // Define the name inside guard code

#include "Particle.h"   // Particle data type definitions
#include <time.h>       // For time_t and time functions

class TimeSpan;

const uint8_t  MCP7940_ADDRESS = 0x6F;         // I2C address
const uint8_t  MCP7940_RTCSEC = 0x00;          // RTCSEC Register address
const uint8_t  MCP7940_RTCMIN = 0x01;          // RTCMIN Register address
const uint8_t  MCP7940_RTCHOUR = 0x02;         // RTCHOUR Register address
const uint8_t  MCP7940_RTCWKDAY = 0x03;        // RTCWKDAY Register address
const uint8_t  MCP7940_RTCDATE = 0x04;         // RTCDATE Register address
const uint8_t  MCP7940_RTCMTH = 0x05;          // RTCMTH Register address
const uint8_t  MCP7940_RTCYEAR = 0x06;         // RTCYEAR Register address
const uint8_t  MCP7940_CONTROL = 0x07;         // RTCCONTROL Register address
const uint8_t  MCP7940_OSCTRIM = 0x08;         // RTCOSCTRIM Register address
const uint8_t  MCP7940_EEUNLOCK = 0x09;        // Virtual unlock register on MCP7940x series
const uint8_t  MCP7940_ALM0SEC = 0x0A;         // Alarm 0, ALM0SEC Register address
const uint8_t  MCP7940_ALM0MIN = 0x0B;         // Alarm 0, ALM0MIN Register address
const uint8_t  MCP7940_ALM0HOUR = 0x0C;        // Alarm 0, ALM0HOUR Register address
const uint8_t  MCP7940_ALM0WKDAY = 0x0D;       // Alarm 0, ALM0WKDAY Register address
const uint8_t  MCP7940_ALM0DATE = 0x0E;        // Alarm 0, ALM0DATE Register address
const uint8_t  MCP7940_ALM0MTH = 0x0F;         // Alarm 0, ALM0MTH Register address
const uint8_t  MCP7940_ALM1SEC = 0x11;         // Alarm 1, ALM1SEC Register address
const uint8_t  MCP7940_ALM1MIN = 0x12;         // Alarm 1, ALM1MIN Register address
const uint8_t  MCP7940_ALM1HOUR = 0x13;        // Alarm 1, ALM1HOUR Register address
const uint8_t  MCP7940_ALM1WKDAY = 0x14;       // Alarm 1, ALM1WKDAY Register address
const uint8_t  MCP7940_ALM1DATE = 0x15;        // Alarm 1, ALM1DATE Register address
const uint8_t  MCP7940_ALM1MTH = 0x16;         // Alarm 1, ALM1MONTH Register address
const uint8_t  MCP7940_PWRDNMIN = 0x18;        // Power-Fail, PWRDNMIN Register address
const uint8_t  MCP7940_PWRDNHOUR = 0x19;       // Power-Fail, PWRDNHOUR Register address
const uint8_t  MCP7940_PWRDNDATE = 0x1A;       // Power-Fail, PWDNDATE Register address
const uint8_t  MCP7940_PWRDNMTH = 0x1B;        // Power-Fail, PWRDNMTH Register address
const uint8_t  MCP7940_PWRUPMIN = 0x1C;        // Power-Fail, PWRUPMIN Register address
const uint8_t  MCP7940_PWRUPHOUR = 0x1D;       // Power-Fail, PWRUPHOUR Register address
const uint8_t  MCP7940_PWRUPDATE = 0x1E;       // Power-Fail, PWRUPDATE Register address
const uint8_t  MCP7940_PWRUPMTH = 0x1F;        // Power-Fail, PWRUPMTH Register address
const uint8_t  MCP7940_RAM_ADDRESS = 0x20;     // NVRAM - Start address for SRAM
const uint8_t  MCP7940_ST = 7;                 // MCP7940 register bits. RTCSEC reg
const uint8_t  MCP7940_12_24 = 6;              // RTCHOUR, PWRDNHOUR & PWRUPHOUR
const uint8_t  MCP7940_AM_PM = 5;              // RTCHOUR, PWRDNHOUR & PWRUPHOUR
const uint8_t  MCP7940_OSCRUN = 5;             // RTCWKDAY register
const uint8_t  MCP7940_PWRFAIL = 4;            // RTCWKDAY register
const uint8_t  MCP7940_VBATEN = 3;             // RTCWKDAY register
const uint8_t  MCP7940_LPYR = 5;               // RTCMTH register
const uint8_t  MCP7940_OUT = 7;                // CONTROL register
const uint8_t  MCP7940_SQWEN = 6;              // CONTROL register
const uint8_t  MCP7940_ALM1EN = 5;             // CONTROL register
const uint8_t  MCP7940_ALM0EN = 4;             // CONTROL register
const uint8_t  MCP7940_EXTOSC = 3;             // CONTROL register
const uint8_t  MCP7940_CRSTRIM = 2;            // CONTROL register
const uint8_t  MCP7940_SQWFS1 = 1;             // CONTROL register
const uint8_t  MCP7940_SQWFS0 = 0;             // CONTROL register
const uint8_t  MCP7940_SIGN = 7;               // OSCTRIM register
const uint8_t  MCP7940_ALMPOL = 7;             // ALM0WKDAY register
const uint8_t  MCP7940_ALM0IF = 3;             // ALM0WKDAY register
const uint8_t  MCP7940_ALM1IF = 3;             // ALM1WKDAY register
const uint32_t SECS_1970_TO_2000 = 946684800;  // Seconds between year 1970 and 2000
const uint8_t  MCP7940_RAM_SIZE = 64;          // Size of the RAM in bytes

class DateTime
{
    public:
    DateTime(uint32_t t = SECS_1970_TO_2000);
    DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0);
    DateTime(const DateTime& copy);
    DateTime(const char* date, const char* time);
    uint16_t year() const {return 2000U + yOff;}
    uint8_t month() const {return m;}
    uint8_t day() const {return d;}
    uint8_t hour() const {return hh;}
    uint8_t minute() const {return mm;}
    uint8_t second() const {return ss;}
    uint8_t  dayOfTheWeek() const;
    long     secondstime() const;
    uint32_t unixtime(void) const;
    bool     equals(const DateTime* other);
    DateTime operator+(const TimeSpan& span);
    DateTime operator-(const TimeSpan& span);
    TimeSpan operator-(const DateTime& right);

    protected:
    uint8_t yOff;  // Internal year offset value
    uint8_t m;     // Internal month value
    uint8_t d;     // Internal day value
    uint8_t hh;    // Internal hour value
    uint8_t mm;    // Internal minute value
    uint8_t ss;    // Internal seconds
};

class TimeSpan
{
    public:
    TimeSpan(int32_t seconds = 0);
    TimeSpan(int16_t days, int8_t hours, int8_t minutes, int8_t seconds);
    TimeSpan(const TimeSpan& copy); 
    int16_t  days() const { return _seconds / 86400L; } 
    int8_t   hours() const { return _seconds / 3600 % 24; } 
    int8_t   minutes() const { return _seconds / 60 % 60; } 
    int8_t   seconds() const { return _seconds % 60; }
    int32_t  totalseconds() const { return _seconds; } 
    TimeSpan operator+(const TimeSpan& right); 
    TimeSpan operator-(const TimeSpan& right);

    protected:
    int32_t _seconds;
};

class MCP7940X
{
    public:
    MCP7940X(){}; 
    ~MCP7940X(){};

    bool     setup() const;
    bool     deviceStatus() const;
    bool     deviceStart() const;
    bool     deviceStop() const;

    DateTime now() const;
    void     adjust(const DateTime& dt);
    int8_t   calibrate() const;
    int8_t   calibrate(const int8_t newTrim);
    int8_t   calibrate(const DateTime& dt);
    int8_t   calibrate(const float fMeas) const;
    int8_t   getCalibrationTrim() const;
    uint8_t  weekdayRead() const;
    uint8_t  weekdayWrite(const uint8_t dow) const;
    bool     setMFP(const bool value) const;
    uint8_t  getMFP() const;
    bool     setAlarm(const uint8_t alarmNumber, const uint8_t alarmType, const DateTime& dt, const bool state = true) const;
    void     setAlarmPolarity(const bool polarity) const;
    DateTime getAlarm(const uint8_t alarmNumber, uint8_t& alarmType) const;
    bool     clearAlarm(const uint8_t alarmNumber) const;
    bool     setAlarmState(const uint8_t alarmNumber, const bool state) const;
    bool     getAlarmState(const uint8_t alarmNumber) const;
    bool     isAlarm(const uint8_t alarmNumber) const;
    bool     setBattery(const bool state) const;
    bool     getBattery() const;
    bool     getPowerFail() const;
    bool     clearPowerFail() const;
    DateTime getPowerDown() const;
    DateTime getPowerUp() const;
    int8_t   calibrateOrAdjust(const DateTime& dt);
    int32_t  getPPMDeviation(const DateTime& dt) const;
    void     setSetUnixTime(uint32_t aTime);
    uint32_t getSetUnixTime() const;
    bool     getRtcAsTime(time_t &time);
    bool     getRtcAsTm(struct tm *timeptr);
    bool     isRTCSet() const;
    bool     setRtcFromTime(time_t time, bool lock = true);
    bool     setRtcFromTm(const struct tm *timeptr, bool lock = true); 
	bool     eraseRam();

	/**
	 * @brief Read from RTC RAM using EEPROM-style API
	 *
	 * @param ramAddr The address in the RTC RAM to read from
     * 
	 * @param t The variable to read to. This must be a simple type (bool, int, float, etc.)
     * or struct. It cannot save a c-string (const char *), String, or other class. You
     * typically cannot get any pointers or structs containing pointers.
	 */
    template <typename T> T &get(size_t ramAddr, T &t)
    {
        readRam(ramAddr, (uint8_t *)&t, sizeof(T));
        return t;
    }

	/**
	 * @brief Write from RTC RAM using EEPROM-style API
	 *
	 * @param ramAddr The address in the RTC RAM to write to
     * 
	 * @param t The variable to write from. t is not modified. This must be a simple type (bool, int, float, etc.)
     * or struct. It cannot save a c-string (const char *), String, or other class. You
     * typically cannot save any pointers or structs containing pointers.
	 */
    template <typename T> const T &put(size_t ramAddr, const T &t)
    {
        writeRam(ramAddr, (const uint8_t *)&t, sizeof(T));
        return t;
    }

    /**
     * @brief Low-level read call
     *
	 * @param ramAddr The address in the RTC RAM to read from
	 *
	 * @param data The buffer to read into
	 *
	 * @param dataLen The number of bytes to read
	 *
	 * The dataLen can be larger than the maximum I2C read. Multiple reads will be done if necessary.
     * However do not read past the end of RAM (address 255).
     */
	virtual bool readRam(size_t ramAddr, uint8_t *data, size_t dataLen);

    /**
     * @brief Low-level write call
     *
	 * @param ramAddr The address in the RTC RAM to write to
	 *
	 * @param data The buffer containing the data to write
	 *
	 * @param dataLen The number of bytes to write
	 *
	 * The dataLen can be larger than the maximum I2C write. Multiple writes will be done if necessary.
     * However do not read past the end of RAM (address 255).
     */
	virtual bool writeRam(size_t ramAddr, const uint8_t *data, size_t dataLen);

    // readRAM   Template Function read any number of bytes from the MCP7940 SRAM area
    template <typename T>
    uint8_t readRAM(const uint8_t& addr, T& value) const
    {
        return I2C_read((addr % 64) + MCP7940_RAM_ADDRESS, value);
    }

    // writeRAM   Template Function write any number of bytes to the MCP7940 SRAM area
    template <typename T>
    uint8_t writeRAM(const uint8_t& addr, const T& value) const
    {
        uint8_t i = I2C_write((addr % 64) + MCP7940_RAM_ADDRESS, value);
        return i;
    }

    private:
    uint32_t _SetUnixTime = 0;

    // I2C_read  Template Function read any number of bytes from the MCP7940
    template <typename T>
    uint8_t I2C_read(const uint8_t address, T& value) const
    {
        uint8_t i = 0;                                       // Initialize byte counter
        WITH_LOCK(Wire)                                      // Lock the I2C bus
        {
            Wire.beginTransmission(MCP7940_ADDRESS);         // Address the I2C device
            Wire.write(address);                             // Send register address to read from
            i = Wire.endTransmission();                      // Close transmission and save status
        }
            if (i == 0)                                      // no error
            {
                Wire.requestFrom(MCP7940_ADDRESS, sizeof(T));// Request a block of data
                uint8_t* bytePtr = (uint8_t*) &value;        // Declare pointer to start of structure
                for (i = 0; i < sizeof(T); i++)              // Loop for each byte to be read
                {
                    *bytePtr++ = Wire.read();                // Read a byte
                }
            }
        return i;                                        // return number of bytes read
    }

    // I2C_write Template Function write any number of bytes to the MCP7940                                           **
    template <typename T>
    uint8_t I2C_write(const uint8_t address, const T& value) const
    {
        uint8_t i = 0;                                   // Initialize byte counter
        WITH_LOCK(Wire)                            // Lock the I2C bus
        {
            Wire.beginTransmission(MCP7940_ADDRESS);   // Address the I2C device
            Wire.write(address);                       // Send register address to read from
            Wire.write((uint8_t*)&value, sizeof(T));   // write the data
            i = Wire.endTransmission();        // close transmission and save status
        }

        if (i == 0) i = sizeof(T);                 // return number of bytes on success
        return i;                                  // return the number of bytes written
    } 
    
    uint8_t readByte(const uint8_t addr) const;  // Read 1 byte from address on I2C
    uint8_t bcd2int(const uint8_t bcd) const;    // convert BCD digits to integer
    uint8_t int2bcd(const uint8_t dec) const;    // convert integer to BCD
    void    clearRegisterBit(const uint8_t reg, const uint8_t b) const;  // Clear a bit, values 0-7
    void    setRegisterBit(const uint8_t reg, const uint8_t b) const;    // Set   a bit, values 0-7
    void    writeRegisterBit(const uint8_t reg, const uint8_t b, bool bitvalue) const; // Clear a bit, values 0-7
    uint8_t readRegisterBit(const uint8_t reg, const uint8_t b) const;  // Read  a bit, values 0-7
};

#endif // _MCP7940_h