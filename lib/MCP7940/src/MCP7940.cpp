/**********************************************************************************
 * MCP7940.h - Library Header for the MCP7940M and MCP7940N Real-Time Clock devices
 * 
 * Class definition header for the MCP7940 from Microchip. Both the MCP7940N (with battery 
 * backup pin and the MCP7940M are supported with this library and they both use the same I2C address.
 * This chip is a Real-Time-Clock with an I2C interface. The data sheet located at: 
 * http://ww1.microchip.com/downloads/en/DeviceDoc/20002292B.pdf describes the functionality used in
 * this library.
 * 
 * Added application specific logger - 14-Sep-25
 **********************************************************************************/

#include "MCP7940.h"

Logger MCP7940N_Logger("app.mcp7940N_RTC");

#define bitWrite(X, Y, Z) (X | Z<<Y)
#define bitRead(X, Y) (X>>Y & 0x01)

// Define the number of days in each month
const uint8_t   daysInMonth[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// returns the number of days from a given Y M D value
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d)
{
    if (y >= 2000) { y -= 2000; }  // of if-then year is greater than 2000
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)  // Add number of days for each month
    {
        days += pgm_read_byte(daysInMonth + i - 1);
    }                         // of for-next loop for each month
    if (m > 2 && y % 4 == 0)  // Deal with leap years
    {
        ++days;
    }                                          // of if-then a leap year
    if (((y % 100) == 0) && ((y % 400) != 0))  // if / by 100 but not by 400 then it is not a l.y.
    {
        --days;
    }                                         // of if-then special leap year
    return days + 365 * y + (y + 3) / 4 - 1;  // Return computed value
}

// return the number of seconds for a D H M S value
static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s)
{
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

// convert character string representation to decimal
static uint8_t conv2d(const char* p)
{
    uint8_t v = 0;
    if ('0' <= *p && *p <= '9') { v = *p - '0'; }  // of if-then character in range
    return 10 * v + *++p - '0';
}

// DateTime Constructor - DateTime constructor instantiates the class. This implementation ignores
// time zones and DST changes. It also ignores leap seconds. t is unixtime in seconds since the year 1970.
DateTime::DateTime(uint32_t t)
{
    if (t < SECS_1970_TO_2000)  {t = 0;}  // set to lowest possible date 2000-01-01
    else                        {t -= SECS_1970_TO_2000;}  // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t  leap;
    
    for (yOff = 0;; ++yOff)
    {
        leap = yOff % 4 == 0;
        if (days < (uint16_t)365 + leap) break;
        days -= 365 + leap;
    }

    for (m = 1;; ++m)
    {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2) ++daysPerMonth;
        if (days < daysPerMonth) break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

// DateTime Constructor - DateTime constructor instantiates the class. This implementation
// sets each component of the date/time separately.
DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{
    if (year >= 2000) { year -= 2000; }             //year is greater than 2000 for offset
    yOff = year;
    m    = month;
    d    = day;
    hh   = hour;
    mm   = min;
    ss   = sec;
} 

// DateTime Constructor - DateTime constructor instantiates the class. This implementation
// copies from a DateTime instance value.
DateTime::DateTime(const DateTime& copy): yOff(copy.yOff), m(copy.m), d(copy.d), hh(copy.hh), mm(copy.mm), ss(copy.ss)
{
}

// DateTime Constructor - DateTime constructor instantiates the class. This implementation uses a
// Date string formatted as "MMM DD YYYY" and a time string formatted as "hh:mm:ss" for the value
DateTime::DateTime(const char* date, const char* time)
{
    yOff = conv2d(date + 9);
    switch (date[0])
    {
        case 'J': m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7); break;  // Jan June July
        case 'F': m = 2; break;                                                  // February
        case 'A': m = date[2] == 'r' ? 4 : 8; break;                             // April August
        case 'M': m = date[2] == 'r' ? 3 : 5; break;                             // March May
        case 'S': m = 9; break;                                                  // September
        case 'O': m = 10; break;                                                 // October
        case 'N': m = 11; break;                                                 // November
        case 'D': m = 12; break;                                                 // December
    }
    d  = conv2d(date + 4);  // Compute the day
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);
}

// DateTime method to return the current day-of-week where Monday is day 1, Sunday is 7
uint8_t DateTime::dayOfTheWeek() const
{
    uint16_t day = date2days(yOff, m, d);  // compute the number of days
    uint8_t  dow = ((day + 6) % 7);        // Jan 1, 2000 is a Saturday, i.e. 6
    if (dow == 0)                          // Correction for Sundays
    {
        dow = 7;
    }
    return dow;
}

// DateTime method to return the UNIX time, which is seconds since 1970-01-01 00:00:00
uint32_t DateTime::unixtime(void) const
{
    uint16_t days = date2days(yOff, m, d);        // Compute days
    MCP7940N_Logger.trace("MCP7940N unixtime days since 2000-01-01: %i uOff: %i m: %i d: %i", days, yOff, m, d);
    uint32_t t    = time2long(days, hh, mm, ss);  // Compute seconds
    MCP7940N_Logger.trace("MCP7940N unixtime seconds since 2000-01-01: %lu hh: %i mm: %i ss: %i", t, hh, mm, ss);
    t += SECS_1970_TO_2000;                       // Add time from 1970 to 2000
    return t;
}

// DateTime method to return the time in seconds since the year 2000-01-01 00:00:00
long DateTime::secondstime(void) const
{
    uint16_t days = date2days(yOff, m, d);
    long     t    = time2long(days, hh, mm, ss);
    return t;
}

// DateTime method to compare 2 DateTime objects, return true if they are equal and false if not
bool DateTime::equals(const DateTime* other)
{
    return (this->yOff == other->yOff && this->m == other->m && this->d == other->d &&
            this->hh == other->hh && this->mm == other->mm && this->ss == other->ss);
}

// overloaded "+" operator for class DateTime - sum of DateTime and TimeSpan instances
DateTime DateTime::operator+(const TimeSpan& span)
{
    return DateTime(unixtime() + span.totalseconds());
}

// overloaded "-" operator for class DateTime - Difference of DateTime and TimeSpan instances
DateTime DateTime::operator-(const TimeSpan& span)
{
    return DateTime(unixtime() - span.totalseconds());
}

// overloaded "-" operator for class DateTime - Difference of two DateTime class instances
TimeSpan DateTime::operator-(const DateTime& right)
{
    return TimeSpan(unixtime() - right.unixtime());
}

TimeSpan::TimeSpan(int32_t seconds) : _seconds(seconds) {}
TimeSpan::TimeSpan(int16_t days, int8_t hours, int8_t minutes, int8_t seconds) : _seconds((int32_t)days * 86400L + (int32_t)hours * 3600 + (int32_t)minutes * 60 + seconds) {}
TimeSpan::TimeSpan(const TimeSpan& copy) : _seconds(copy._seconds) {}
TimeSpan TimeSpan::operator+(const TimeSpan& right) {return TimeSpan(_seconds + right._seconds);}
TimeSpan TimeSpan::operator-(const TimeSpan& right) {return TimeSpan(_seconds - right._seconds);}

// setup() method, returns true if the device is found and Wire.begin() is called if needed
bool MCP7940X::setup() const
{
    if (!Wire.isEnabled()) Wire.begin();
    byte result = 0;

    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(MCP7940_ADDRESS);  // Address the MCP7940
        result = Wire.endTransmission();
    }

    MCP7940N_Logger.trace("MCP7940N setup: %i", result);

    return (bool) (result == 0);
    /*
    clearRegisterBit(MCP7940_RTCHOUR, MCP7940_12_24);  // Use 24 hour clock
    setRegisterBit(MCP7940_CONTROL, MCP7940_ALMPOL);   // assert alarm low, default high??
    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(MCP7940_ADDRESS);
        Wire.write(MCP7940_CONTROL);
        Wire.write(0);    //clear out the entire control register
        Wire.endTransmission();
    }

    if (isBatteryBackup)
    {
        setRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN);
    }
    else
    {
        clearRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN);
    }

    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(MCP7940_ADDRESS);
        Wire.write(MCP7940_RTCSEC);
        Wire.endTransmission();
    }

    Wire.requestFrom(MCP7940_ADDRESS, 1);
    delay(1);
    byte rtcSecondRegister = Wire.read();//read out seconds
    rtcSecondRegister |= 0x80;// flip the start bit to ON
    
    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(MCP7940_ADDRESS);
        Wire.write(MCP7940_RTCSEC);
        Wire.write(rtcSecondRegister);//write it back in... now the RTC is running
        Wire.endTransmission();
    }
    */
}

// helper to read a single byte from the device address
uint8_t MCP7940X::readByte(const uint8_t addr) const
{
    uint8_t dataByte = 0;
    I2C_read(addr, dataByte);
    return dataByte;
}

// helper to clear a specified bit in a register on the device
void MCP7940X::clearRegisterBit(const uint8_t reg, const uint8_t b) const
{
    I2C_write(reg, (uint8_t)(readByte(reg) & ~(1 << b)));
}

// helper to set a specific bit in a register on the device
void MCP7940X::setRegisterBit(const uint8_t reg, const uint8_t b) const
{
    I2C_write(reg, (uint8_t)(readByte(reg) | (1 << b)));
}

// helper to write a specific bit in a register true = 1, false = 0
void MCP7940X::writeRegisterBit(const uint8_t reg, const uint8_t b, const bool bitvalue) const
{
    bitvalue ? setRegisterBit(reg, b) : clearRegisterBit(reg, b);
}

// helper to read a specific bit from a register
uint8_t MCP7940X::readRegisterBit(const uint8_t reg, const uint8_t b) const
{
    return bitRead(readByte(reg), b);
}

// helper to convert a BCD encoded value into integer representation
uint8_t MCP7940X::bcd2int(const uint8_t bcd) const
{
    return ((bcd / 16 * 10) + (bcd % 16));
}

// helper to convert an integer to a BCD encoded value
uint8_t MCP7940X::int2bcd(const uint8_t dec) const
{
    return ((dec / 10 * 16) + (dec % 10));
}

// Returns the status of the device true if the device is running, false if not
bool MCP7940X::deviceStatus() const
{
    uint8_t result = 0;
    result = readRegisterBit(MCP7940_RTCSEC, MCP7940_ST);
    MCP7940N_Logger.trace("MCP7940N device status: %i", result);
    return (bool) (result == 1);
}

// Start the MCP7940 oscillator returns true if success false if not
bool MCP7940X::deviceStart() const
{
    uint8_t oscillatorStatus = 0;
    setRegisterBit(MCP7940_RTCSEC, MCP7940_ST);  // Set the ST bit in the RTCSEC register
    for (uint8_t j = 0; j < 255; j++)
    {
        delay(1);
        oscillatorStatus = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_OSCRUN);
        if (oscillatorStatus == 1) break;
    }
    return (oscillatorStatus == 1);
}

// Stop the MCP7940 oscillator returns true if success false if not
bool MCP7940X::deviceStop() const
{
    clearRegisterBit(MCP7940_RTCSEC, MCP7940_ST);  // clear the ST bit in the RTCSEC register
    uint8_t oscillatorStatus = 0;
    for (uint8_t j = 0; j < 255; j++) 
    {
        delay(1);
        oscillatorStatus = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_OSCRUN);
        if (oscillatorStatus == 0) break;
    }
    return (oscillatorStatus == 0);
}

// Get the current date/time
DateTime MCP7940X::now() const
{
    uint8_t readBuffer[7] = {0};
    I2C_read(MCP7940_RTCSEC, readBuffer);
    MCP7940N_Logger.trace("MCP7940N now bcd: %02x-%02x-%02x %02x:%02x:%02x", readBuffer[6] & 0x7F, readBuffer[5] & 0x1F, readBuffer[4] & 0x3F, readBuffer[2] & 0x3F, readBuffer[1] & 0x7F, readBuffer[0] & 0x7F);
    return DateTime(bcd2int(readBuffer[6] & 0x7F) + 2000, bcd2int(readBuffer[5] & 0x1F),
                    bcd2int(readBuffer[4] & 0x3F), bcd2int(readBuffer[2] & 0x3F),
                    bcd2int(readBuffer[1] & 0x7F), bcd2int(readBuffer[0] & 0x7F));
}

// Get the time from the RTC as a time_t (seconds since 1970-01-01 00:00:00 UTC) true if success or false if error
// The time value is basically the same as what would be returned from `Time.now()` except its from the RTC
bool MCP7940X::getRtcAsTime(time_t &time)
{
    if (!isRTCSet()) return false;  // If RTC is not set, return false
    struct tm tmstruct;

    bool result = getRtcAsTm(&tmstruct);
    if (result)
    {
        time = mktime(&tmstruct);
    }

    return result;   
}

// Get the time from the RTC as a 
bool MCP7940X::getRtcAsTm(struct tm *timeptr)
{
    uint8_t readBuffer[7] = {0};
    I2C_read(MCP7940_RTCSEC, readBuffer);
    timeptr->tm_sec = bcd2int(readBuffer[0] & 0x7F);
    timeptr->tm_min = bcd2int(readBuffer[1] & 0x7F);
    timeptr->tm_hour = bcd2int(readBuffer[2] & 0x3F);
    timeptr->tm_mday = bcd2int(readBuffer[4] & 0x3F);
    timeptr->tm_mon = bcd2int(readBuffer[5] & 0x1F) - 1;
    timeptr->tm_year = bcd2int(readBuffer[6]) + 2000;
    timeptr->tm_wday = weekdayRead() - 1;  // Convert to 0-6 range
    timeptr->tm_yday = date2days(timeptr->tm_year, timeptr->tm_mon + 1, timeptr->tm_mday) - 1; // Convert to 0-365 range
    timeptr->tm_isdst = 0;  // No DST support in this library
    return (readBuffer[6] != 0);  // Return true if year is not 0, false if it is
}

// return true if the RTC has been set, false if not
bool MCP7940X::isRTCSet() const
{
    bool set = false;  // Assume RTC is not set
    // Check the ST bit in the RTCSEC register to see if the RTC is running
    uint8_t result = readRegisterBit(MCP7940_RTCSEC, MCP7940_ST);
    if (result == 1)                                //RTC is running
    {
        uint8_t readBuffer[7] = {0};
        I2C_read(MCP7940_RTCSEC, readBuffer);
        uint16_t _year = bcd2int(readBuffer[6] & 0x7F);
        if ((_year == 0 || _year == 1) && bcd2int(readBuffer[5] & 0x1F) == 0 && bcd2int(readBuffer[4] & 0x3F) == 0 && bcd2int(readBuffer[2] & 0x3F) == 0) set = false;     //if year is 0 or 1 , month is 0, day is 0 and hour is 0 then RTC is not set
        else set = true;  // RTC is set if year is not 0 and month is not 0 and day is not 0
        MCP7940N_Logger.trace("MCP7940N isRTCset bcd: %02x-%02x-%02x %02x result: %c", readBuffer[6] & 0x7F, readBuffer[5] & 0x1F, readBuffer[4] & 0x3F, readBuffer[2] & 0x3F, set?'T':'F');
    }
    return set;
}

bool MCP7940X::setRtcFromTime(time_t time, bool lock)
{
    struct tm *tm = gmtime(&time);
    return setRtcFromTm(tm, lock);
}

bool MCP7940X::setRtcFromTm(const struct tm *timeptr, bool lock)
{
    if (lock) {Wire.lock();}
    deviceStop();  // Stop the oscillator
    uint8_t numbytes = 0;  // Number of bytes written to the device
    numbytes += I2C_write(MCP7940_RTCSEC, int2bcd(timeptr->tm_sec));
    numbytes += I2C_write(MCP7940_RTCMIN, int2bcd(timeptr->tm_min));
    numbytes += I2C_write(MCP7940_RTCHOUR, int2bcd(timeptr->tm_hour));
    weekdayWrite(timeptr->tm_wday);
    numbytes += I2C_write(MCP7940_RTCMTH, int2bcd(timeptr->tm_mon + 1));
    numbytes += I2C_write(MCP7940_RTCDATE, int2bcd(timeptr->tm_mday));
    numbytes += I2C_write(MCP7940_RTCYEAR, int2bcd(timeptr->tm_year - 2000));
    deviceStart();  // Restart the oscillator
    weekdayWrite(timeptr->tm_wday);                        // Silicon errata issue 4
    _SetUnixTime = mktime((tm *)timeptr);                  // Store time of last change

    if (lock) {Wire.unlock();}

    return (numbytes == 6);  // Return true if all bytes were written
}

// Erases the RTC RAM to 0x00 values
bool MCP7940X::eraseRam()
{
    uint8_t array[MCP7940_RAM_SIZE] = {0};  // Create an array of 64 bytes to write
    uint8_t number = I2C_write(MCP7940_RAM_ADDRESS, array);  // Write the array to the RTC RAM
    return (number == MCP7940_RAM_SIZE);  // Return true if all bytes were written
}

// Get the date/time that the power went off
DateTime MCP7940X::getPowerDown() const
{
    uint8_t min{0}, hr{0}, day{0}, mon{0};  // temporary storage set to 0
    uint8_t readBuffer[4];
    if (I2C_read(MCP7940_PWRDNMIN, readBuffer) > 0)
    {
        min = bcd2int(readBuffer[0] & 0x7F);     // Clear high bit in minutes
        hr  = bcd2int(readBuffer[1] & 0x3F);     // Clear all but 6 LSBs
        day = bcd2int(readBuffer[2] & 0x3F);     // Clear 2 high bits for day-of-month
        mon = bcd2int(readBuffer[3] & 0x1F);     // Clear 3 high bits for Month
    }
    return DateTime(0, mon, day, hr, min, 0);    // Return class value
}

// Get the date/time that the power went back on
DateTime MCP7940X::getPowerUp() const
{
    uint8_t min{0}, hr{0}, day{0}, mon{0};  // temporary storage set to 0
    uint8_t readBuffer[4];
    if (I2C_read(MCP7940_PWRUPMIN, readBuffer) > 0)
    {
        min = bcd2int(readBuffer[0] & 0x7F);     // Clear high bit in minutes
        hr  = bcd2int(readBuffer[1] & 0x3F);     // Clear all but 6 LSBs
        day = bcd2int(readBuffer[2] & 0x3F);     // Clear 2 high bits for day-of-month
        mon = bcd2int(readBuffer[3] & 0x1F);     // Clear 3 high bits for Month
    } 
    return DateTime(0, mon, day, hr, min, 0);    // Return class value
}

// Set the current date/time
void MCP7940X::adjust(const DateTime& dt)
{
    deviceStop();  // Stop the oscillator
    I2C_write(MCP7940_RTCSEC, int2bcd(dt.second()));
    I2C_write(MCP7940_RTCMIN, int2bcd(dt.minute()));
    I2C_write(MCP7940_RTCHOUR, int2bcd(dt.hour()));
    weekdayWrite(dt.dayOfTheWeek());                        // Update the weekday
    I2C_write(MCP7940_RTCMTH, int2bcd(dt.month()));         // Month, ignore R/O leapyear bit
    I2C_write(MCP7940_RTCDATE, int2bcd(dt.day()));          // Write the day of month
    I2C_write(MCP7940_RTCYEAR, int2bcd(dt.year() - 2000));  // Write the year
    deviceStart();                                          // Restart the oscillator
    weekdayWrite(dt.dayOfTheWeek());                        // Silicon errata issue 4
    _SetUnixTime = dt.unixtime();                           // Store time of last change
    MCP7940N_Logger.trace("MCP7940N adjusted to: %02i-%02i-%02i %02i:%02i:%02i unix: %lu", dt.year() - 2000, dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), _SetUnixTime);
}

// Get the current weekday number from the RTC 1-7
uint8_t MCP7940X::weekdayRead() const
{
    return readByte(MCP7940_RTCWKDAY) & 0x07;  // no need to convert, values 1-7
}

//Set the RTC weekday number 1 Monday to 7 Sunday
uint8_t MCP7940X::weekdayWrite(const uint8_t dow) const
{
    uint8_t retval = (readByte(MCP7940_RTCWKDAY) & 0xF8) | dow;  // Read, mask DOW bits & add DOW
    if (dow > 0 && dow < 8)  // If parameter is in range, then
    {
        I2C_write(MCP7940_RTCWKDAY, retval);  // Write the register
        retval = dow;                         // set the return value
    }
    return retval;
}

// Calibrate the MCP7940 When called with no parameters the internal calibration is reset to 0
int8_t MCP7940X::calibrate() const
{
    clearRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);  // fine trim mode on, to be safe
    I2C_write(MCP7940_OSCTRIM, (uint8_t)0);              // Write zeros to the trim register
    return (0);
}

// Calibrate the MCP7940 with one signed integer value that is used as the new trim value
int8_t MCP7940X::calibrate(const int8_t newTrim)
{
    int8_t trim = abs(newTrim);  // Make a local copy of absolute value
    if (newTrim < 0)             // if the trim is less than 0
    {
        trim = 0x80 | trim;                            // set non-excess 128 negative val
    }                                                  // of if-then value of trim is less than 0
    clearRegisterBit(MCP7940_CONTROL, MCP7940_CRSTRIM);// fine trim mode on, to be safe
    I2C_write(MCP7940_OSCTRIM, trim);                  // Write value to the trim register
    _SetUnixTime = now().unixtime();                   // Store time of last change
    return newTrim;
}

// Calibrate the MCP7940 with a DateTime value that is used to compute the ppm deviation
int8_t MCP7940X::calibrate(const DateTime& dt)
{
    int32_t ppm = getPPMDeviation(dt);
    adjust(dt);  // set the new Date-Time value
    ppm          = constrain(ppm, -130, 130);
    int16_t trim = readByte(MCP7940_OSCTRIM);  // Read current trim register value
    if (trim >> 7)                             // use negative value if necessary
    {
        trim = (~0x80 & trim) * -1;
    }                                    // of if-then trim is set
    trim += ppm * 32768 * 60 / 2000000;  // compute the new trim value
    trim = constrain(trim, -127, 127);   // Clamp to value range
    return calibrate((const int8_t)trim);
}

// Calibrate the MCP7940 with one floating point value that is used as the measured frequency
int8_t MCP7940X::calibrate(const float fMeas) const
{
    int16_t  trim   = getCalibrationTrim();  // Get the current trim
    uint32_t fIdeal = 1;

    trim += ((fMeas - (float)fIdeal) * (32768.0 / fIdeal) * 60.0) / 2.0;  // Use formula from datasheet
    if (trim > 127)  // Force number ppm to be in range
    {
        trim = 127;
    }
    else if (trim < -127)
    {
        trim = -127;
    }
    int8_t returnTrim = calibrate((int8_t)trim);  // Set the new trim value
    return (returnTrim);
}

// Calibrate the MCP7940 if the ppm deviation is < 130 and > -130 else Adjust the datetime
int8_t MCP7940X::calibrateOrAdjust(const DateTime& dt)
{
    int32_t ppm = getPPMDeviation(dt);
    if ((ppm > 130) || (ppm < -130))
    {
        adjust(dt);
        return (int8_t) readByte(MCP7940_OSCTRIM);
    }
    else
    {
        return calibrate(dt);
    }
}

// Returns the ppm deviation since the last time the clock was set
// One hour deviation in 6 months is 225 ppm
int32_t MCP7940X::getPPMDeviation(const DateTime& dt) const
{
    int32_t SecDeviation = dt.unixtime() - now().unixtime();  // Get difference in seconds
    int32_t ExpectedSec  = dt.unixtime() - _SetUnixTime;      // Get number of seconds since set
    int32_t ppm = 1000000 * SecDeviation / ExpectedSec;       // Multiply first to avoid truncation
    return ppm;
}

// Sets the time the clock was last calibrated or adjusted - testing only use
void MCP7940X::setSetUnixTime(uint32_t aTime)
{
    _SetUnixTime = aTime;
}

// Returns the time the clock was last calibrated or adjusted - testing only use
uint32_t MCP7940X::getSetUnixTime() const
{
    return _SetUnixTime;
}

// Returns the current calibration trim value
int8_t MCP7940X::getCalibrationTrim() const
{
    uint8_t trim = readByte(MCP7940_OSCTRIM);   // read the register
    if (trim >> 7)
    {                                           // If trim is negative, the convert
        trim = (0x7F & trim) * -1;              // convert to excess128 value
    }
    return ((int8_t)trim);
}

// Sets the MFP (Multifunction Pin) to the requested state - returns true on success otherwise false
bool MCP7940X::setMFP(const bool value) const
{
    if ((readByte(MCP7940_CONTROL) & 0x70) != 0)  // Get Control register, error if SQWEN/ALM1EN/ALM0EN set
    {
        return false;
    }
    writeRegisterBit(MCP7940_CONTROL, MCP7940_OUT, value);
    return true;
}

// Gets the MFP (Multifunction Pin) value - returns 0 for LOW, 1 for HIGH, 2 for alarm output, 3 for square wave output
uint8_t MCP7940X::getMFP() const
{
    uint8_t controlRegister = readByte(MCP7940_CONTROL);  // Get control register contents
    if (controlRegister & (1 << MCP7940_SQWEN))           // Square wave output enabled
    {
        return 3;  // MFP in SQW output mode
    }
    else if ((controlRegister & (1 << MCP7940_ALM0EN)) | (controlRegister & (1 << MCP7940_ALM1EN)))
    {                                                     // One or both alarms enabled, and no square wave output.
        return 2;                                         // MFP in alarm output mode
    }
    return bitRead(controlRegister, MCP7940_OUT);         // MFP in manual mode, return value
}

/* Sets one of the 2 alarms
*  In order to configure the alarm modules, the following steps need to be performed in order:
*   1. Load the timekeeping registers and enable the oscillator
*   2. Configure the ALMxMSK<2:0> bits to select the desired alarm mask
*   3. Set or clear the ALMPOL bit according to the desired output polarity
*   4. Ensure the ALMxIF flag is cleared
*   5. Based on the selected alarm mask, load the alarm match value into the appropriate register(s)
*   6. Enable the alarm module by setting the ALMxEN bit
* There are two ALMPOL bits - one in the ALM0WKDAY register which can be written, one in the ALM1WKDAY
* Returns true if the alarm is set, false if not or if the alarm number is not 0 or 1
*/
bool MCP7940X::setAlarm(const uint8_t alarmNumber, const uint8_t alarmType, const DateTime& dt, const bool state) const
{
    bool result = false;
    if (alarmNumber < 2 && alarmType < 8 && alarmType != 5 && alarmType != 6 && deviceStart())
    {
        clearRegisterBit(MCP7940_CONTROL, alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN);  // Turn off the alarm
        uint8_t offset        = 7 * alarmNumber;                          // Offset to be applied
        uint8_t wkdayRegister = readByte(MCP7940_ALM0WKDAY + offset);     // Load register to memory
        wkdayRegister &= ((1 << MCP7940_ALM0IF) | (1 << MCP7940_ALMPOL)); // Keep ALMPOL and ALMxIF bits
        wkdayRegister |= alarmType << 4;                                  // Set 3 bits from alarmType
        wkdayRegister |= (dt.dayOfTheWeek() & 0x07);                      // Set 3 bits for dow from date
        I2C_write(MCP7940_ALM0WKDAY + offset, wkdayRegister);             // Write alarm mask
        I2C_write(MCP7940_ALM0SEC + offset, int2bcd(dt.second()));        // Write seconds, keep device off
        I2C_write(MCP7940_ALM0MIN + offset, int2bcd(dt.minute()));        // Write the minutes value
        I2C_write(MCP7940_ALM0HOUR + offset, int2bcd(dt.hour()));         // Also re-sets the 24Hour clock on
        I2C_write(MCP7940_ALM0DATE + offset, int2bcd(dt.day()));          // Write the day of month
        I2C_write(MCP7940_ALM0MTH + offset, int2bcd(dt.month()));         // Month, ignore R/O leap-year bit
        setAlarmState(alarmNumber, state);                                // Set the requested alarm to state
        result = true;
    }
    return result;
}

// Sets both the alarm polarities for MFP pin - false for high when no alarm, true for low when no alarm
// Note: the MFP pin is open collector, it needs an external pull-up resistor
// if both alarms are set then false means MFP high when no alarm or one alarm, low when both alarms go off, best true when two
void MCP7940X::setAlarmPolarity(const bool polarity) const
{
    writeRegisterBit(MCP7940_ALM0WKDAY, MCP7940_ALMPOL, polarity);  // Write polarity to the ALMPOL bit
    writeRegisterBit(MCP7940_ALM1WKDAY, MCP7940_ALMPOL, polarity);  // Write polarity to the ALMPOL bit
}

// Returns the DateTime for the given alarm after setting the alarm type or DateTime(0) if alarmNumber is not 0 or 1
DateTime MCP7940X::getAlarm(const uint8_t alarmNumber, uint8_t& alarmType) const
{
    if (alarmNumber > 1) return DateTime(0);      // alarm number out of range

    uint8_t offset = 7 * alarmNumber;  // Offset to be applied
    alarmType      = (readByte(MCP7940_ALM0WKDAY + offset) >> 4) & 0x07;  // get 3 bits for alarmType
    uint8_t ss     = bcd2int(readByte(MCP7940_ALM0SEC + offset) & 0x7F);  // Clear high bit in seconds
    uint8_t mm     = bcd2int(readByte(MCP7940_ALM0MIN + offset) & 0x7F);  // Clear high bit in minutes
    uint8_t hh     = bcd2int(readByte(MCP7940_ALM0HOUR + offset) & 0x3F);  // Clear high bits in hours
    uint8_t d      = bcd2int(readByte(MCP7940_ALM0DATE + offset) & 0x3F);  // Clear 2 high bits for day-of-month
    uint8_t m      = bcd2int(readByte(MCP7940_ALM0MTH + offset) & 0x1F);  // Clear 3 high bits for Month
    uint16_t y     = 0;  // Year is not part of the alarms
    return DateTime(y, m, d, hh, mm, ss);
}

// Returns true if alarm cleared, false if alarmNumber is not 0 or 1
bool MCP7940X::clearAlarm(const uint8_t alarmNumber) const
{
    if (alarmNumber > 1) return false;            // alarm number out of range
    clearRegisterBit(alarmNumber ? MCP7940_ALM1WKDAY : MCP7940_ALM0WKDAY, MCP7940_ALM0IF);  // reset register bit
    return true;
}

// Returns true if alarm state is set, false if alarmNumber is not 0 or 1
bool MCP7940X::setAlarmState(const uint8_t alarmNumber, const bool state) const
{
    if (alarmNumber > 1) return false;          // alarm number out of range
    writeRegisterBit(MCP7940_CONTROL, alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN, state);  // Overwrite register bit
    return true;
}

// Returns true or false if a given alarm is active or not, false if alarm number not 0 or 1
bool MCP7940X::getAlarmState(const uint8_t alarmNumber) const
{
  if (alarmNumber > 1) return false;          // out of range alarm number
  return (bool) readRegisterBit(MCP7940_CONTROL, alarmNumber ? MCP7940_ALM1EN : MCP7940_ALM0EN);  // Get state of alarm
}

// Return true is alarm active, false if not or alarmNumber isn't 0 or 1
bool MCP7940X::isAlarm(const uint8_t alarmNumber) const
{
    if (alarmNumber > 1) return false;      // alarmNumber range check
    return (bool) readRegisterBit(alarmNumber ? MCP7940_ALM1WKDAY : MCP7940_ALM0WKDAY, MCP7940_ALM0IF);  // Get alarm state
}

// Enable battery backup (true) disable (false) returns state of battery backup
bool MCP7940X::setBattery(const bool state) const
{
    writeRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN, state);
    return (state);
}

// returns true for battery backup on, false if off
bool MCP7940X::getBattery() const
{
    bool status = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_VBATEN);
    return status;
}

// Returns the power failure status, true if power failure has occurred
bool MCP7940X::getPowerFail() const
{
    bool status = readRegisterBit(MCP7940_RTCWKDAY, MCP7940_PWRFAIL);
    return status;
} 

// Clears the power failure status flag - always returns true
bool MCP7940X::clearPowerFail() const
{
    I2C_write(MCP7940_RTCWKDAY, readByte(MCP7940_RTCWKDAY));
    return true;
}

/**
 * Low-level read call
 * ramAddr The address in the RTC RAM to read from
 * data The buffer to read into
 * dataLen The number of bytes to read
 * The dataLen can be larger than the maximum I2C read. Multiple reads will be done if necessary.
 */
bool MCP7940X::readRam(size_t ramAddr, uint8_t *data, size_t dataLen)
{
    bool result = true;

    while(dataLen > 0)
    {
        size_t count = dataLen;
        if (count > 32) {count = 32;}    // Too large for a single I2C operation
            
        uint8_t number = I2C_read(MCP7940_RAM_ADDRESS + ramAddr, data);  
        if (number != count) {result = false; break;}
        ramAddr += count;
        dataLen -= count;
        data += count;
    }

    return result;
}

/**
 * Low-level write call
 * ramAddr The address in the RTC RAM to write to
 * data The buffer containing the data to write
 * dataLen The number of bytes to write
 * The dataLen can be larger than the maximum I2C write. Multiple writes will be done if necessary.
 */
bool MCP7940X::writeRam(size_t ramAddr, const uint8_t *data, size_t dataLen)
{
    bool result = true;

    while(dataLen > 0)
    {
        size_t count = dataLen;
        if (count > 31) {count = 31;}    // Too large for a single I2C operation

        uint8_t number = I2C_write(MCP7940_RAM_ADDRESS + ramAddr, data);  
        if (number != count) {result = false; break;}
        ramAddr += count;
        dataLen -= count;
        data += count;
    }

    return result;
}
