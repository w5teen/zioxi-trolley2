/* 
 * Project zioxi-trolley2
 * Author: Will Steen
 * 
 * Version  Date    	Notes
 * 001	  	28-Aug-24  	Initial version
 * 002		01-Sep-24   Tested publish of Vitals
 * 003		02-Sep-24	Tested use of relay control
 * 004		03-Sep-24	Tested charging, supply detect and battery voltage reading
 * 005		03-Sep-24	Tested use of temperature sensor MCP9800 and added library
 * 006		04-Sep-24	Tested use of AB1805 and added library AB1805_AA
 * 007      04-Sep-24	Current sensor ACS37800 support added
 * 008      04-Sep-24   Added Ethernet adaptor support
 * 009      25-Sep-24   Publish key data every 60 seconds
 * 010      15-Oct-24   Strip down base to explore issues better - evaluation board only supports RTC/Watchdog IC
 * 011      15-Oct-24   Explore with external temperature sensor at address base + 1
 * 012		23-Nov-24   Merged functional for CTROLLEY15 and CTROLLEY2 removing view related code
 * 013      04-Dec-24   Demerge functions into library
 * 014      10-Dec-24   Test ethernet
 * 015      14-Dec-24   Branch focused on stress testing relays
 * 016      20-Jan-25   Add OFF operation to Ethernet_WiFi library call setAutomaticInterface(false) and true to restore
 * 017      20-Jan-25   Add PublishQueuePosixRK to assess usage
 * 018      31-Jan-25   Build a test bed with P2 Evaluation board and AB1805 plus LEDs for relay mimics includes AC Supply detect
 * 019      01-Feb-25   Remove some of the variables and add a variable for each relay on/off state
 * 020      03-Feb-25   Add active and reactive power from ACS37800
 * 021      14-Feb-25   Add schedule data and param and admin commands from existing Trolley code
 * 022      15-Feb-25   Make it build with just Continuous On
 * 023      12-Mar-25   Build for the PDU Rev5 board
 * 024      16-Mar-25   Add Ledger based configuration elements - AB1805 and ACS37800 not detected on Rev5
 * 025      17-Mar-25   Use JSON writer for event composition
 * 026      24-Mar-25   Build and test on Rev2 board
 * 027      03-Apr-25   Build and test on Rev5 board
 * 028      05-Apr-25   Build and test on Rev5 board - added date/time to events
 * 029      05-Apr-25   Build and test on Rev5 board - added state post setup to handle issue with PublishQueuePosixRK from setup
 * 030      06-Apr-25   Build and test on Rev5 board - fixed Z and J values
 * 031      07-Apr-25   Build and test on Rev5 board - remove google location API use and reporting
 * 032      07-Apr-25   Build and test on Rev5 board - add reporting of WiFi signal strength and data quality when WiFi connected
 * 033      08-Apr-25   Build and test on Rev5 board - add delay on restart to assess if this helps with ethernet adaptor detection
 * 034      09-Apr-25   Build and test on Rev5 board - confirmed issue is hardware related, not software - fix detection of ethernet
 * 035      12-Apr-25   Build and test on Rev5 board - remove RELAY_MIMIC and enable EXT_I2C_ENABLE and EXT_TEMP_SENSOR
 * 036      19-Apr-25   Build and test on Rev5 board - add DIAG events and removed Log info, wait for event queue empty before going to sleep, add PowerOnState
 * 037      19-Apr-25   Build and test on Rev5 board - test and refine continuous on and timed on
 * 038      20-Apr-25   Build and test on Rev5 board - remove TROLLEY15 condition and continue continuous on and timed on test
 * 039      21-Apr-25   Build and test on Rev5 board - fix length of BLE name to 20 characters and get BLE address and WiFi/Ethernet MAC
 * 040      22-Apr-25   Build and test on Rev5 board - add support for PWR_RST and correct WDT_FOUT pin
 * 041      24-Apr-25   Build and test on Rev5 board - test and refine auto timed on
 * 042      30-Apr-25   Build and test on Rev5 board - add debug for current sensor to calibrate for Smart charge testing
 * 043      08-May-25   Build and test on Rev5 board - increase to 3dp LA var
 * 044      14-May-25   Build and test on Rev5 board - test smart charge and refine
 * 045      16-May-25   Build and test on Rev5 board - replace AB1805 watchdog with Particle HW watchdog
 * 046      19-May-25   Build and test on Rev5 board - amend ledger names to avoid conflict with sandbox
 * 047      19-May-25   Build and test on Rev5 board - correct Particle Function names to support onView
 * 048      19-May-25   Build and test on Rev5 board - add Hub board support
 * 049      19-May-25   Build and test on Rev5 board - add Particle Function for remote charge control Remote_Charge
 * 050      20-May-25   Build and test on Rev5 board - zioxi-8453, add "dev" to edit MD var
 * 051      21-May-25   Build and test on Rev5 board - add 'HB0' and 'LV0' event vars for Hub
 * 052      22-May-25   Build and test on Rev5 board - zioxi-8458, BLE MAC address needs to be reversed to be correct
 * 053      22-May-25   Build and test on Rev5 board - zioxi-8453, when LVSUN Hub board is present, at startup need to set all LVSUN LEDs to off and also when the power to chargers is removed (AC Relays off)
 * 054      22-May-25   Build and test on Rev5 board - improvement to wandering voltage when no load clamp to 250V, set DRUP frequency when not in standby to every 1 minute
 * 055      22-May-25   Build and test on Rev5 board - improvement to no load current being 0.035A, set to 0.0A
 * 056      22-May-25   Build and test on Rev5 board - zioxi-8457, add R:1 to WCST event to handle aux command when in auto Timed on/off
 * 057      23-May-25   Build and test on Rev5 board - zioxi-8464, add waitfor not isbusy to switch off LED strips
 * 058      23-May-25   Build and test on Rev5 board - build out Remote_Charge function
 * 059      04-Jun-25   Build and test on Rev5 board - reduce Item Name size to 12 add more support for Hub board control
 * 060      07-Jun-25   Build and test on Rev5 board - add configuration of Hub board to the control and comms (ports and channels)
 * 061      08-Jun-25   Build and test on Rev5 board - zioxi-8497, disable BLE whilst sleeping - try ULTRA LOW POWER sleep
 * 062      09-Jun-25   Build and test on Rev5 board - zioxi-8237, implement factory reset including configuration from ledger - remoteAdmin command 'hst'
 * 063      09-Jun-25   Build and test on Rev5 board - add support in ble-wifi-setup-manager library to stop and start advertising and change device name
 * 064      09-Jun-25   Build and test on Rev5 board - convert LVSUN Hub board configuration HB0 var to a JSON array, add battery data and temperature to first DRUP event
 * 065      11-Jun-25   Build and test on Rev5 board - build to test boards 5Mod
 * 066      23-Jun-25   Build and test on Rev5 board - change Hub comms to fix issue
 * 067      04-Jul-25   Build and test on Rev5 board - Build on Device OS 6.3.3 and test if BLE issue solved - it is
 * 068      04-Jul-25   Build and test on Rev5 board - add support for Smart USB C charge and unit test
 * 069      06-Jul-25   Build and test on Rev5 board - add DEUP event after startup with SSID and other network information
 * 070      07-Jul-25   Build and test on Rev5 board - add check for change in network information and publish DEUP event if changed
 * 071      07-Jul-25   Build and test on Rev5 board - zioxi-8580, add BLE connection/disconnection event as DEUP event with var 'BTC'
 * 072      07-Jul-25   Build and test on Rev5 board - zioxi-8453, recovery after sleeping due to power off, DEUP event contains only C 1 and not runstate
 * 073      15-Jul-25   Build and test on Rev5 board - add WiFiChannelRK library to return
 * 074      16-Jul-25   Build and test on Rev5 board - add compile switch to remove AB1805 in preparation for Rev9 board with MCP7940N
 * 075      19-Jul-25   Build and test on Rev9 board - add support for MCP7940N RTC and remove AB1805
 * 076      20-Jul-25   Build and test on Rev9 board - add hub using detection from PRC at startup and dynamically setup, change 'MAC' to 'M'
 * 077      23-Jul-25   Build and test on Rev9 board - strip back to P2-PDU-base to test MCP7940N RTC - determined that board fault with RTC I2C connection
 * 078      29-Jul-25   Build and test on Rev9 board - implement initial power on state logic and values
 * 079      29-Jul-25   Build and test on Rev9 board - add parameter for factoring ACS37800 current sensor output to get calibrated current
 * 080      30-Jul-25   Build and test on Rev9 board - test mains off during always on and power on state
 * 081      31-Jul-25   Build and test on Rev9 board - add hibernate command 'hib' and logic to test whether restart works
 * 082      14-Aug-25   Build and test on Rev9 board - changes to pin mode for wake pin and added LocalTimeRK library
 * 083      16-Aug-25   Build and test on Rev9 board - cut down firmware to test LiPo charging and AC Present, remove REV2 and REV5 board support
 * 084      19-Aug-25   Build and test on Rev9 board - remove LiPo debug, add RTC normal high active low setup
 * 085      24-Aug-25   Build and test on Rev9M board - remove graceful shutdown support and place delay and retest for mains off before hibernation
 * 086      04-Sep-25   Build and test on Rev9M board - correct battery voltage calibration
 * 087      05-Sep-25   Build and test on Rev9M board - zioxi-8682, BLE wifi setup scenario leading to watchdog reset - fixed wifiProvisioning() STATE_PROVISIONED handling
 * 088      07-Sep-25   Build and test on Rev9M board - zioxi-8472, BLE advertised name reduced to 'Z9999' with numbers last 4 digits of serial number
 * 089      07-Sep-25   Build and test on Rev9M board - zioxi-8505, event sent on WiFi/Cloud connection to include C=1
 * 090      07-Sep-25   Build and test on Rev9M board - zioxi-8508, DEUP event on startup contains some zero/missing values
 * 091      07-Sep-25   Build and test on Rev9M board - zioxi-8570, when USBC charging finishes the AC Relay off events need to send 'LV0' with OOOOOOOO
 * 092      12-Sep-25   Build and test on Rev9M board - zioxi-8585, resume from hibernate due to mains restored
 * 093      14-Sep-25   Build and test on Rev9M board - zioxi-8608, 8659, RTC date sometimes shown as 1907-02-24 and time after 3V3 enable button reset, sync time with cloud every 24 hours
 * 094      14-Sep-25   Build and test on Rev9M board - zioxi-8570, at start-up always send LV0 OOOOOO in DEST event when hub board
 * 095      14-Sep-25   Build and test on Rev9M board - ensure network DEUP event also contains BLE MAC address
 * 096      14-Sep-25   Build and test on Rev9M board - add recovery of eeprom data in setup when structure/content change to param (not yet completed)
 * 097      16-Sep-25   Build and test on Rev9M board - rationalise resumeFlag not used, add 1 to "K" in CTSE event, remove "J" from DRUP when charging
 * 098      16-Sep-25   Build and test on Rev9M board - zioxi-8714, factory reset is not setting DST auto true
 * 099      18-Sep-25   Build and test on Rev9M board - zioxi-8662, maximum time on MXO value should only apply to Smart Charge
 * 100      20-Sep-25   Build and test on Rev9M board - zioxi-8720, 8678, BLE advertising being turned off likely due to Watchdog reset
 * 101      21-Sep-25   Build and test on Rev9M board - remove coded BLE advertised name T200000 V090 and revert as item name with max length 7, add status message to BLE when connection occurs
 * 102      22-Sep-25   Build and test on Rev9M board - zioxi-8677, add cloud connection status to 'get_wap' reply
 * 103      22-Sep-25   Build and test on Rev9M board - zioxi-8649, AC smart charge event rate
 * 104      22-Sep-25   Build and test on Rev9M board - zioxi-8200, power on state not working for Timed On (3) and Auto Resume (0) from Smart USBC 
 * 105      22-Sep-25   Build and test on Rev9M board - zioxi-8637, flashing dark blue (listening) LED state not updated after Hibernation restart and credentials loaded
 * 106      23-Sep-25   Build and test on Rev9M board - zioxi-8591, Smart USB-C charge completion add delay to recheck status before turning off relays
 * 107      24-Sep-25   Build and test on Rev9M board - zioxi-8593, Smart USB-C charge send DRUP event 40 seconds after start
 * 108      25-Sep-25   Build and test on Rev9M board - zioxi-8681, standby power off and on within 1 second, revert DRUP frequency to 1 minute for testing
 * 109      25-Sep-25   Build and test on Rev9M board - zioxi-8593, requirement expanded to include all charge modes
 * 110      27-Sep-25   Build and test on Rev9M board - zioxi-8613, auto not retained through restart, introduce compile switch to test idea of removing
 * 111      27-Sep-25   Build and test on Rev9M board - zioxi-8495, smart USBC schedule when start period does not start smart USBC charge
 * 112      29-Sep-25   Build and test on Rev9M board - zioxi-8736, tip and mxo value range fixed
 * 113      29-Sep-25   Build and test on Rev9M board - zioxi-8664, send LV when LA sent except if LA = 0
 * 114      29-Sep-25   Build and test on Rev9M board - zioxi-8672, following factory reset DEST BLE is zeros
 * 115      30-Sep-25   Build and test on Rev9M board - zioxi-8735, consecutive 30 minute period start auto smart charge and auto smart USBC charge
 * 116      01-Oct-25   Build and test on Rev9M board - zioxi-8593, initial DRUP event to be 40 seconds after CTOS and CTSS events not web command
 * 117      05-Oct-25   Build and test on Rev9M board - Device OS 6.3.4 
 * 118      05-Oct-25   Build and test on Rev9M board - revised Smart AC charge ledger items and param struct change
 * 119      09-Oct-25   Build and test on Rev9M board - revised Smart AC charge logic per design document
 * 120      10-Oct-25   Build and test on Rev9M board - baselined onto project zioxi-trolley2 with github repository, removed AB1805 support and library and other compile flags
 * 121      11-Oct-25   Build and test on Rev9M board - zioxi-8765, CTUE event LA = 0 and LV = measured
 * 122      12-Oct-25   Build and test on Rev9M board - zioxi-8753, remove "MD" from events
 * 123      13-Oct-25   Build and test on Rev9M board - zioxi-8770, fix for POS Smart AC charge - returns to Smart USB-C charge when mains off when in standby
 * 124      13-Oct-25   Build and test on Rev9M board - zioxi-8768, fix for POS auto resume not working for Smart AC charge
 * 125      13-Oct-25   Build and test on Rev12 board - add compile switch to add support for Rev12 board
 * 126      13-Oct-25   Build and test on Rev12 board - add constant soft start delay SSDELAY rather than parameter
 * 127      13-Oct-25   Build and test on Rev12 board - zioxi-8781, BLE commands to get operation mode and set operation mode to Local or Connected
 * 128      14-Oct-25   Build and test on Rev12 board - zioxi-8781, add parameter to hold local or connected mode so the device will not forget on restart/restore
 * 129      14-Oct-25   Build and test on Rev12 board - initial batch with serial connected wait turned off
 * 130      15-Oct-25   Build and test on Rev12 board - zioxi-8766, revised events and variable for smart charging status
 * 131      16-Oct-25   Build and test on Rev12 board - zioxi-8794, suppress DRUP event between web command and ACRelaysOn event and ensure first DRUP 40 seconds after ACRelaysOn
 */ 

// P2-PDU-base *************************************
#define EXT_TEMP_SENSOR false
#define GOOGLE_LOCATE false
#define CMD_BUFFER false
#define LVSUNCHARGER true
#define LOCAL_TIME_RK false                 //V082
#define SERIAL_WAIT false                   //V083
#define RESET_AUTO_SMART_MONITORING false   //V110
#define REV12_BOARD true                    //V125

#include "Particle.h"

#include "MCP9800.h"

#include "MCP7940.h"

#include "ACS37800.h"

#include "EthernetWiFi.h"

#include "ble_wifi_setup_manager.h"

#include "WiFiChannelRK.h"

#if GOOGLE_LOCATE
#include "device-locator.h"
#endif  //GOOGLE_LOCATE

#if LOCAL_TIME_RK
#include "LocalTimeRK.h"
#endif      //LOCAL_TIME_RK

SYSTEM_MODE(SEMI_AUTOMATIC);            //let firmware manage the connection to the Particle Cloud

const char* const firmware         =   "6.3.4";
const char* const softwarebuild    =   "131 16-10-25";
const char* const hardwarebuild    =   "Rev12";

PRODUCT_VERSION(131);

// instantiations
MCP9800 tmpSensor;                      //always instantiate onboard MCP9800 sensor

#if EXT_TEMP_SENSOR
MCP9800 extSensor1;                      //instantiate external MCP9800 sensor 1
MCP9800 extSensor2;                      //instantiate external MCP9800 sensor 2
MCP9800 extSensor3;                      //instantiate external MCP9800 sensor 3
MCP9800 extSensor4;                      //instantiate external MCP9800 sensor 4
#endif

#include "PublishQueuePosixRK.h"

MCP7940X MCP7940;

#define TIMEDATA_ADDR 0                 // Address in RTC SRAM for time data
typedef struct {
    float       timezone;
    float       DSToffset;
    int         DST;
} timeData;

timeData timedata;
uint8_t serializedTData[sizeof(timeData)];      // Serialize timedata into a uint8_t array

#define RESTARTDATA_ADDR TIMEDATA_ADDR + sizeof(timeData)   // Address in RTC SRAM for restart data
typedef struct {
    int resumeReason;
    int resumeState;
    int powerOnState;
    uint8_t relayState[4];
    bool isHubBoard;    // true if hub board present, false if not V076
} restartData;

restartData restartdata;
uint8_t serializedRData[sizeof(restartData)];   // Serialize restartdata into a uint8_t array

#define RESTART_NORMAL 0
#define RESUME_OUT_OF_MEMORY 1
#define RESUME_WATCHDOG 2
#define RESUME_MAINS_OFF 3
#define RESUME_OVERHEAT 4
#define RESTART_FIRMWARE_UPDATE 5
#define RESTART_DEEP_POWER_DOWN 98
#define RESTART_COUNTDOWN_TIMER 97
#define RESTART_ALARM 96
#define RESTART_UNKNOWN 95
#define RESTART_SLEEP 94
#define RESTART_HIBERNATE 93
#define RESTART_FAULT 90
#define RESTART_HARD_RESET 88
#define RESTART_WEBCMD 10
#define RESTART_MAINS_ON 9

ACS37800 mySensor;                  // create an object of the ACS37800 class
const float AMPS_OFFSET = 0.0;      // offset for ACS37800 current sensor to correct for internal load
const float AMPS_GAIN = 0.9456;     // gain for ACS37800 current sensor to calibrate
const float AMPS_DEADBAND = 0.0;    // deadband for ACS37800 current sensor to calibrate
const float VOLTS_CLAMP = 250.0;    // clamp for ACS37800 voltage sensor to correct for no load wander V053

typedef struct {
    float voltsrms;
    float ampsrms;
    bool isACsupply;
    float apowerwatt;
    float rpowerwatt;
} powerData;

powerData powerdata;

typedef struct {
    float batvolts;
    bool isCharging;
    bool isCharged;
    bool isFault;
    bool isFaultRecoverable;
} batteryData;

batteryData batterydata;

#if LVSUNCHARGER
typedef struct {
    bool isInputs;
    bool isOutputs;
    int8_t channelsIn;
    int8_t channelsLED;
    int8_t portsIn;
    int8_t portsLED;
    int8_t doorSensors;
    int8_t hardwareRevision;
    int16_t firmwareRevision;
} hubData;

hubData hubdata;                                // hubdata structure to hold hub information assume just 1 hub V060
#endif  // LVSUNCHARGER

// Pin definitions
#if REV12_BOARD
const pin_t P2_PDU_RELAYS[] = {D19, D18, D21, D6};
#else   // REV09M_BOARD
const pin_t P2_PDU_RELAYS[] = {D19, D18, D7, D6};
#endif  // REV12_BOARD

const pin_t P2_PDU_SUPPLY_DETECT = A5;
const pin_t P2_PDU_WAKE = WKP;

const pin_t P2_PDU_5V_I2C_EN = D20;
const pin_t P2_PDU_3V3_SW_EN = D12;
const pin_t P2_PDU_BATT_CHARGING1 = D13;
const pin_t P2_PDU_BATT_CHARGING2 = D11;
const pin_t P2_PDU_VBAT_MEAS = A6;
const pin_t P2_PDU_RTC_MFP = D2;

const pin_t P2_PDU_ETH_SPI_CS = D5;
const pin_t P2_PDU_ETH_SPI_INT = D4;
const pin_t P2_PDU_ETH_SPI_RST = D3;

//Constants
const int TEMPOFFSET = 3; // offset to correct temperature readings from board to ambient (subtracted)
const size_t counterRamAddr = 0;
const uint32_t COUNTER_MAGIC = 0x3939671e;
const size_t TIMEZONE_ADDR = 0;
const size_t DST_OFFSET_ADDR = 4;
const size_t DST_ADDR = 8;

//Global variables
double battv = 0.0;             //Particle variable for battery voltage
double bTemp = 0.0;             //Particle variable for internal temperature
double xTemps = 0.0;

char dataStr[500];

/*
SerialLogHandler logHandler(LOG_LEVEL_INFO,
{
	{ "app.pubq", LOG_LEVEL_TRACE },
	{ "app.seqfile", LOG_LEVEL_TRACE }
});
*/

#define PARAM_ADDR 0            // start address in eeprom address space

// device run state values
#define D_STARTUP               0
#define D_STANDBY               1
#define D_GOTOSTANDBY           21
#define D_GOTOTIMED_ON          23
#define D_TIMED_ON              3
#define D_AUTO_OFF              4
#define D_GOTOAUTO              25
#define D_AUTO_ON               5
#define D_AUTOEDIT              55
#define D_GOTOALWAYS_ON         26
#define D_ALWAYS_ON             6
#define D_GOTOSLEEP             27
#define D_SLEEPING              7
#define D_GOTOCHARGED_ON        28
#define D_CHARGED_ON            8
#define D_GOTOCHARGED_ON_AUTO   178
#define D_CHARGED_ON_AUTO       78
#define D_GOTOCHARGED_ON_USBC   29
#define D_GOTOCHARGED_ON_USBC_AUTO 179
#define D_CHARGED_ON_USBC       9
#define D_CHARGED_ON_USBC_AUTO  79
#define D_RESTART               88
#define D_WEBOUCAEXIT           90
#define D_WEBGOTORESTART        91
#define D_WEBGOTOHARDRESTART    92
#define D_WEBGOTOSTANDBY        93
#define D_WEBGOTOHIBERNATE      94
#define D_WEBGOTOAUTO           95
#define D_WEBGOTOCHARGED        96
#define D_WEBGOTOCHARGEDUSBC    196
#define D_WEBGOTOON             97
#define D_WEBGOTOTIMED          98
#define D_WEBOUCAUTO            99

#define HARD_RESET_CMD          28

enum PowerOnState {
    POS_LAST = 0,    // Auto - resume from last state
    POS_STANDBY = 1, // Standby - resume in standby state
    POS_TIMED_ON = 3, // Timed On - resume in timed on state
    POS_CONT_ON = 6, // Continuous - resume in continuous on state
    POS_SMART_AC = 7, // Smart AC - resume in smart AC state
    POS_SMART_USB = 8 // Smart USB - resume in smart USB state
};

// charge state values (KL)
enum ChargeState {
    C_NOT_CHARGING = 0,  //not charging
    C_CHARGING = 1,      //charging or warm-up charging
    C_CHARGING_FULL = 2, //full rate charging-warm up ended V130
    C_RATE_CHARGING = 7, //rate monitoring
    //C_RATE_CHARGING_PLUS = 8, //confirmed rate monitoring V130 removed
    C_CHARGING_DONE = 9, //rate monitoring slope < target or time expired
    C_CHARGING_ENDED = 10 //charging stopped after extra time
};
// onView web app RunState values
#define W_STANDBY               1 
#define W_TIMED_ON              3 
#define W_AUTO_OFF              4 
#define W_AUTO_ON               5
#define W_ON                    6
#define W_SLEEPING              7 
#define W_CHARGED_ON            8 
#define W_CHARGED_ON_AUTO       9
#define W_CHARGED_ON_USBC       15
#define W_CHARGED_ON_USBC_AUTO  16

// onView web app PowerState values
#define W_MAINS_OFF             100
#define W_MAINS_ON              101
#define W_CHARGING              105
#define W_RESUME                200

// Smart charging state values
#define ON_UNTIL_OFF            0           //Monitoring off
#define ON_UNTIL_AUTO           1           //Auto Smart start monitoring
#define ON_UNTIL_USBC           3           //Smart USB-C charging state (until all ports are green or off)
#define ON_UNTIL_SS             10          //initial soft start smart charging
#define ON_UNTIL_WARM           20          //warm-up smart charging state
#define ON_UNTIL_SENSE          30          //sense OUCState and 31 to 34 if 4 relays
#define ON_UNTIL_SS2            40          //post sense soft start OUCState
#define ON_UNTIL_WARM2          41          //pre charging with monitoring warm up
#define ON_UNTIL_CHARGE         50          //charging with monitor OUCState
#define ON_UNTIL_END            60          //finish OUCState

#define AUTO_ON_UNTIL_OFF       false       //On Until Charged auto start not engaged
#define AUTO_ON_UNTIL_ON        true        //On Until Charged auto start monitoring

// Constant char arrary definitions for event names
const char* const eventonstart    =    "CTOS";
const char* const eventonended    =    "CTOE"; 
const char* const eventtimerstart =    "CTTS"; 
const char* const eventtimerended =    "CTTE"; 
const char* const eventontilstart =    "CTUS"; 
const char* const eventontilended =    "CTUE"; 
const char* const eventscheduleon =    "CTSS"; 
const char* const eventscheduloff =    "CTSE"; 
const char* const eventschedulexp =    "CTSX"; 
const char* const eventunlocked   =    "CTUN";
const char* const eventlocked     =    "CTLK";
const char* const eventoverheated =    "CTOV";
const char* const eventmainsoff   =    "CTMX";
const char* const eventmainsresum =    "CTMR";

const char* const eventtimedweb   =    "WCOT"; 
const char* const eventautoweb    =    "WCAU"; 
const char* const eventchargeweb  =    "WCOU";
const char* const eventoucautoweb =    "WCAO";
const char* const eventoucstopweb =    "WCAS";
const char* const eventonweb      =    "WCON";
const char* const eventstandbyweb =    "WCST"; 
const char* const eventrestartweb =    "WCRE";

const char* const eventsleep      =    "CTSL";
const char* const eventvarchanged =    "DEUP";
const char* const eventregularupd =    "DRUP";
const char* const eventstartupdat =    "DEST";
const char* const eventresetparam =    "DERE";
const char* const eventstartupres =    "DERS"; 
const char* const eventdiagnostic =    "DIAG";
const char* const eventwifiupdate =    "DEUP";

#define SERLEN 7                            //Serial number length (amended to 7 characters V120)
#define PRODLEN 30                          //Product Type, Name, Code length
#define MAXLEN 7                            //Item name length reduced to 7 to see if works better with BLE advertising V059/V090
#define ARRAYSIZE 42                        //schedule byte array size
#define MAXTEMPC 45                         //maximum temperature oC to trigger over temperature warning default value
#define DEFAULTIMER 180                     //timerperiod default value 
#define DEFAULTMAXON 300                    //maximum On time default value

#define SSDELAY 2                           //sequenced start delay - 2 seconds
#define CHARGEONOFF  'C'                    //schedule type is 'Charging On/Off'
#define OUC_START    'O'                    //schedule type is 'On Until Charged-Start'
#define NONESCHEDULE 'N'                    //no schedule loaded

#define LAST_CONFIG 20000UL                 //last configuration check time in milliseconds

#define MAXDATA 500                         //maximum event data string length
#define RESET_TO 2000                       //timeout when waiting for events in backlog to clear before system.reset
#define ONE_MINUTE 60000                    //one minute in milliseconds
#define ONE_DAY_MILLIS 86400000             //one day in milliseconds

#define GOTOSLEEPDELAY 120000               //period in milliseconds between mains off during charging and going to sleep to save power
#define PSCHK 2000                          //frequency of checking update on powerstate (mains on/off and charging) every 2s
#define SCHK 2000                           //frequency of checking update on sensors door locks/temperature every 2s
#define CCCHK 20000                         //frequency of checking update on charging current every 20 seconds
#define CSCHK 20000                         //charge current rate of change monitoring every 20 seconds (must be the same as CCCHK)
#define DONECHK 15000                       //When OuC is completed then hold display of completed screen for 15 seconds
#define UCHK 10000                          //frequency of usage report update
#define MCHK 90000                          //90 seconds of inactivity triggers watchdog reset with resume
#define DCHK 60000                          //5 minutes for device regular updates when charging V107 revert for testing V108
#define SDCHK 900000                        //Slow DCHK to every 15 minutes when in standby
#define SAMPLE_INTERVAL 60000               //60 seconds in milliseconds for average current calculation

// context values for ACRelaysOn and ACRelaysOff
#define R_NONE  88                          //relay no message
#define R_AUTO  10                          //auto - start/ended under schedule control
#define X_AUTO  12                          //suspend auto - mains power removed or overheated/resume start
#define T_AUTO  13                          //auto - ended no valid time
#define W_AUTO  15                          //stopped auto charging - web command
#define R_TIMED 20                          //timed on relay message/timed end
#define X_TIMED 22                          //exit timed on because mains power removed/resume start
#define Z_TIMED 23                          //stop timed on because scheduled start of on until charged
#define W_TIMED 24                          //stop timed on by web command
#define R_ONC   30                          //continuous charge - start
#define X_ONC   32                          //suspend continuous charge - mains power removed/resume start
#define Z_ONC   33                          //stop continuous charge - scheduled start of auto on until charged
#define M_ONC   34                          //stop continuous charge - maximum charge time reached
#define W_ONC   35                          //stop continuous charge by web command
#define R_ONTIL 40                          //on til charged relay message - normal soft start
#define X_ONTIL 42                          //suspend on until charged - mains power removed/resume start
#define M_ONTIL 44                          //on til charged relay stop - maximum charge time reached
#define W_ONTIL 45                          //auto OuC exit by web command
#define Z_ONTIL 46                          //stop OuC charge - scheduled start of auto on until charged
#define R_USBC  50                          //USBC charge relay message - normal soft start
#define X_USBC  52                          //suspend USBC charge - mains power removed/resume start
#define M_USBC  54                          //USBC charge relay stop - maximum charge time reached
#define W_USBC  55                          //stop USBC charge by web command
#define Z_USBC  56                          //stop USBC charge - scheduled start of auto on until charged

#define AUTOANDOFF 99                       //special prevrunstate value to signal auto off message needs to be sent

#define NEXT_ON 0x01                        //schedule on value
#define NEXT_OFF 0x0                        //schedule off value

#if CMD_BUFFER
#define BUFFER_SIZE  16                     //maximum size is 127 with index as uint8_t and one item spare is required - for web commands more than sub devices should be sufficient 
struct Buffer {
    uint32_t data[BUFFER_SIZE];
    uint8_t newest_index;
    uint8_t oldest_index;
};

enum BufferStatus {BUFFER_FULL = -1, BUFFER_OK = 0, BUFFER_EMPTY = 1 };         //typedef BufferStatus
enum BufferStatus status = BUFFER_OK;                                           //initialise status
enum BufferStatus bufferWrite(volatile struct Buffer *buffer, uint32_t item);
enum BufferStatus bufferRead(volatile struct Buffer *buffer, uint32_t *item);
enum BufferStatus bufferPeek(volatile struct Buffer *buffer, uint32_t *item);

volatile struct Buffer rs_buffer = {{}, 0, 0};                                  //create an instance of the Buffer called rs_buffer
uint8_t webDesk;
uint8_t webSocket;
#endif  //CMD_BUFFER

int remoteAdmin(const char * command);
int remoteParam(const char * command);
int remoteCharge(const char * command);     //V048
void onViewRunState();

struct deviceSettings
{
    int     magicNumber = 0x99DEA128;   //must be changed if struct changes or default values set to show upgrade of struct
    byte    resumeFlag = 0;             //=28 if set before hard reset or 0 if not or 5 firmware flashed V097
    int     timerPeriod = DEFAULTIMER;  //timer period in minutes (to start)
    bool    isDst = false;              //daylight saving time
    bool    isAutoDst = true;           //automatically set/reset daylight saving
    double  timeZone = 0.0;             //time zone +/- from UTC
    char    serialNum[SERLEN+1];        //serial number - SN
    char    productCode[PRODLEN];       //product code - PRC
    char    itemName[MAXLEN+1];         //item (given) name - NAM
    uint8_t macA[6];                    //mac address for wifi device
    uint8_t ethA[6];                    //mac address for ethernet adaptor
    uint8_t bleA[6];                    //mac address for bluetooth
    uint8_t schedule[ARRAYSIZE];        //schedule control data
    char    scheduleType = NONESCHEDULE;//defines whether 'OuC-Start' = 'O' or 'Charge On/Off' = 'C'  or 'None' = 'N'
    int     valid2day = 1;              //day of month
    int     valid2month = 1;            //month
    int     valid2year = 2024;          //year
    float   minCurrent;                 //level of charging current per device below which rate of charge monitoring used - MNC
    int     resumeState = 0;            //watchdog resume state after reset
    int     resumeMinutes = -1;         //watchdog timing resume minutes
    int     resumeTrace = 0;            //added in case using resumeMinutes compromising restart after being in timed, etc.
    int     resumeCause = 0;            //added to ensure the cause of the resume condition is known to apply the right recovery
    int     maxTimeOn = DEFAULTMAXON;   //maximum time allowed for On - MXO
    int     numSpaces[4] = {0};         //number of bays or charging spaces - NUM V118
    int     numDevices = 0;             //total number of devices in trolley/cabinet sum of DPO[] V118
    int     doorSensors = 0;            //number of door sensors - DRS
    int     tempSensors = 0;            //number of temperature sensors (default onboard only) - TMP
    int     extraChargingTime = 1;      //extra charging time after charging done - EXT V118
    int     maxTemp = MAXTEMPC;         //maximum temperature before warning in oC - MXT
    bool    isAuto = false;             //to ensure auto is re-engaged after a restart or charging cycle  this can be Auto On/Off or Auto On_Until_Charged 
    int     devicesPerOutlet[4] = {0};  //entered devices per outlet - DPO V118    
    bool    isOUCMonitoring = false;    //OUC monitoring
    float   minChargeRate;              //minimum charge rate - MNR
    int     warmupMins = 2;             //warmup period in minutes (default = 2)
    int     maxMonitoringTime = 10;     //maximum monitoring time in minutes - MMT V118
    int     powerOnState = POS_CONT_ON; //power on state - default continuous on - POS
    uint8_t hubBoard = 0;               //hub board present - 'UC' in product code V076
    float   powerMeterFactor = 0.9456;  //power meter factor to multiply raw output from ACS37800 in amps to get actual current in amps
    bool    isLocalMode = false;        //local mode - not connected to cloud default is connected V128
    uint8_t padding[3] = {0};           //fill to 4 byte multiple boundary
    int     checkSum  = 0;              //checksum value is always the last item
};

deviceSettings param;                   //create an instance of the deviceSettings struct called param

typedef struct {
    int      startCondition = 0;
    int      chargeMode = 0;
    int      endCondition = 0;
    int      maxTime = 0;
    uint16_t overrideConditions = 0;
    uint8_t  outletsUnderControl = 0;
    uint8_t  inUseFlag = 0;
} chargecommand;

chargecommand command[4];               //create an array of the chargecommand struct called command, one for each outlet

void setupMCP7940();
volatile bool mfpPinTriggered = false;  // Flag to indicate if MFP pin was triggered
const uint8_t  MCP7940_MEMORY_SIZE = 64;        //Number of bytes in memory
void rtcMFP();  // Forward declaration of MFP pin interrupt handler
void checkAlarm();  // Function to check if an alarm has been triggered

void checkForConfiguration();
void restoreParameters();
bool performConfiguration();
int calcCheckSum(void *data, size_t len);
int calcCheckSumParameters();
bool checkSumParameters();
void putParameters();
void getParameters();
bool getValue(const char *key, Variant &value);
void setConfigurationDone(int ver);
int getConfigurationStatus();
void configurationAvailableCheck();
void restoreConfigurationItems();
int powerOnStateSelection();

int timerMins = 0;                      //for timed on
int chargeMins = 0;                     //for on until charged

void timing_countdown(void);
void resetTimeout(void);
void charge_timer(void);
void initTimers(void);

Timer timerCountdown(ONE_MINUTE, timing_countdown);                 //auto trigger - countdown (timed on)
Timer timerReset(RESET_TO, resetTimeout, true);                     //one time timer for reset wait for all events sent timeout
Timer timerCharging(ONE_MINUTE, charge_timer);                      //charge on timer (on and on until charged)
int timerStartMins;                                                 //to store the start duration of the timer for the countdown typically param.timerPeriod
volatile bool isUpdateCountdown = false;
volatile bool isResetTimeout = false;
volatile bool istimerCountdown_ended = false;
bool wasSmartChargeEndedThisHalfHour = false; 
bool isSmartScheduleChanged = false; 
volatile bool isUpdateTimer = false;
uint32_t lastLocalTime = 0;
volatile bool isScheduleChanged = false;
bool isSleepWake = false;                                           //V072
bool isChargingJustStarted = false;                                     //V107
bool isChargingStarted = false;                                     //V108

String SerialNum = "T225000";
String ItemName = "No Name";                                        //V059 - max length 7
String product_code = "Error No Product Code Set       ";

bool isConfigurationAvailable = false;

int nextDay = 0;
unsigned long mainsofftime; 
bool isChargingFinished = false;
bool isTriedAutoOUConce = false;
bool isDoorLocked = false;
bool wasDoorLocked = false;
bool isAutoTimeInValidSent = false;
bool isOverheated = false;
bool wasOverheated = false;
float averageCurrent = 0.0;
float sense0 = 0.0;
float sense1 = 0.0;
float sense2 = 0.0;
float sense3 = 0.0;
int iStep;
float cSlope;

float ampsY[10], timeX[10];
int ptrNow, dataPoints;

volatile int runState;
int prevRunState;
int runStateInt;
int connectedStatus = 0;
int oucState;
int knownFRCDevices[4];
int totalAdaptors;
int powerState;
int powerStateInt;
int prevPowerState;
int restartReason;
int chargeState;            // charging status (0 = not charging, 1 - 9 = charging, 10 = charged)
int chargingDevices;        // number of devices charging

void initTimers();
void stoptimerCountdown();
void starttimerCountdown();
void stoptimerCharging();
void starttimerCharging();

void ACRelaysOn(int context);
void ACRelaysOff(int context);
void startupController();
void restartController();
void goToStandbyController();
void standbyController();
void goToTimedOnController();
void timedOnController();
void goToOnController();
void onController();
void goToAutoController();
void autoController();
void goToChargedOnController();
void chargedOnController();
void helperSendFullRateChargingEvent();     //V130
void helperSendRateMonitoringChargingEvent(); //V130
void helperSendChargingDoneEvent();         //V130
void goToChargedOnAutoController();
void goToChargedOnUSBCController();
void goToChargedOnUSBCAutoController();
void chargedOnUSBCController();
bool sampleHubPorts();
void helperGoToUSBCCharged(int context);
void checkScheduledStartUntilCharged();     //V111

void webGoToTimedController();
void webGoToAutoController();
void webGoToChargedController();
void webGoToOnController();
void webGoToOUCAutoController();
void webGoToRestartController();
void webGoToHardRestartController();
void webGoToStandbyController();
void webOUCAExitController();
void webGoToExitController();
void webGoToFactoryResetController();
void webGoToHibernateController();
void goToSleepController();
void sleepingController();
int powerStateCheck();
void helperAutoMainsOffOverheated();
void helperCleanUpResumeData();
void helperOnMainsOffOverheated();
void helperTimedMainsOffOverheated();
void putParameters();
void previousStateHandler(int prevRunState);
void helperAutoSetup();
byte decodeBase64(char b64chr);
void sampleCurrent();
void sampleBMS();
int numberOfAdaptors();
void mainsPowerOffEvent();
void mainsPowerRestoredEvent();
void helperAfterSleep();
void checkForConfiguration();
void checkDeviceUpdate();
void networkInfoEvent();
bool hasScheduleExpired();
void helperAutoOverheated();
void helperAutoMainsOff();
void helperGoToCharged(int i);
void helperSmartMainsOffOverheated(int context);
int determineChargeState(int cs, float slope, float amps, int cdevices, int minutes);
float linearRegression(int n, float *x, float *y);
bool startChargeRateMonitor(float amps, int cdevices, int cs);
bool checkIfChangedPeriod(int i, int j);
int chargingDevicesLogic(float amps);
void checkOverheated();
void waitForQueueToEmpty();
void helperDelayDRUP();       //V116
void helperCheckFirstDRUP();  //V116
void checkForWiFiHealth();
void initialDESTevent();
char* getCreatedTime();

Ledger cloudconfigurationtodevice;  // Cloud to Device, device-specific configuration
Ledger configurationstatus;         // Device to Cloud configuration status

void syncCallback(Ledger ledger);   // Call back when ledger sync'd

int outOfMemory = -1;

float xtemp = 0.0;

timer_t eventdelay;
timer_t checknetwork;
timer_t connectedSince;
timer_t connectTime;
timer_t lastLoop;
timer_t lastRelayOn = 0;
timer_t lastRelayOff = 0;
timer_t currentupdate;
timer_t hubupdate;
timer_t charginginitialupdate;      //V107

int loopRate;

const timer_t DELAY_INITIAL_DRUP = 40000UL; // 40 seconds V107
const timer_t HUB_UPDATE_INTERVAL = 20000UL; // 20 seconds V106
const timer_t HUB_COMPLETION_DELAY = 300000UL; // 5 minutes V106
const timer_t NETWORK_CHECK_INTERVAL = 10000UL;

timer_t ethernetConnectedCount;

bool wasEthernetConnected;
bool wasWiFiConnected;
bool wasCloudConnected;
bool isWiFiConnected;
bool isEthernetConnected;
bool wasNotCalledConnect = true;

//Forward declarations
void setupTemperatureSensor();
float temperatureFromSensor();
float smoothTemperature(int value);

float boardTemp;
uint8_t fault[8] = {8,8,8,8,8,0,0,8}; // 0 = no fault, 1 = bus busy, 2 = start bit error, 3 = address timeout, 4 = data timeout, 5 = end timeout, 6 = stop bit timeout, 7 = I2C not enabled, 8 = not installed

#if EXT_TEMP_SENSOR
float smoothXTemperature(int value);
void readExternalTempSensor();
void setupExternalTempSensor();
#endif  //EXT_TEMP_SENSOR

void saveTimeSettingsToRam();
void restoreTimeSettingsFromRam();
bool isDSTactive();
void clockTimeUpdate(bool forced);
void checkTimeSync();
#define CLOCKUPDATE 20000UL

void saveRestartDataToRam();
void clearRestartData();
void restoreRestartDataFromRam();


SerialLogHandler logHandler(LOG_LEVEL_WARN, { // Logging level for non-application messages
    { "app", LOG_LEVEL_INFO }, // Default logging level for all application messages
    { "app.mcp7940N_RTC", LOG_LEVEL_TRACE }, // Logging level for MCP7940N RTC messages
    { "app.BLEWiFiSetupManager", LOG_LEVEL_TRACE } // Logging level for networking messages
});

void outOfMemoryHandler(system_event_t event, int param);
void handle_restart_events(system_event_t event, int data);

void setupPins();

void setupACS37800();

bool isACSupplyPresent();

void setupNetwork();
void checkNetworks();
uint8_t macAddress[6] = {0};        //WiFi MAC address
uint8_t ethAddress[6] = {0};        //Ethernet MAC address

void sensorReading();

typedef enum {
    STATE_IDLE = 0,
    STATE_PROVISIONED,
    STATE_CREDENTIALS
} ProvisionStates_t;

ProvisionStates_t provision_state, next_provision_state;

void cloudConnected();
void wifiProvisioning();
void sendBLEProvisioningEvent(bool isConnected);
void provisionCb()                  //Callback function for BLE WiFi Setup
{
    provision_state = STATE_PROVISIONED;
}

void updateCallback();
int wifiChannel = 0;
    
#if GOOGLE_LOCATE
void checkLocation();
void locationCallback(float lat, float lon, float accuracy);
float latitude, longitude, locateAcc;
GoogleMapsDeviceLocator locator;
#endif  //GOOGLE_LOCATE

#if LVSUNCHARGER
const byte I2C_ADDRESS = 0x10;      // I2C address of the Hub slave device
uint32_t hubBoardVersion();
bool isHUBbusy(uint8_t address);
byte sendMessage(uint8_t address, byte cmd, byte channel, const char *message);
String receiveMessage(uint8_t address, uint8_t _channel);
void i2cSetup();
void LVSUNLEDsOff();                // V053
void LVSUNInputChannelsOnOff(uint8_t address, uint8_t _channel, bool _on); // V059
float HubTemperature(uint8_t address); // V059
#endif //LVSUNCHARGER


volatile bool isResetPending = false;
volatile bool isOKtoSleep = true;
int trace = 0;

void setup()
{
    System.on(out_of_memory, outOfMemoryHandler);   // If system runs out of memory a System.reset() is done.
    System.on(reset + reset_pending + firmware_update, handle_restart_events); // register event handler

	setupPins();

    #if SERIAL_WAIT
    waitFor(Serial.isConnected, 10000);
    Log.info("smartPDU-test starting");    // Log message
    #endif      //SERIAL_WAIT


    // This allows a graceful shutdown on System.reset()
    Particle.setDisconnectOptions(CloudDisconnectOptions().graceful(true).timeout(3000));

	PublishQueuePosix::instance().setup();
    PublishQueuePosix::instance().withRamQueueSize(0);

    // PublishQueuePosix::instance().clearQueues(); // may want to call this is hard reset

    setupMCP7940();

    Watchdog.init(WatchdogConfiguration().timeout(90s));    // V100 time increased to be more than 2x CONNECTION_TIMEOUT in ble_wifi_setup_manager  
    Watchdog.start();                               // start the watchdog timer

    restoreRestartDataFromRam();                    // restore restart data from RTC RAM
    Log.info("Resume Reason: %i Resume runstate: %i PowerOn State: %i Relay1234: %1i%1i%1i%1i HubBoard %c", restartdata.resumeReason, restartdata.resumeState, restartdata.powerOnState, restartdata.relayState[0], restartdata.relayState[1], restartdata.relayState[2], restartdata.relayState[3], restartdata.isHubBoard ? 'Y' : 'N'); //V076

    #if LVSUNCHARGER
    if (restartdata.isHubBoard) i2cSetup();         // setup I2C bus V076
    #endif  //LVSUNCHARGER

    // Start ledger synchronization
    cloudconfigurationtodevice = Particle.ledger("zcloudconfigtodevice");   //V046
    cloudconfigurationtodevice.onSync(syncCallback);
    configurationstatus = Particle.ledger("zdeviceconfig"); //V046

    checkForConfiguration();

    restoreParameters();                            // need to read back params from eeprom before use!

    Log.info("Params TZ: %f isAutoDst: %c POS: %i Update: %i", param.timeZone, param.isAutoDst?'T':'F', param.powerOnState, param.resumeFlag);

    restoreTimeSettingsFromRam();                   // restore time settings from RTC RAM (TimeZone, DST Offset, isDST)

    Particle.function("Remote_Admin", remoteAdmin); //V047
    Particle.function("Remote_Param", remoteParam); //V047
    Particle.function("Remote_Charge", remoteCharge); //V048

    Particle.variable("SN", SerialNum);
    Particle.variable("ITN", ItemName);
    Particle.variable("PRC", product_code);

    setupTemperatureSensor();

    #if EXT_TEMP_SENSOR
    setupExternalTempSensor();                      // needs to be after parameters restored
    #endif

    setupACS37800();                                // setup ACS37800 current sensor
 
    Particle.variable("ACPWR", powerdata.isACsupply);

    Particle.variable("CHRGG", batterydata.isCharging);
    Particle.variable("FAULT", batterydata.isFault);
    Particle.variable("REFLT", batterydata.isFaultRecoverable);
    Particle.variable("CHRGD", batterydata.isCharged);
    Particle.variable("BATTV", battv);
    Particle.variable("BDTMP", bTemp);
    
    #if EXT_TEMP_SENSOR
    Particle.variable("XTMP", xTemps);
    #endif

    Particle.variable("LOOPC", loopRate);

    setupNetwork();                                 // setup the Etherwifi controller V033

    sensorReading();                                // read the current sensor

    powerdata.isACsupply = isACSupplyPresent();     // check if AC supply is present

	provision_state = next_provision_state = STATE_IDLE;
    
    BLEWiFiSetupManager::instance().setup((const char*) param.itemName); //V101
    BLEWiFiSetupManager::instance().setProvisionCallback(provisionCb);

    if (WiFi.hasCredentials() && !(WiFi.ready()))    provision_state = next_provision_state = STATE_CREDENTIALS;

    WiFiChannelRK::instance().withUpdateCallback(updateCallback).setup();

    #if GOOGLE_LOCATE
    locator.withSubscribe(locationCallback);
    locator.withLocateOnce();
    #endif  //GOOGLE_LOCATE

    #if LVSUNCHARGER
    if (restartdata.isHubBoard) LVSUNLEDsOff();     // turn off all LEDs on the LVSUN charger V053/V076
    #endif  //LVSUNCHARGER

    initTimers();
    runStateInt = runState = prevRunState = D_STARTUP;
    lastLoop = micros();                            // set last loop time to current time
}

// helper to manage the power on state setting within setup()
int powerOnStateSelection()
{
    int result = D_GOTOALWAYS_ON; // default to always on

    switch (param.powerOnState)
    {
        case POS_LAST:
            Log.info("Power On State: Last State %i", param.resumeState);
            switch (param.resumeState)      //V104
            {
                case D_STANDBY:
                    result = D_GOTOSTANDBY;
                    break;
                case D_TIMED_ON:
                    result = D_GOTOTIMED_ON;
                    break;
                case D_ALWAYS_ON:
                    result = D_GOTOALWAYS_ON;
                    break;
                case D_AUTO_OFF:
                case D_AUTO_ON:
                    result = D_GOTOAUTO;
                    break;
                case D_CHARGED_ON:
                case D_CHARGED_ON_AUTO:
                    result = D_GOTOCHARGED_ON;
                    break;
                case D_CHARGED_ON_USBC:
                case D_CHARGED_ON_USBC_AUTO:
                    result = D_GOTOCHARGED_ON_USBC;
                    break;
                default:
                    result = D_GOTOSTANDBY;
                    break;
            }   
            break;
        case POS_STANDBY:
            Log.info("Power On State: Standby");
            result = D_GOTOSTANDBY;
            break;
        case POS_TIMED_ON:
            Log.info("Power On State: Timed On");
            result = D_GOTOTIMED_ON;
            break;
        case POS_CONT_ON:
            Log.info("Power On State: Continuous On");
            result = D_GOTOALWAYS_ON; // continuous on is always on
            break;
        case POS_SMART_AC:
            Log.info("Power On State: Smart AC");
            result = D_GOTOCHARGED_ON;
            break;
        case POS_SMART_USB:
            Log.info("Power On State: Smart USB");
            result = D_GOTOCHARGED_ON_USBC;
            break;
        default:
            Log.error("Power On State: Unknown State %i", param.powerOnState);
            result = D_GOTOALWAYS_ON; // default to always on if unknown state
            break;
    }
    return result;
}

void loop()
{
    loopRate = (int) (micros() - lastLoop);         // calculate loop rate
    lastLoop = micros();                            // set last loop time to current time

    Watchdog.refresh();                             // refresh the watchdog timer

    sensorReading();                                // read the sensors

    EthernetWiFi::instance().loop();                // required to manage network connection
    checkNetworks();

    configurationAvailableCheck();                  // check if configuration is available from Ledger update

    checkTimeSync();                                // check if time needs to be synchronized with cloud
    clockTimeUpdate(false);                         // unforced check if time has changed and update if necessary V093

    PublishQueuePosix::instance().loop();

    #if GOOGLE_LOCATE
    if (Particle.connected()) locator.loop();
    checkLocation();
    #endif  //GOOGLE_LOCATE

	BLEWiFiSetupManager::instance().loop();
    wifiProvisioning();

    checkScheduledStartUntilCharged();              //check if scheduled start smart charge needs to be actioned V111

    onViewRunState();                               //to present a usable runState to onView

    switch (runState)                               //runState select action
    {
    case D_STARTUP:
        startupController();
        break;
    case D_RESTART:
        restartController();
        break;
    case D_GOTOSTANDBY:
        goToStandbyController();
        break;
    case D_STANDBY:
        standbyController();
        break;
    case D_WEBGOTORESTART:
        webGoToRestartController();
        break;
    case D_GOTOTIMED_ON:
        goToTimedOnController();
        break;
    case D_TIMED_ON:
        timedOnController();
        break;
    case D_GOTOALWAYS_ON:
        goToOnController();
        break;
    case D_ALWAYS_ON:
        onController();
        break;
    case D_GOTOAUTO:
        goToAutoController();
        break;
    case D_AUTO_OFF:
    case D_AUTO_ON:
        autoController();
        break;
    case D_GOTOCHARGED_ON:
        goToChargedOnController();
        break;
    case D_GOTOCHARGED_ON_AUTO:
        goToChargedOnAutoController();
        break;
    case D_CHARGED_ON:
    case D_CHARGED_ON_AUTO:
        chargedOnController();
        break;
    case D_GOTOCHARGED_ON_USBC:
        #if LVSUNCHARGER
        goToChargedOnUSBCController();
        #endif //LVSUNCHARGER
        break;
    case D_GOTOCHARGED_ON_USBC_AUTO:
        #if LVSUNCHARGER
        goToChargedOnUSBCAutoController();
        #endif //LVSUNCHARGER
        break;
    case D_CHARGED_ON_USBC:
    case D_CHARGED_ON_USBC_AUTO:
        #if LVSUNCHARGER
        chargedOnUSBCController();
        #endif //LVSUNCHARGER
        break;
    case D_GOTOSLEEP:
        goToSleepController();
        break;
    case D_SLEEPING:
        sleepingController();
        break;
    case D_WEBGOTOSTANDBY:
        webGoToStandbyController();
        break;
    case D_WEBGOTOTIMED:
        webGoToTimedController();
        break;
    case D_WEBGOTOAUTO:
        webGoToAutoController();
        break;
    case D_WEBGOTOON:
        webGoToOnController();
        break;
    case D_WEBGOTOCHARGED:
    case D_WEBGOTOCHARGEDUSBC:
        webGoToChargedController();
        break;
    case D_WEBGOTOHARDRESTART:
        webGoToFactoryResetController();
        break;
    case D_WEBGOTOHIBERNATE:
        webGoToHibernateController();
        break;
    case D_WEBOUCAUTO:
        webGoToOUCAutoController();
        break;
    case D_WEBOUCAEXIT:
        webOUCAExitController();
        break;
    default:
        runState = D_STANDBY;
    }

    checkDeviceUpdate();

}

// helper to manage the restart (immediate post setup())
void startupController()
{
    clockTimeUpdate(true);      // force check if date/time needs to be updated for DST V093
    initialDESTevent();

    param.resumeFlag = 0;       // V097
    putParameters();
    
    runState = powerOnStateSelection(); //V078 - select the power on state based on the parameter setting
}

// helper to format and send startup event
void initialDESTevent()
{
    char temp[2] = {0};
    temp[0] = param.scheduleType;
    param.numDevices = numberOfAdaptors();

    #if LVSUNCHARGER
    uint32_t hubversion = hubBoardVersion();    //V060
    char lvsun[4][17] = {0};

    if (hubversion == 0)
    {
        Log.info("LVSUN Hub board not found or not responding");
        param.hubBoard = 0; // hub board not found or not responding V124
        putParameters();    //V124
        fault[7] = 7; // handle the hub board not found/responding case
    }
    else 
    {
        fault[7] = 0; // OK, hub board found and responding
        param.hubBoard = 1; // hub board found V124
        putParameters();    //V124
        Log.info("LVSUN Hub board version: %lu", hubversion);
        int8_t temp = 0;
        hubdata.firmwareRevision = hubversion%100;
        //Log.info("LVSUN Hub firmware version: %i", hubdata.firmwareRevision);
        hubversion /= 100;
        hubdata.hardwareRevision = hubversion%10;
        //Log.info("LVSUN Hub hardware version: %i", hubdata.hardwareRevision);
        hubversion /= 10;
        hubdata.doorSensors = hubversion%10;
        //Log.info("LVSUN Hub door sensors: %i", hubdata.doorSensors);
        hubversion /= 10;
        temp = hubversion%10;
        if      (temp == 2) hubdata.portsIn = 10;
        else if (temp == 6) hubdata.portsIn = 16;
        else                hubdata.portsIn = 0; // default to no ports
        //Log.info("LVSUN Hub input ports: %i", hubdata.portsIn);
        hubversion /=10;
        hubdata.channelsIn = hubversion%10;
        //Log.info("LVSUN Hub input channels: %i", hubdata.channelsIn);
        hubversion /= 10;
        temp = hubversion%10;
        if      (temp == 2) hubdata.portsLED = 10;
        else if (temp == 6) hubdata.portsLED = 16;
        else                hubdata.portsLED = 0; // default to no ports
        //Log.info("LVSUN Hub LED ports: %i", hubdata.portsLED);
        hubversion /= 10;
        hubdata.channelsLED = hubversion%10;
        //Log.info("LVSUN Hub LED channels: %i", hubdata.channelsLED);
        hubversion /= 10;
        hubdata.isInputs = (hubversion%10 == 1); // 1 = inputs, 0 = outputs
        //Log.info("LVSUN Hub isInputs: %c", hubdata.isInputs?'T':'F');
        hubversion /= 10;
        hubdata.isOutputs = (hubversion%10 == 1); // 1 = outputs, 0 = inputs
        //Log.info("LVSUN Hub isOutputs: %c", hubdata.isOutputs?'T':'F');
        
        for (int8_t i = 0; i < hubdata.channelsIn; i++) // V094
        {
            for (int8_t j = 0; j < hubdata.portsIn; j++) {lvsun[i][j] = '0';} // all ports off V094
        }
    }
    #endif //LVSUNCHARGER

    char bleAddr[18] = {0};
    BleAddress bacAddress = BLE.address();  // get the device BLE address V114
    for (int i = 0; i < 6; i++) {param.bleA[i] = bacAddress[5-i];}  //V114
    snprintf(bleAddr, sizeof(bleAddr), "%02X:%02X:%02X:%02X:%02X:%02X", param.bleA[0], param.bleA[1], param.bleA[2], param.bleA[3], param.bleA[4], param.bleA[5]);
    
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("F").value(firmware);
    writer.name("V").value(softwarebuild);
    writer.name("H").value(hardwarebuild);
    writer.name("PRC").value((const char*)param.productCode);
    writer.name("SN").value((const char*)param.serialNum);
    writer.name("B").beginArray();
    for (int i = 0; i < 4; i++) {writer.value(param.numSpaces[i]);}
    writer.endArray();
    writer.name("DPO").beginArray();
    for (int i = 0; i < 4; i++) {writer.value(param.devicesPerOutlet[i]);}
    writer.endArray();
    writer.name("MMT").value(param.maxMonitoringTime);  //V118
    writer.name("EXT").value(param.extraChargingTime);  //V118
    writer.name("MNR").value((double) param.minChargeRate); //V118
    writer.name("MNC").value((double) param.minCurrent);    //V118
    writer.name("WUP").value(param.warmupMins);   //V118
    //writer.name("MD").value(param.numDevices);  //V122
    writer.name("DS").value(param.doorSensors);
    writer.name("TS").value(param.tempSensors);
    writer.name("J").value(param.isDst?1:0);
    writer.name("JC").value(param.isAutoDst?2:param.isDst?1:0);
    writer.name("RR").value(param.resumeCause); //restartdata.resumeReason); - this was returning 0
    writer.name("MT").value(param.maxTemp);
    writer.name("TZ").value(param.timeZone,1);
    writer.name("T").value(param.timerPeriod);
    writer.name("MX").value(param.maxTimeOn);
    writer.name("NAM").value((const char*)param.itemName);
    writer.name("ST").value((const char*)temp); // Convert char to const char*
    writer.name("TSF").beginArray();
    for (int i = 0; i < 8; i++) {writer.value((int) fault[i]);}
    writer.endArray();
    #if LVSUNCHARGER
    if (param.hubBoard == 1)    //V076
    {
        writer.name("HB0").beginArray();        // LVSUN Hub data as a JSON array V064
        writer.value((int) hubdata.hardwareRevision);
        writer.value((int) hubdata.firmwareRevision);
        writer.value((int) hubdata.portsIn);
        writer.value((int) hubdata.channelsIn);
        writer.value((int) hubdata.portsLED);
        writer.value((int) hubdata.channelsLED);
        writer.value((int) hubdata.doorSensors);
        writer.value(hubdata.isInputs);
        writer.value(hubdata.isOutputs);
        writer.endArray();
    }

    if (hubdata.channelsIn > 0)                // V094
    {
        writer.name("LV0").beginArray();
        writer.value(1);
        writer.value((const char*) &lvsun[0][0]);
        writer.value(2);
        writer.value((const char*) &lvsun[1][0]);
        writer.value(3);
        writer.value((const char*) &lvsun[2][0]);
        writer.value(4);
        writer.value((const char*) &lvsun[3][0]);
        writer.endArray();
    }
    #endif //LVSUNCHARGER

    writer.name("POS").value(param.powerOnState);
    if (bleAddr[0] != 0) writer.name("BLE").value((const char*)bleAddr);
    writer.name("LOC").value(param.isLocalMode);    //V128
    writer.endObject();
    PublishQueuePosix::instance().publish(eventstartupdat, dataStr, 50, PRIVATE);
}

#if GOOGLE_LOCATE
// helper to call location checker on timed basis
void checkLocation()
{
    static unsigned long locateupdate = 0;
    const unsigned long LCHK = 240000;

    if (millis()-locateupdate >= LCHK)
    {
        locateupdate = millis();
    
        if (WiFi.ready() || Ethernet.ready())
        {
            if (Particle.connected()) locator.loop();
        }
    }
}

// subscription handler for Google location service
void locationCallback(float lat, float lon, float accuracy)
{
    latitude  = lat;
    longitude = lon;
    locateAcc = accuracy;
}
#endif  //GOOGLE_LOCATE

// helper to setup MCP9800 based I2C temperature sensor on the board to read oneshot 9bit resolution shutdown mode 
void setupTemperatureSensor()
{
    fault[0] = tmpSensor.begin(0);
}

// returns temperature at the sensor as smoothed float oC (sensor accuracy is +/- 1 oC)
float temperatureFromSensor()
{
    int _temp = tmpSensor.readTemperature();
    _temp -= TEMPOFFSET;
    return smoothTemperature(_temp);
}

// function to smooth the temperature readings using a rolling average and will round down and return float
float smoothTemperature(int value)
{
	const int WINDOW = 15;
	static int _pointer = 0;
	static int _maxpointer = 0;
	static int _readings[WINDOW];

    _readings[_pointer] = value;
    float total = 0;
    for (int i = 0; i <= _maxpointer; i++) {total += _readings[i];}
    float avg = total / (_maxpointer + 1);
    if (_maxpointer < WINDOW - 1) _maxpointer++;
    _pointer++;
    if (_pointer == WINDOW) _pointer = 0;
    return avg;
}

#if EXT_TEMP_SENSOR
// setup external I2C bus temperature sensor(s)
void setupExternalTempSensor()
{
    if (param.tempSensors == 0) return; // no external sensors to setup

    int sensor = 1;
    while(param.tempSensors >= sensor)
    {
        switch (sensor)
        {
            case 1:
                fault[sensor] = extSensor1.begin(0);
                break;
            case 2:
                fault[sensor] = extSensor2.begin(1);
                break;
            case 3:
                fault[sensor] = extSensor3.begin(2);
                break;
            case 4:
                fault[sensor] = extSensor4.begin(3);
                break;
            default:
                break;
        }
        sensor++;
    }
}

// returns temperature at the external sensor as smoothed float oC (sensor accuracy is +/- 1 oC)
float temperatureFromXSensor()
{
    if (param.tempSensors == 0) return 0.0; // no external sensors to read
    int _temp[5] = {0};
    int sensor = 1;

    while (param.tempSensors >= sensor)
    {
        switch (sensor)
        {
            case 1:
                if (fault[sensor] == 0) {_temp[sensor] = extSensor1.readTemperature();}
                break;
            case 2:
                if (fault[sensor] == 0) {_temp[sensor] = extSensor2.readTemperature();}
                break;
            case 3:
                if (fault[sensor] == 0) {_temp[sensor] = extSensor3.readTemperature();}
                break;
            case 4:
                if (fault[sensor] == 0) {_temp[sensor] = extSensor4.readTemperature();}
                break;
            default:
                break;
        }
        sensor++;
    }

    if      (param.tempSensors == 1)        // one external sensor
    {
        _temp[0] = _temp[1];                // take this sensor only    
    }
    else if (param.tempSensors == 2)        // two external sensors
    {
        _temp[0] = max(_temp[1], _temp[2]); // take the maximum of the two sensors
    }
    else if (param.tempSensors == 3)        // three external sensors
    {
        _temp[0] = max(_temp[1], max(_temp[2], _temp[3])); // take the maximum of the three sensors
    }
    else if (param.tempSensors == 4)        // four external sensors
    {
        _temp[0] = max(_temp[1], max(_temp[2], max(_temp[3], _temp[4]))); // take the maximum of the four sensors
    }
    
    return smoothXTemperature(_temp[0]);
}

// function to smooth the external temperature readings using a rolling average and will round down and return float
float smoothXTemperature(int value)
{
	const int WINDOW = 15;
	static int _pointer = 0;
	static int _maxpointer = 0;
	static int _readings[WINDOW];

    _readings[_pointer] = value;
    float total = 0;
    for (int i = 0; i <= _maxpointer; i++) {total += _readings[i];}
    float avg = total / (_maxpointer + 1);
    if (_maxpointer < WINDOW - 1) _maxpointer++;
    _pointer++;
    if (_pointer == WINDOW) _pointer = 0;
    return avg;
}
#endif

// clear the restart data structure and save to RTC RAM
void clearRestartData()
{
    std::memset(&restartdata, 0, sizeof(restartData));
    saveRestartDataToRam();
}

// when AB1805 is enabled it's battery backed RAM can be used to save restart data
void saveRestartDataToRam()
{
    std::memcpy(serializedRData, &restartdata, sizeof(restartData));
    (void) MCP7940.writeRAM(RESTARTDATA_ADDR, serializedRData);
    Log.info("Saved restart data to MCP7940 RAM %02x %02x %02x %02x %02x %02x %02x %02x", serializedRData[0], serializedRData[1], serializedRData[2], serializedRData[3], serializedRData[4], serializedRData[5], serializedRData[6], serializedRData[7]);
}

// when AB1805 is enabled it's battery backed RAM can be used to restore restart data
void restoreRestartDataFromRam()
{
    (void) MCP7940.readRAM(RESTARTDATA_ADDR, serializedRData);
    Log.info("Read restart data from MCP7940 RAM %02x %02x %02x %02x %02x %02x %02x %02x", serializedRData[0], serializedRData[1], serializedRData[2], serializedRData[3], serializedRData[4], serializedRData[5], serializedRData[6], serializedRData[7]);
    std::memcpy(&restartdata, serializedRData, sizeof(restartData));
}

// when AB1805 is enabled it's battery backed RAM can be used to save time (and other) settings
void saveTimeSettingsToRam()
{
    timedata.timezone = Time.zone();            // store the timezone value (float) in RTC RAM
    timedata.DSToffset = Time.getDSTOffset();   // store the DST offset value (float) in RTC RAM
    timedata.DST = (int) Time.isDST()?1:0;      // store the DST value (int) in RTC RAM
    std::memcpy(serializedTData, &timedata, sizeof(timeData));
    (void) MCP7940.writeRAM(TIMEDATA_ADDR, serializedTData);
}

// when AB1805 is enabled it's battery backed RAM can be used to restore time (and other) settings
void restoreTimeSettingsFromRam()
{
    float timezone;
    MCP7940.readRAM(TIMEDATA_ADDR + TIMEZONE_ADDR, timezone);
    if (timezone < -12.0 || timezone > 14.0) {timezone = 0.0;} // if invalid timezone value then set to 0.0
    Time.zone(timezone);

    float DSToffset;
    MCP7940.readRAM(TIMEDATA_ADDR + DST_OFFSET_ADDR, DSToffset);
   if (DSToffset != 1.0) {DSToffset = 1.0;} // if invalid DST offset value then set to 1.0
    Time.setDSTOffset(DSToffset);

    int DST;
    MCP7940.readRAM(TIMEDATA_ADDR + DST_ADDR, DST);
    if (DST == 1)
    {
        Time.beginDST();
        param.isDst = true;
    }
    else
    {
        Time.endDST();
        param.isDst = false;
    }
    putParameters();

    if (MCP7940.isRTCSet())
    {
        time_t rtctime = MCP7940.now().unixtime();  // Get the time from the RTC as a time_t value
        Time.setTime(rtctime);                      // set the P2 time/date to the RTC time/date
    }
}

// helper function to regularly check and update local time (if cloud connected or has been set before)
void clockTimeUpdate(bool _forceUpdate)
{
    static unsigned long lastclockupdate = 0;
    if ((millis() - lastclockupdate >= CLOCKUPDATE) || _forceUpdate)    //V093
    {
        lastclockupdate = millis();
        if (Time.isValid()) // only do this if RTC Time is valid
        {
            if (param.isAutoDst) // only check automatically for change in DST if param.isAutoDst is true
            {
                if (isDSTactive()) // true if changed, false if no change in DST - this is so it is only reported once
                {
                    memset(dataStr, 0, sizeof(dataStr));
                    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
                    writer.beginObject();
                    writer.name("date").value((const char*)getCreatedTime());
                    writer.name("J").value((int)(param.isDst?1:0));
                    writer.name("JC").value((int)(param.isAutoDst?2:0));
                    writer.endObject();
                    PublishQueuePosix::instance().publish(eventvarchanged, dataStr, 50, PRIVATE);
                    saveTimeSettingsToRam();
                }
            }
        }
        else
        {
            restoreTimeSettingsFromRam(); // restore time settings from RTC RAM
        }
    }
}

// function to check if last sync with Particle Cloud is more than one day and if so then request time sync and update MCP7940 RTC
void checkTimeSync()
{
    if (Particle.connected())                                                   //Only time sync when Cloud connected V288
    {
        time_t lastSyncTimestamp;
        unsigned long lastSync = Particle.timeSyncedLast(lastSyncTimestamp);
        if (millis() - lastSync >= ONE_DAY_MILLIS)                              //More than one day since last time sync
        {
            unsigned long cur = millis();
            Particle.syncTime();                                                //Request time synchronization from Particle Device Cloud
            waitUntil(Particle.syncTimeDone);                                   //Wait until device receives time from Particle Device Cloud (or connection to Particle Device Cloud is lost)
            if (Particle.timeSyncedLast() >= cur)                               //Check if synchronized successfully
            {
                MCP7940.adjust(Time.now());                                     //Set the RTC to the current time
            }
        }
    }
}

// returns true or false should DST changed event be sent
// according to the time zone if (0.0(exc. Iceland), +1.0, +2.0) Europe exc. Russia/Belarus
// essentially from the last Sunday in March to the last Sunday in October DST is enabled at UTC 01.00 -> 02.00 and disabled at UTC 02.00 -> 01.00
// automatically begins DST at UTC 01.00 or ends DST at UTC 02.00 - change is saved to AB1805 RAM
bool isDSTactive()
{
    float tz = Time.zone();                                 //offset from UTC
    int dayOfMonth = Time.day();
    int hour = ((Time.now() % 86400) / 3600);
    int month = Time.month();
    int dayOfWeek = Time.weekday() - 1;                     //make Sunday 0 .. Saturday 6
    bool shouldDSTbeEnabled = false;                        //not in European time zones for which coded DST rules apply by default
    if (tz == 0.0 || tz == 1.0 || tz == 2.0)                //UK and European time zones (WET/GMT, CET, EET)
    {
        switch (month)
        {
            case 1:                                         //Jan, Feb, Nov and Dec are definitely standard time
            case 2:
            case 11:
            case 12:
                shouldDSTbeEnabled = false;
                break;
            case 4:                                         //Apr to Sep definitely DST
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                shouldDSTbeEnabled = true;
                break;
            case 10:                                        //March and October need deeper examination
            case 3:
                if (!(dayOfMonth - dayOfWeek > 24))         //but before switching Sunday     
                {
                    shouldDSTbeEnabled = (month == 10);     //for October DST will be true, for March not
                }
                else if (dayOfWeek > 0)                     //after the switching Sunday (dayOfWeek !=)
                {
                    shouldDSTbeEnabled = (month == 3);      //for March DST is true, for October not
                }
                else                                        //switching Sunday (dayOfWeek = 0)
                {
                    if (hour >= 1 && month == 3)  shouldDSTbeEnabled = true; //time is 01:00 or later on switching sunday in march then DST should be ON
                    if (hour >= 2 && month == 10) shouldDSTbeEnabled = false; //time is 02:00 or later on switching sunday in october then DST should be OFF
                }
                break;
        }
        //then if a change condition check if switch required from current setting
        if (shouldDSTbeEnabled && !Time.isDST())            //if it should be on but is off
        {
            param.isDst = true;
            putParameters();
            Time.beginDST();
            return true;
        }
        else if (!shouldDSTbeEnabled && Time.isDST())       //if it should be off but is on
        {
            param.isDst = false;
            putParameters();
            Time.endDST();
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

// helper to return created time in format for event "date" field
char* getCreatedTime()
{
    time_t localTime = 0;
    static char createdAt[28];
    memset(createdAt, 0, sizeof(createdAt));

    if (!Time.isValid())
    {
        restoreTimeSettingsFromRam();
    }
    localTime = Time.now();

    strcpy(createdAt, Time.format(localTime, "%Y-%m-%dT%H:%M:%S"));
    return createdAt;
}

// this is the out of memory handler. It's called when an out of memory condition occurs.
// need to add backup of parameters to AB1805 RAM area
void outOfMemoryHandler(system_event_t event, int _param)
{
    outOfMemory = _param;
    restartdata.resumeReason = RESUME_OUT_OF_MEMORY;
    param.resumeCause = RESUME_OUT_OF_MEMORY; //V076
    restartdata.resumeState = runState;
    restartdata.powerOnState = D_STANDBY;   //this should be a configuration parameter TO DO   
    for (int i = 0; i < (int) (sizeof(P2_PDU_RELAYS) / sizeof(P2_PDU_RELAYS[0])); i++) {restartdata.relayState[i] = (uint8_t) (digitalRead(P2_PDU_RELAYS[i])?1:0);}
    if (param.hubBoard == 1) restartdata.isHubBoard = true; //V076
    else                     restartdata.isHubBoard = false;
    putParameters(); // save parameters to EEPROM
    saveRestartDataToRam();
}

// this is the reset on OTA handler. It's called when an OTA reset occurs.
void handle_restart_events(system_event_t event, int data)
{
    if (event == reset)
    {
        PublishQueuePosix::instance().setPausePublishing(true);
        isResetPending = false;
    }
    else if (event == reset_pending)
    {
        System.enableReset();
    }
    else if (event == firmware_update && data == firmware_update_complete)
    {
        PublishQueuePosix::instance().setPausePublishing(true); 
        param.resumeFlag = RESTART_FIRMWARE_UPDATE;
        putParameters();
        isResetPending = true;
    }
    else if (event == firmware_update && data == firmware_update_begin)
    {
        isOKtoSleep = false;

    }
}

// setup pin modes and initial states
void setupPins()
{
    pinMode(P2_PDU_3V3_SW_EN, OUTPUT);
	pinSetFast(P2_PDU_3V3_SW_EN);

  	pinMode(P2_PDU_5V_I2C_EN, OUTPUT);
	pinSetFast(P2_PDU_5V_I2C_EN);

    pinMode(P2_PDU_WAKE, INPUT_PULLDOWN);

  	pinMode(P2_PDU_SUPPLY_DETECT, INPUT);

  	pinMode(P2_PDU_BATT_CHARGING1, INPUT);
  	pinMode(P2_PDU_BATT_CHARGING2, INPUT);
  	
    pinMode(P2_PDU_RTC_MFP, INPUT);
    attachInterrupt(P2_PDU_RTC_MFP, rtcMFP, FALLING);  // Attach interrupt to P2_PDU_RTC_MFP for MFP
    
	for (size_t i = 0; i < sizeof(P2_PDU_RELAYS) / sizeof(P2_PDU_RELAYS[0]); i++)
	{
		pinMode(P2_PDU_RELAYS[i], OUTPUT);
		pinResetFast(P2_PDU_RELAYS[i]);
	}
}

// call from setup() to initialise the MCP7940 RTC
void setupMCP7940()
{
    if (MCP7940.setup()) // If the device is found
    {
        Log.info("MCP7940 RTC found and setup successfully");
        MCP7940.deviceStop(); // Stop the MCP7940 RTC to configure it
        MCP7940.setMFP(true); // set MFP to off
        MCP7940.setAlarmPolarity(false);
        MCP7940.deviceStart(); // Start the MCP7940 RTC device  
        if (MCP7940.deviceStatus())
        {
            fault[5] = 0; // no fault
            Log.info("MCP7940 RTC device status is running");
            if (MCP7940.isRTCSet()) // If the RTC is set
            {
                Log.info("MCP7940 RTC is set");
                time_t rtctime = MCP7940.now().unixtime();  // Get the time from the RTC as a time_t value
                Time.setTime(rtctime);               // set the P2 time/date to the RTC time/date
                sampleBMS(); // Sample the BMS to get the initial state
                if (!batterydata.isFault)
                {
                    if (MCP7940.getBattery())
                    {
                        Log.info("MCP7940 RTC battery is OK and already set");
                    }
                    else
                    {
                        (void) MCP7940.setBattery(true);
                    }
                }
                else
                {
                    (void) MCP7940.setBattery(false);
                }
            }
            else
            {
                Log.info("MCP7940 RTC is not set, setting to current time if valid");
                if (Time.isValid()) MCP7940.adjust(Time.now()); // Set the RTC to the current time
                MCP7940.setMFP(true); // set MFP to on
                Log.info("MCP7940 RTC started successfully - MFP set to on");
                MCP7940.setAlarmPolarity(false);  // Set alarm polarity to active low
                Log.info("Alarm polarity set to active low");
                bool res = MCP7940.setBattery(true); // Set battery backup mode
                Log.info("Battery backup mode set: %s", res ? "true" : "false");
                MCP7940.clearPowerFail(); // Clear power fail flag
                Log.info("Power fail flag and data cleared");
                clearRestartData(); // clear any restart data in RAM
                
            }
        }
        else
        {
            Log.info("MCP7940 RTC not running - start");
            MCP7940.setMFP(true); // set MFP to on
            MCP7940.setAlarmPolarity(false);  // Set alarm polarity to active low
            MCP7940.deviceStart(); // Start the MCP7940 RTC
            while (!MCP7940.deviceStatus()) delay(500);  // Wait for MCP7940 to be ready  
            if (MCP7940.deviceStatus())
            {
                fault[5] = 0; // no fault
                Log.info("MCP7940 RTC device status is running");
                if (MCP7940.isRTCSet()) // If the RTC is set
                {
                    time_t rtctime = MCP7940.now().unixtime();  // Get the time from the RTC as a time_t value
                    Time.setTime(rtctime);               // set the P2 time/date to the RTC time/date
                    Log.info("MCP7940 RTC is set update P2 time to %llu", rtctime);
                    sampleBMS(); // Sample the BMS to get the initial state
                    if (!batterydata.isFault)
                    {
                        if (MCP7940.getBattery())
                        {
                            Log.info("MCP7940 RTC battery is OK and already set");
                        }
                        else
                        {
                            (void) MCP7940.setBattery(true);
                        }
                    }
                    else
                    {
                        (void) MCP7940.setBattery(false);
                    }
                }
                else
                {
                    Log.info("MCP7940 RTC is not set, setting to current time if valid");
                    if (Time.isValid()) MCP7940.adjust(Time.now()); // Set the RTC to the current time
                    MCP7940.setMFP(true); // set MFP to on
                    Log.info("MCP7940 RTC started successfully - MFP set to on");
                    MCP7940.setAlarmPolarity(false);  // Set alarm polarity to active low
                    Log.info("Alarm polarity set to active low");
                    bool res = MCP7940.setBattery(true); // Set battery backup mode
                    Log.info("Battery backup mode set: %s", res ? "true" : "false");
                    MCP7940.clearPowerFail(); // Clear power fail flag
                    Log.info("Power fail flag and data cleared");
                    clearRestartData(); // clear any restart data in RAM
                }
            }
        }
    }
    else
    {
        fault[5] = 7;
    }
}

//
void checkAlarm()
{
    if (mfpPinTriggered == true)
    {
        Serial.println("MFP Triggered");
        mfpPinTriggered = false;
        if (MCP7940.isAlarm(0))  // Check if Alarm 0 is triggered
        {
            Serial.println("Alarm0 Triggered");
            MCP7940.clearAlarm(0);  // Clear Alarm 0
        }
        else
        {
            Serial.println("Alarm0 False Alarm");
        }
    }
}

// MFP interrupt handler for MCP7940
void rtcMFP()
{
    mfpPinTriggered = true;
}

// call from setup() to initialise the ACS37800 current sensor
void setupACS37800()
{
	//if (!Wire.isEnabled()) Wire.begin();
	//mySensor.enableDebugging(); // Uncomment this line to print useful debug messages to logger

	if (mySensor.begin() == false)      	//initialize sensor using default I2C address
	{
		Log.error("ACS37800 not detected. Check connections and I2C address.");
        fault[6] = 7; // If the sensor is not detected, set the fault state
	}
    else
    {
        fault[6] = 0; // If the sensor is detected, clear the fault state
	    mySensor.setBypassNenable(false, true); // Disable bypass_n in shadow memory and eeprom was false true
    }
}

// setup ethernet and wifi fallback mechanism
void setupNetwork()
{
    EthernetWiFi::instance().setup();
    EthernetWiFi::instance().withEthernetConnectTimeout(std::chrono::milliseconds(2min));

    Log.info("setupNetwork Operation mode local: %c", param.isLocalMode?'T':'F'); //V128
    if (param.isLocalMode) EthernetWiFi::instance().setAutomaticInterface(false);  //set for local mode V128
    else                   EthernetWiFi::instance().setAutomaticInterface(true);   //select according to what is available Ethernet or WiFi to connect

    connectTime = millis();
    eventdelay = checknetwork = millis();
    wasEthernetConnected = false;
    wasWiFiConnected = false;
    wasCloudConnected = false;
}

// check network connection
void checkNetworks()
{    
    if (millis() - checknetwork > NETWORK_CHECK_INTERVAL) //V089
    {
        checknetwork = millis();

        int activenetwork = EthernetWiFi::instance().getActiveInterface();

        if ((activenetwork == (int) EthernetWiFi::ActiveInterface::OFF) && !param.isLocalMode) // V128
        {
            Log.info("Switched to local mode update parameters");
            param.isLocalMode = true;
            putParameters();
        }
        else if ((activenetwork != (int) EthernetWiFi::ActiveInterface::OFF) && param.isLocalMode)
        {
            Log.info("Switched to connected mode update parameters");
            param.isLocalMode = false;
            putParameters();
        }

        if (param.isLocalMode)
        {
            Log.info("Operating in local mode - no cloud connection attempts");
        }
        else
        {
            isEthernetConnected = Ethernet.ready();

            if      (isEthernetConnected && !wasEthernetConnected)  {wasEthernetConnected = true; }
            else if (!isEthernetConnected && wasEthernetConnected)  {wasEthernetConnected = false;}

            if      (!isEthernetConnected)
            {
                isWiFiConnected = WiFi.ready();
                
                if      (isWiFiConnected && !wasWiFiConnected)      {wasWiFiConnected = true; }
                else if (!isWiFiConnected && wasWiFiConnected)      {wasWiFiConnected = false;}
            }

            if (isEthernetConnected || isWiFiConnected)
            {
                bool isCloudConnected = Particle.connected();
                if (!isCloudConnected && wasNotCalledConnect) {wasNotCalledConnect = false; Particle.connect();}
                if      (isCloudConnected && !wasCloudConnected)
                {
                    wasCloudConnected = true;
                    connectedSince = millis();
                    MCP7940.adjust(Time.now());             // RTC updated with current time once cloud connected V093
                }
                else if (!isCloudConnected && wasCloudConnected)
                {
                    wasCloudConnected = false;
                    ethernetConnectedCount++;
                    connectedSince = 0UL;
                }
            }

            networkInfoEvent(); // publish network info event V070
        }
    }
}

// read sensor data
void sensorReading()
{
    static unsigned long lastSensorRead = 0;
    if (millis() - lastSensorRead > 10000UL) // every 10 seconds
    {
        lastSensorRead = millis();

        powerdata.isACsupply = digitalRead(P2_PDU_SUPPLY_DETECT);

        sampleBMS();
            
        boardTemp = temperatureFromSensor();
        bTemp = (double) boardTemp;

        #if EXT_TEMP_SENSOR
        xtemp = temperatureFromXSensor();
        xTemps = (double) xtemp;
        #endif

        sampleCurrent();

        //Log.info("Volts (RMS): %4.1f Amps(RMS): %5.3f Active Power(W): %4.2f Reactive Power(W): %4.2f", voltsrms, powerdata.ampsrms, powerdata.apowerwatt, powerdata.rpowerwatt);
    }
}

// helper to return true if AC supply is present
bool isACSupplyPresent()
{
    return digitalRead(P2_PDU_SUPPLY_DETECT);
}

// loop function to handle WiFi provisioning
void wifiProvisioning()
{
    // State machine to handle WiFi provisioning states - add triggers for BLE connection event V071
    static bool wasBLEConnected = false;
    static bool wasBLEinformed = false; //V101

    switch(provision_state)
    {
        case STATE_IDLE:
        {
            next_provision_state = STATE_IDLE;
            bool isBLEconnected = BLE.connected(); //V101
            if (!wasBLEConnected && isBLEconnected) //V071
            {
                wasBLEConnected = true;
                sendBLEProvisioningEvent(true); // send event to indicate BLE connection for WiFi provisioning
            }
            else if (wasBLEConnected && !isBLEconnected)
            {
                wasBLEConnected = false;
                wasBLEinformed = false; //V101
                sendBLEProvisioningEvent(false); // send event to indicate BLE disconnection for WiFi provisioning
                BLEWiFiSetupManager::instance().stopStartAdvertising(true); //start BLE advertising V100
            }
            else if (wasBLEConnected && isBLEconnected && !wasBLEinformed) // still connected to the same BLE mobile app as connection process on going V101
            {
                if (WiFi.ready() && !(Particle.connected()))
                {
                    Log.info("WiFi connected - let the BLE mobile app know");
                    cloudConnected();
                    BLEWiFiSetupManager::instance().status_message("Connected to WAP");
                    BLEWiFiSetupManager::instance().status_message("Connecting to Cloud..."); // let the BLE mobile app know WiFi is connected
                }
                else if (WiFi.ready() && Particle.connected())
                {
                    Log.info("Cloud connected - let the BLE mobile app know");
                    BLEWiFiSetupManager::instance().status_message("Connected to Cloud"); // let the BLE mobile app know WiFi is connected
                    wasBLEinformed = true; //V101
                }
            }
            break;
        }

        case STATE_PROVISIONED:                         //this state is set by the provision callback from BLEWiFiSetupManager
        {
            next_provision_state = STATE_PROVISIONED;
            if (param.isLocalMode)                      //V128
            {
                Log.info("In local mode no connection provisioned credentials");
                next_provision_state = STATE_IDLE;
            }
            else
            {
                if (!(WiFi.ready()))                        //Credentials set and waited 40 seconds but not connected what should be done?
                {
                    Log.info("WiFi credentials provisioned but not connected to WiFi goto STATE_IDLE");
                    wasBLEinformed = false; //V101
                    next_provision_state = STATE_IDLE;
                }
                else if (WiFi.ready() && !(Particle.connected()))
                {
                    Log.info("WiFi but not Cloud connected");
                    WiFi.listen(false); //stop listening V105
                    wasBLEinformed = false; //V101
                    cloudConnected();
                    next_provision_state = STATE_IDLE;
                }
                else
                {
                    Log.info("WiFi and Cloud connected");
                    next_provision_state = STATE_IDLE;
                }
            }
            break;
        }

        case STATE_CREDENTIALS:                         //this state is set in setup if credentials are available
        {
            if (param.isLocalMode)                      //V128
            {
                Log.info("In local mode no connection with credentials");
                next_provision_state = STATE_IDLE;
            }
            else
            {
                if (WiFi.hasCredentials() && !(WiFi.ready()))
                {
                    Log.info("WiFi credentials available, connecting to WiFi");
                    wasBLEinformed = false; //V101
                    WiFi.connect();
                    waitFor(WiFi.ready, 20000);
                    if (WiFi.ready())
                    {
                        WiFi.listen(false); //stop listening V105
                        cloudConnected();
                        next_provision_state = STATE_IDLE;
                    }
                }
                else if (WiFi.ready() && !(Particle.connected()))
                {
                    Log.info("WiFi but not Cloud connected");
                    wasBLEinformed = false; //V101
                    cloudConnected();
                    next_provision_state = STATE_IDLE;
                }
                else
                {
                    Log.info("WiFi and Cloud connected");
                    wasBLEinformed = false; //V101
                    next_provision_state = STATE_IDLE;
                }
            }
            break;
        }
    }
    if (provision_state != next_provision_state) provision_state = next_provision_state;
}

// Function to handle waiting for Particle connected
void cloudConnected()
{
    Particle.connect();
    waitFor(Particle.connected, 20000);
    if (Particle.connected())
    {
        connectedStatus = 1;
    }
}

// send event DEUP to indicate that the device was connected/disconnected to BLE Central for WiFi provisioning V071
void sendBLEProvisioningEvent(bool isConnected)
{
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("BTC").value(isConnected?1:0); // 1 = connected, 0 = disconnected
    writer.endObject();
    PublishQueuePosix::instance().publish(eventvarchanged, dataStr, 50, PRIVATE);
}

// entry from: runState has been set to D_RESTART by a web command
// function  : wait to trigger system.reset until ready
// exit to   : System.reset()
void restartController()
{
    if (!timerReset.isActive() && !isResetTimeout)
    {
        timerReset.start();
        isResetTimeout = false;
    }

    if (isResetTimeout)
    {
        //note: other restartdata will have been set in the web command handler
        restartdata.powerOnState = D_STANDBY;   //this should be a configuration parameter 
        for (int i = 0; i < (int) (sizeof(P2_PDU_RELAYS) / sizeof(P2_PDU_RELAYS[0])); i++) {restartdata.relayState[i] = (uint8_t) (digitalRead(P2_PDU_RELAYS[i])?1:0);}
        if (param.hubBoard == 1) restartdata.isHubBoard = true; //V076
        else                     restartdata.isHubBoard = false;
        saveRestartDataToRam();

        System.reset();
    }
}

// entry from: runState = D_GOTOSTANDBY (User command or web command, initial startup or system control or resume if it was D_STANDBY)
// function  : 
// exit to   : runState = D_STANDBY
void goToStandbyController()
{
    System.enableUpdates();             //re-enable firmware updates when in D_STANDBY 

    if (param.isOUCMonitoring)          //if background monitoring for OUC start true 
    {
        if (param.hubBoard == 0)    oucState = ON_UNTIL_AUTO;   //no hub board fitted Smart AC charge V111
        else                        oucState = ON_UNTIL_USBC;   //hub board fitted Smart USBC charge V111
    }
    else                               //else set to off                 
    {
        oucState = ON_UNTIL_OFF;
    }

    if (powerState == W_MAINS_OFF)
    {
        runState = D_GOTOSLEEP;
    }
    else
    {
        if (param.resumeCause == RESUME_MAINS_OFF)
        {
            mainsPowerRestoredEvent(); // log mains power restored event if mains was off before reset V092
        }

        chargeState = C_NOT_CHARGING;                       //set charge state to not charging
        param.resumeMinutes = -1;
        param.resumeCause = 0;                              //reset resume cause V092
        putParameters();
        runState = D_STANDBY;
    }
}

// entry from: runState = D_STANDBY after D_GOTOSTANDBY
// function  : check if Mains Off or overheated
// exit to   : runState = D_GOTOSLEEP if mains off or stay in D_STANDBY if overheated but set flag overheated to prevent relays from being turned on
void standbyController()
{
    powerState = powerStateCheck();                  //check power supply
    if (powerState == W_MAINS_ON)                    //only when mains powered
    {
        //runState = D_STANDBY;                      //stay in standby V124
        prevRunState = runState;                     //save previous run state for resume V124
        checkOverheated();
        if (isOverheated && !wasOverheated)          //if overheated then set flag
        {
            wasOverheated = true;                       //set flag to true not acted upon
            // do something here, go to sleep etc. relevant to the run state
        }
        
        if (!isOverheated && wasOverheated)          //if not overheated then reset flag
        {
            wasOverheated = false;                      //reset flag to false
            // do something here, resume etc. relevant to the run state
        }
    }
    else if (powerState == W_MAINS_OFF) 
    {
        mainsPowerOffEvent(); 
        param.resumeState = D_STANDBY;              //save current run state for resume V123
        param.resumeCause = RESUME_MAINS_OFF;
        param.resumeMinutes = -1;
        putParameters();
        prevRunState = runState;                    //save previous run state for resume
        runState = D_GOTOSLEEP;
    }
    else                                            //this is an error condition as neither MAINS_ON or OFF
    {
        snprintf(dataStr, MAXDATA, "{\"CX\":\"Error Power %i Run %i charge %i prevrun %i ouc %i mins %i trace %i\"}", powerState, runState, chargeState, prevRunState, oucState, chargeMins, trace);
        PublishQueuePosix::instance().publish(eventdiagnostic, dataStr, 50, PRIVATE);
        powerState = W_MAINS_ON;
        chargeState = C_NOT_CHARGING;
    }
}

// helper to determine whether overheated or not and when recovered using the board temperature and external sensors if enabled
void checkOverheated()
{
    float maxtemp = boardTemp; // default to board temperature

    #if EXT_TEMP_SENSOR
    maxtemp = max(boardTemp, xtemp); // take the maximum of the two sensors
    #endif // EXT_TEMP_SENSOR

    if(!isOverheated && (maxtemp >= (float)(param.maxTemp))) //has become overheated
    {
        isOverheated = true;                  //set
        wasOverheated = false;                //reset - set when overheated acted upon
    }
    else if (isOverheated && (maxtemp < (float)(param.maxTemp - 10))) //overheated condition cleared
    {
        isOverheated = false;                 //reset
        wasOverheated = true;                 //set - reset when !overheated acted upon
    }
}

// entry from: runState = D_GOTOTIMED_ON (User command or web command)
// function  : prepare screen and timer for Timed On
// exit to   : runState = D_TIMED_ON
void goToTimedOnController()
{
    System.disableUpdates();                                //disable firmware updates when not in D_STANDBY
    powerState = powerStateCheck();                         //check power supply V104 
    Log.info("goToTimedOnController powerState %i", powerState);
    if (powerState == W_MAINS_ON)
    {
        initTimers();                                       //this sets timerMins and chargeMins = 0

        timerMins = param.resumeMinutes;
        if (timerMins <= 0)                                 //if resumeMinutes is -1 then has not been set therefore start timer at 0
        {
            timerMins = 0;
        }

        timerStartMins = param.timerPeriod;
        chargingDevices = -1;                               //means web app will display "not measured"
        mainsofftime = 0UL;                                 //reset the time the mains power is lost
        helperDelayDRUP();                                  //V116
        runState = D_TIMED_ON;
    }
    else
    {
        runState = D_GOTOSTANDBY;
    }
}

// entry from: runState = D_TIMED_ON after D_GOTOTIMED_ON (web command D_WEBGOTOTIMED)
// function  : timed on state logic function 
// exit to   : runState = D_GOTOSTANDBY (when timerCountdown ends)
void timedOnController()
{
    powerState = powerStateCheck();
    if (runState == D_TIMED_ON)                             //avoid this code if user has signalled exit to standby
    {
        checkOverheated();

        helperCheckFirstDRUP();                             //V116

        if (isUpdateCountdown)                              //minute timer has ended - check if countdown has ended
        {
            timerMins++;                                    //increment the timer minutes
            isUpdateCountdown = false;                      //reset timerCountdown flag
            if (timerStartMins-timerMins <= 0)              //if countdown has ended (reached zero)
            {
                istimerCountdown_ended = true;              //set flag to true
                stoptimerCountdown();                       //stop the countdown timer
            }
            else                                            //countdown not yet ended
            {
                istimerCountdown_ended = false;             //set flag to false
            }
        }
        if (istimerCountdown_ended)                         //countdown has ended
        {
            param.resumeMinutes = -1;
            param.resumeCause = 0;
            ACRelaysOff(R_TIMED);                           //turn off relays and send event
            runState = D_GOTOSTANDBY;
        }
        else if (!timerCountdown.isActive() && powerState != W_MAINS_OFF && !isOverheated)   //Mains Power and suspended/not yet started/not overheated
        {
            int context = R_TIMED;                          //context for relays
            starttimerCountdown();                          //does not set timerMins
            if (param.resumeMinutes >= 0)                   //only if resume condition to timing [amended to include 0 with -1 no resume]
            {
                Log.info("Timed resume condition to timing %i", param.resumeMinutes);
                switch (param.resumeCause)
                {
                    case RESUME_OUT_OF_MEMORY:
                    case RESUME_WATCHDOG:
                        break;
                    case RESUME_MAINS_OFF:
                        mainsofftime = 0;                   //reset the time the mains power is lost
                        mainsPowerRestoredEvent();
                        break;
                    case RESUME_OVERHEAT:
                        if (wasOverheated) wasOverheated = false;   //overheated condition ended
                        break;
                    default:
                        break;
                }
                context = X_TIMED;
                helperCleanUpResumeData();
            }
            isChargingStarted = true;                       //indicate charging has started to reset the update event timer V108    
            ACRelaysOn(context);                            //turn off relays and send event
        }

        if (powerState == W_MAINS_OFF && mainsofftime == 0) //mains removed and not on suspend then suspend charging and stop countdown
        {
            Log.info("Timed Mains Power Off Detected");
            mainsPowerOffEvent();
            helperTimedMainsOffOverheated();
            param.resumeCause = RESUME_MAINS_OFF;
            param.resumeMinutes = timerMins;                //save the current charge time to eeprom
            param.resumeState = D_TIMED_ON;                 //added so that if mains off then after a period it will sleep and may go off if no battery power left
            putParameters();
            mainsofftime = millis();                        //set the time the mains power is lost
        }
        else if (isOverheated && !wasOverheated)            //has just become overheated
        {
            Log.info("Timed Overheated Condition");
            wasOverheated = true;
            param.resumeCause = RESUME_OVERHEAT;
            param.resumeMinutes = timerMins;                //save the current charge time to eeprom
            putParameters();
            helperTimedMainsOffOverheated();
        }
        else if (powerState == W_MAINS_OFF && ((millis() - mainsofftime) > GOTOSLEEPDELAY))
        {
            Log.info("Timed Mains Power Off Detected and GOTOSLEEPDELAY reached");
            runState = D_GOTOSLEEP;
        }
    }
}

// helper for Timed On for for Mains Off or Overheated
void helperTimedMainsOffOverheated()
{
    stoptimerCountdown();                                   //stop the countdown timer
    istimerCountdown_ended = false;                         //to avoid next loop doing a normal countdown ended
    ACRelaysOff(X_TIMED);                                   //turn off relays and send event related to mains off
}

// entry from: runState = D_GOTOAUTO
// function  : transition to auto control
// exit to   : runState = D_AUTO_OFF
void goToAutoController()
{
    if (prevRunState != AUTOANDOFF) prevRunState = D_GOTOAUTO; 
    chargingDevices = -1; 
    mainsofftime = 0UL;                                     //reset the time the mains power is lost
    Log.info("Go to Auto Controller");
    runState = D_AUTO_OFF;
}

// entry from: runState = D_AUTO_ON or D_AUTO_OFF (following D_GOTOAUTO) web command
// function  : schedule (auto on or auto off) logic function
// exit to   : runState = D_GOTOSTANDBY (when schedule expires or mains off) or D_AUTO_ON (when auto on) or D_AUTO_OFF (when auto off)
void autoController()
{
    powerState = powerStateCheck();                         //check power supply

    uint8_t block, bit;
    nextDay = 0;                                            //default to blank space
    if (Time.isValid())                                     //auto can only work with a valid time
    {
        isAutoTimeInValidSent = false;
        if (hasScheduleExpired())                           //schedule has expired
        {
            ACRelaysOff(R_NONE);                            //turn off relays and send event
            memset(dataStr, 0, sizeof(dataStr));
            JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("R").value(W_STANDBY);
            writer.name("CX").value("Schedule Expired go to standby");
            writer.endObject();
            PublishQueuePosix::instance().publish(eventschedulexp, dataStr, 50, PRIVATE);
            runState = D_GOTOSTANDBY;
        }
        else
        {
            checkOverheated();
            int i = 6*(Time.weekday()-1)+(Time.hour()/4);
            block = param.schedule[i];                          //find the byte in the schedule array for the weekday and 4 hour block
            int j = 7-(2*(Time.hour()%4)+(Time.minute()>29?1:0));
            bit = (block>>j);                                   //shift right to move the bit corresponding to the 1/2 hour reqd to LSB
            if ((bit & 0x01) == 0x01)                           //schedule is for ON and
            {
                if (powerState == W_MAINS_ON && !isOverheated)  //Mains Power and suspended/not yet started/not overheated
                {
                    Log.info("Auto Schedule On AND Mains Power On AND not overheated");
                    int context = R_AUTO;
                    if (param.resumeMinutes >= 0)               //only if resume condition to On [amended to include 0 with -1 no resume]
                    {
                        Log.info("Auto On resume condition to timing %i", param.resumeMinutes);
                        switch (param.resumeCause)
                        {
                            case RESUME_OUT_OF_MEMORY:
                            case RESUME_WATCHDOG:
                                break;
                            case RESUME_MAINS_OFF:
                                mainsofftime = 0;               //reset the time the mains power is lost
                                mainsPowerRestoredEvent();
                                break;
                            case RESUME_OVERHEAT:
                                if (wasOverheated) wasOverheated = false;   //overheated condition ended
                                break;
                            default:
                                break;
                        }
                        context = X_AUTO;
                        runState = D_AUTO_ON;
                        chargeMins = param.resumeMinutes;       //set minutes counted to value at reset
                        helperCleanUpResumeData();
                    }
                    else                                        //first time start
                    {
                        Log.info("Auto On first time start");
                        chargeMins = 0;
                        runState = D_AUTO_ON;
                    }
                    starttimerCharging();                       //start charge timer
                    isChargingStarted = true;                   //indicate charging has started to reset the update event timer V108    
                    ACRelaysOn(context);                        //turn off relays and send event
                }
                else if (isOverheated && !wasOverheated)        //has just become overheated
                {
                    Log.info("Auto On Overheated Condition");
                    helperAutoOverheated();
                }

                if (powerState == W_MAINS_OFF && mainsofftime == 0)   //mains removed and not countdown to sleep
                {
                    Log.info("Auto On Mains Power Off Detected");
                    helperAutoMainsOff();
                }
                else if (powerState == W_MAINS_OFF && ((millis() - mainsofftime) > GOTOSLEEPDELAY))
                {
                    Log.info("Auto On Mains Power Off Detected and GOTOSLEEPDELAY reached");
                    param.resumeState = D_AUTO_ON;              //added so that if mains off then after a period it will sleep and may go off if no battery power
                    param.resumeMinutes = chargeMins;
                    putParameters();
                    runState = D_GOTOSLEEP;
                }

                helperCheckFirstDRUP();                         //V131

                if (isUpdateTimer)
                {
                    chargeMins++;
                    isUpdateTimer = false;
                }
            }
            else if ((bit & 0x01) == 0x00)                      //schedule is for OFF and
            {
                if (powerState == W_CHARGING)                   //and currently relay is ON then switch OFF 
                {
                    Log.info("Auto Schedule Off end charging");
                    runState = D_AUTO_OFF;
                    stoptimerCharging(); 
                    ACRelaysOff(R_AUTO);                        //turn off relays and send event
                    prevRunState = runState;
                }
                else if (powerState == W_MAINS_ON && !isOverheated) //Mains Power and suspended/not yet started/not overheated
                {
                    if (param.resumeMinutes >= 0)               //only if resume condition to Auto Charge [amended to include 0 & -1 non resume]
                    {
                        Log.info("Auto Schedule Off AND Mains Power On AND not overheated resume condition to timing %i %i", param.resumeMinutes, param.resumeCause);
                        switch (param.resumeCause)
                        {
                            case RESUME_OUT_OF_MEMORY:
                            case RESUME_WATCHDOG:
                                break;
                            case RESUME_MAINS_OFF:
                                mainsofftime = 0;               //reset the time the mains power is lost
                                mainsPowerRestoredEvent();
                                break;
                            case RESUME_OVERHEAT:
                                if (wasOverheated) wasOverheated = false;   //overheated condition ended
                                break;
                            default:
                                break;
                        }
                        runState = D_AUTO_OFF;
                        helperCleanUpResumeData(); 
                    }
                    else                                        //whilst schedule is off/mains on and not overheated loop will go thru here every time
                    {
                        chargeMins = 0;
                        runState = D_AUTO_OFF;
                    }
                }

                if (isOverheated && !wasOverheated)             //has just become overheated
                {
                    Log.info("Auto Off Overheated Condition");
                    helperAutoOverheated();
                }
                else if (powerState == W_MAINS_OFF && mainsofftime == 0)  //mains removed and not on suspend
                {
                    Log.info("Auto Off Mains Power Off Detected");
                    helperAutoMainsOff();
                }
                else if (powerState == W_MAINS_OFF && ((millis() - mainsofftime) > GOTOSLEEPDELAY)) 
                {
                    Log.info("Auto Off Mains Power Off Detected and GOTOSLEEPDELAY reached");
                    param.resumeState = D_AUTO_OFF;             //added so that if mains off then after a period it will sleep and may go off if no battery power left
                    putParameters();
                    runState = D_GOTOSLEEP;
                }
                else if (prevRunState == D_GOTOAUTO)            //update the schedule screen now next On time available - just once
                {
                    prevRunState = 0;
                }
                else if (prevRunState == AUTOANDOFF)            //to ensure Schedule Off event sent after a Web Auto Command and Schedule is off 
                {
                    ACRelaysOff(AUTOANDOFF);                    //turn off relays and send event
                    prevRunState = 0;
                }
            }
        }
    }
    else
    {
        if (!isAutoTimeInValidSent)                             //just do once
        {
            ACRelaysOff(T_AUTO);                                //turn off relays and send event
            isAutoTimeInValidSent = true;
            memset(dataStr, 0, sizeof(dataStr));
            JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("R").value(W_STANDBY);
            writer.name("CX").value("Suspended Time Invalid");
            writer.endObject();
            PublishQueuePosix::instance().publish(eventscheduloff, dataStr, 50, PRIVATE);
        }
    }
}

// helper Auto On or Off Mains Off
void helperAutoMainsOff()
{
    param.resumeCause = RESUME_MAINS_OFF;
    param.resumeMinutes = chargeMins;                           //save the current charge time to eeprom
    param.resumeState = D_AUTO_ON;                              //added so that if mains off then after a period it will sleep and may go off if no battery power left
    putParameters();

    mainsPowerOffEvent(); 
    helperAutoMainsOffOverheated();
    mainsofftime = millis();                                    //set the time the mains power is lost
}

// helper Auto On or Off Overheated
void helperAutoOverheated()
{
    wasOverheated = true;
    param.resumeCause = RESUME_OVERHEAT;
    param.resumeMinutes = chargeMins;                           //save the current charge time to eeprom
    param.resumeState = D_AUTO_ON;                              //added so that if mains off then after a period it will sleep and may go off if no battery power left
    putParameters();

    helperAutoMainsOffOverheated();
    //runState = D_GOTOSTANDBY;
}

// helper Auto for Mains Off and Overheated
void helperAutoMainsOffOverheated()
{
    stoptimerCharging();                                   //stop the charging timer
    if (wasOverheated) 
    {
        param.isAuto = false; 
    }
    else
    {
        param.resumeMinutes = chargeMins;           //save the current charge time to eeprom
        param.resumeState = D_AUTO_OFF;             //added so that if mains off then after a period it will sleep and may go off if no battery power left
    }
    putParameters();
    
    ACRelaysOff(X_AUTO);                            //turn off relays and send event
    //delay(2s);
}

// helper to return true if schedule has expired
bool hasScheduleExpired()
{
    bool _result = false;
    if (Time.isValid()) 
    {
        int _y = Time.year();
        int _m = Time.month();
        int _d = Time.day();
        if (_y > param.valid2year)                    //last year
        {
            _result = true;
        }
        else if (_y == param.valid2year)              //same year 
        {
            if (_m > param.valid2month) //then check month
            {
                _result = true;
            }
            else if (_m == param.valid2month && _d > param.valid2day)       //same year and month then check day
            {
                _result = true;
            }    // else if same year and month and day then not expired as time to expire is 23:59:59
        }
    }
    else    //if time is invalid then have to assume scheduleExpired
    {
        _result = true;
    }
    return _result;
}

// helper called when charging started (ACRelaysOn) to set flag and initial update time V116
void helperDelayDRUP()
{
        isChargingJustStarted = false; //V109
        charginginitialupdate = millis(); //set the initial update time V109/V131
}

// helper to check for first DRUP event after charging started V116
void helperCheckFirstDRUP()
{
    if (charginginitialupdate > 0 && (millis() - charginginitialupdate >= DELAY_INITIAL_DRUP)) //V109
    {
        charginginitialupdate = 0;      //only do this once
        isChargingJustStarted = true;   //flag to indicate initial charging period has ended and DRUP should be sent
    }
}

// entry from: runState = D_GOTOALWAYS_ON
// function  : prepare screen and sends events for and switches on charging for AVCONTROL2 this overrides the web control and schedules and put on all sockets V410
// exit to   : runState = D_ALWAYS_ON
void goToOnController()
{
    System.disableUpdates();    //disable firmware updates when not in D_STANDBY

    powerState = powerStateCheck();                     //check power supply
    Log.info("Go to On Controller powerState %i", powerState);
    if (powerState == W_MAINS_ON)
    {
        initTimers();                                   //this sets timerMins and chargeMins = 0
        chargeMins = param.resumeMinutes; 
        if (chargeMins <= 0)   chargeMins = 0;
        chargingDevices = -1;                           //means web app will display "not measured"
        helperDelayDRUP();                              //V116
        runState = D_ALWAYS_ON;
    }
}

// entry from: runState = D_ALWAYS_ON
// function  : monitor for timer updates every minute to check if maximum time reached
// exit to   : runState = various if web command
void onController()
{
    powerState = powerStateCheck();                         //check power supply
    if (runState == D_ALWAYS_ON)                            //avoid this code if user has signalled exit to standby
    {
        checkOverheated();
        prevRunState = runState;                            //have done continuous on at least once

        helperCheckFirstDRUP();                             //V116

        if (isUpdateTimer)                                  //one minute timer ended
        {
            chargeMins++;
            isUpdateTimer = false;
            /*                                          V099 removed as not required for continuous on
            Log.info("Charge Mins %i Max Mins %i", chargeMins, param.maxTimeOn);
            if (param.maxTimeOn > 0 && chargeMins >= param.maxTimeOn)     //if maximum time limit set and reached
            {
                istimerCountdown_ended = true;          //set flag to true
                stoptimerCharging();                    //stop the charging timer
            }
            else                                        //countdown not yet ended
            {
                istimerCountdown_ended = false;         //set flag to false
            }
            */
            istimerCountdown_ended = false;             //set flag to false V099
        }

        if (istimerCountdown_ended)                         //maximum time on charge reached
        {
            ACRelaysOff(M_ONC);                             //turn off relays and send event
            runState = D_GOTOSTANDBY;
        }
        else if (!timerCharging.isActive() && powerState != W_MAINS_OFF && !isOverheated) //Mains Power and suspended/not yet started
        {
            Log.info("onController timer inactive Mains Power On AND not overheated");
            starttimerCharging();                       //sets chargeMins = 0
            int context = R_ONC;                        //normal start
            if (param.resumeMinutes >= 0)               //only if resume condition to On [amended to include 0 & -1 = no resume]
            {
                Log.info("Resume condition to timing %i", param.resumeMinutes);
                switch (param.resumeCause)
                {
                    case RESUME_OUT_OF_MEMORY:
                    case RESUME_WATCHDOG:
                        break;
                    case RESUME_MAINS_OFF:
                        mainsPowerRestoredEvent();
                        break;
                    case RESUME_OVERHEAT:
                        if (wasOverheated) wasOverheated = false;   //overheated condition ended
                        break;
                    default:
                        break;
                }
                context = X_ONC;                        //resume start
                chargeMins = param.resumeMinutes;       //set minutes counted to value at suspend
                helperCleanUpResumeData();
            }
            else                                        //first time start
            {
                chargeMins = 0;
            }
            isChargingStarted = true;                   //indicate charging has started to reset the update event timer V108    
            ACRelaysOn(context);                        //turn on relays and send event V91M
        }

        if (powerState == W_MAINS_OFF && param.resumeMinutes < 0) //mains removed and not on suspend then suspend charging and stop countdown
        {
            Log.info("onController Mains Power Off Detected");
            mainsPowerOffEvent();
            helperOnMainsOffOverheated();
            param.resumeMinutes = chargeMins;               //save the current charge time to eeprom
            param.resumeCause = RESUME_MAINS_OFF;
            param.resumeState = D_ALWAYS_ON;                //added so that if mains off then after a period it will sleep and may go off if no battery power left
            putParameters();
            mainsofftime = millis();                        //set the time the mains power is lost
        }
        else if (isOverheated && !wasOverheated)            //has just become overheated
        {
            Log.info("onController Overheated Condition");
            param.resumeCause = RESUME_OVERHEAT;
            wasOverheated = true;
            param.resumeMinutes = chargeMins;               //save the current charge time not required to eeprom
            helperOnMainsOffOverheated();
        }
        else if (powerState == W_MAINS_OFF && ((millis() - mainsofftime) > GOTOSLEEPDELAY))
        {
            Log.info("onController Mains Power Off Detected and GOTOSLEEPDELAY reached");
            runState = D_GOTOSLEEP;
        }
    }
}

// helper Continuous On for Mains Off or Overheated
void helperOnMainsOffOverheated()
{
    stoptimerCharging();                                    //stop the charging timer
    ACRelaysOff(X_ONC);                                     //turn off relays and send event related to mains off
}

// entry from: runState = D_GOTOCHARGED_ON
// function  : prepare for on until charged cycle Web or Local initiated
// exit to   : runState = D_CHARGED_ON or D_GOTOSTANDBY if no mains power
void goToChargedOnController()
{
    goToChargedOnAutoController();
}

// entry from: runState = D_GOTOCHARGED_ON_AUTO
// function  : prepare for Smart Charge or Smart Charge Auto cycle start 
// exit to   : runState = D_CHARGED_ON_AUTO if OK or D_GOTOSTANDBY
void goToChargedOnAutoController()
{
    powerState = powerStateCheck();                                             //check power supply V123
    if (powerState == W_MAINS_ON || powerState == W_CHARGING)                   //correction to add powerState = W_CHARGING as now handling other than just standby/mains on
    {
        if (runState == D_GOTOCHARGED_ON_AUTO)
        {
            switch (prevRunState)                                               //manage transitions from previous runState to standby
            {
                case D_STANDBY:
                    break;
                case D_ALWAYS_ON:
                    stoptimerCharging();
                    ACRelaysOff(Z_ONC);
                    break;
                case D_TIMED_ON:
                    stoptimerCountdown();
                    ACRelaysOff(Z_TIMED);
                    break;
                case D_CHARGED_ON:
                    stoptimerCharging();
                    ACRelaysOff(Z_ONTIL);
                    break;
                default:
                    break;
            }

            switch (runState)
            {
            case D_GOTOCHARGED_ON_AUTO:
                helperGoToCharged(R_ONTIL);
                runState = D_CHARGED_ON_AUTO;
                break;
            default:
                break;
            }
            prevRunState = runState;            
        }
        else                                                                    //CHARGED_ON
        {
            helperGoToCharged(R_ONTIL);
            if (runState == D_GOTOCHARGED_ON) runState = D_CHARGED_ON;          //traps the condition that the web command standby has been sent during smart charge startup
        }
    }
    else
    {
        if (runState == D_GOTOCHARGED_ON)
        {
            runState = D_GOTOSTANDBY;
        }
        else
        {
            runState = prevRunState;                                            //go back to previous state and handle the MAINS_OFF there, userState unchanged
        }
    }
}

// helper to prepare for on until charge cycle
void helperGoToCharged(int context)
{
    chargeMins = 0;
    chargeState = C_CHARGING;                       //chargeState starts as charging V119
    chargingDevices = -1;
    totalAdaptors = 0;
    oucState = ON_UNTIL_SS;
    isChargingStarted = true;                       //indicate charging has started to reset the update event timer V108    
    ACRelaysOn(context);
    helperDelayDRUP();                              //V116
    starttimerCharging();
    oucState = ON_UNTIL_WARM;
}

// entry from: runState = D_CHARGED_ON or D_CHARGED_ON_AUTO or D_AUTO_ON ?? 
// function  : monitor for user action or smart AC devices fully charged - charge period suspends on mains off and resumes when on, maximum on charge period overrides charge status
// exit to   : runState = D_GOTOSTANDBY if web command
void chargedOnController()
{
    powerState = powerStateCheck();                                              //check power supply
    if (runState == D_CHARGED_ON || runState == D_CHARGED_ON_AUTO || runState == D_AUTO_ON)     //avoid this code if user has signalled exit to standby
    {
        helperCheckFirstDRUP();                                                 //V116

        if (isUpdateTimer)                                                      //one minute timer ended
        {
            chargeMins++;
            Log.info("smart AC charge minute timer update chargeMins %i", chargeMins);
            isUpdateTimer = false;
        }

        if ((oucState == ON_UNTIL_CHARGE) && (chargeMins >= param.maxTimeOn))   //charging and maximum time on charge reached => stop
        {
            chargeState = C_CHARGING_ENDED;                                    //set chargeState to ended V130
            Log.info("smart AC charge maximum time on charge reached %i mins ChargeState %i", chargeMins, chargeState);
            wasSmartChargeEndedThisHalfHour = true;
            stoptimerCharging();
            ACRelaysOff(M_ONTIL);                                               //turn off relays and send event
            putParameters(); 
            runState = D_GOTOSTANDBY;                                           //max time reached switching off
        }
        else if (oucState == ON_UNTIL_OFF)                                      //web 'aux' command received
        {
            Log.info("smart AC charge web command to go to standby received");
            stoptimerCharging();                                                //stop the charging timer   
            ACRelaysOff(W_ONTIL);                                               //turn off relays and send event
            runState = D_GOTOSTANDBY;                                           //standby
        }
        else if (oucState == ON_UNTIL_WARM && chargeMins >= param.warmupMins)   //warmup period has ended
        {
            oucState = ON_UNTIL_CHARGE;
            chargeState = C_CHARGING_FULL;                                      //set chargeState to full charge V130
            iStep = 0;
            chargingDevices = 0;
            sampleCurrent();
            currentupdate = millis();
            param.numDevices = numberOfAdaptors();                             //get number of devices connected V119
            Log.info("smart AC charge WARMUP ended after %i minutes ChargeState %i CAmps %f", chargeMins, chargeState, powerdata.ampsrms);
            helperSendFullRateChargingEvent();
        }
        else                                                                    //maximum time not exceeded or suspended/not yet started
        {
            if (!timerCharging.isActive() && powerState != W_MAINS_OFF && !isOverheated && oucState == ON_UNTIL_CHARGE) //Mains Power and suspended/not yet started
            {
                starttimerCharging();                                           //sets chargeMins = 0
                int context = R_ONTIL;                                          //regular start
                if (param.resumeMinutes >= 0)                                   //only if resume condition to On
                {
                    if (wasOverheated)                                          //overheated condition ended
                    {
                        wasOverheated = false;
                        context = X_ONTIL;
                        chargeMins = param.resumeMinutes;                       //set minutes counted to value at reset
                        helperCleanUpResumeData();
                    }
                    else                                                        //mains restored (even if a short time off)
                    {
                        mainsPowerRestoredEvent();                              //send mains restored event
                        if (param.resumeState == D_CHARGED_ON_AUTO) runState = D_GOTOCHARGED_ON_AUTO;       //restart the smart charge auto cycle
                        else                                        runState = D_GOTOCHARGED_ON;            //restart the smart charge cycle
                        helperCleanUpResumeData();
                        return;                                                 //get out of the function
                    }
                }
                ACRelaysOn(context);                                            //turn on relays and send event according to context
            }
        }

        if (runState != D_WEBGOTOSTANDBY)                                       //only do the rest if there has been no web command to go to standby during soft start
        {
            if (powerState == W_MAINS_OFF && param.resumeMinutes < 0)           //mains removed and not on suspend then suspend charging and stop timer
            {
                mainsPowerOffEvent();
                helperSmartMainsOffOverheated(X_ONTIL);
                mainsofftime = millis();                                        //set the time the mains power is lost
            }
            else if (isOverheated && !wasOverheated)                            //has just become overheated
            {
                wasOverheated = true;
                helperSmartMainsOffOverheated(X_ONTIL);
                runState = D_GOTOSTANDBY;
            }
            else if (powerState == W_MAINS_OFF && ((millis() - mainsofftime) > GOTOSLEEPDELAY)) //mains off for delay period
            {
                prevRunState = runState;                                        //V124
                param.resumeCause = RESUME_MAINS_OFF;                           //save the current charge time to eeprom
                param.resumeState = runState;                                   //added so that if mains off then after a period it will sleep and may go off if no battery power left
                param.resumeMinutes = 0;                                        //smart charge and auto smart will always need to restart the cycle
                putParameters();
                runState = D_GOTOSLEEP;
            }
            else if (powerState != W_MAINS_OFF && !isOverheated)                //only do this checking if not mains off AND not overheated
            {
                if (oucState == ON_UNTIL_CHARGE)
                {
                    if (millis() - currentupdate > CCCHK)                       //time to check the charging current
                    {
                        int lastChargeState = chargeState;                      //get last charge state for comparison V103
                        sampleCurrent();                                        //sample current and updates amps global variable
                        Log.info("determineChargeState CAmps %f", powerdata.ampsrms); 
                        chargeState = determineChargeState(chargeState, cSlope, powerdata.ampsrms, param.numDevices, chargeMins);     //set chargeState value
                        if (chargeState == C_RATE_CHARGING) //V130
                        {
                            if (dataPoints == 0)                                //first entry after slope monitoring started
                            {
                                ampsY[0] = powerdata.ampsrms;
                                timeX[0] = 1.0;
                                dataPoints++;
                                ptrNow = 1;
                            }
                            else
                            {
                                int prevPtr;
                                if (ptrNow == 0) prevPtr = 9;
                                else prevPtr = ptrNow-1;
                                ampsY[ptrNow] = powerdata.ampsrms;
                                timeX[ptrNow] = timeX[prevPtr] + CSCHK;
                                dataPoints < 10 ? dataPoints++ : dataPoints = 10;           //increment up to 10
                                ptrNow < 9 ? ptrNow++ : ptrNow = 0;                         //wrap data in circular buffer
                            }
                            cSlope = linearRegression(dataPoints, timeX, ampsY);//calculate slope of charging vs time - take the absolute value 
                        }

                        if ((lastChargeState != chargeState) && (chargeState == C_RATE_CHARGING || chargeState == C_CHARGING_DONE))  //on charge V119/V130
                        {
                            Log.info("Charge State Changed %i to %i", lastChargeState, chargeState);
                            if (chargeState == C_RATE_CHARGING) 
                            {
                                helperSendRateMonitoringChargingEvent();
                            }
                            else if (chargeState == C_CHARGING_DONE) 
                            {
                                helperSendChargingDoneEvent();
                            }
                        }
                        currentupdate = millis();
                    }
                }

                if (chargeState == C_CHARGING_ENDED)                //fully charged, now checked after 
                {
                    oucState = ON_UNTIL_END;
                    stoptimerCharging();                            //stop charge on timer
                    ACRelaysOff(R_ONTIL);                           //turn off relays and send event
                    isTriedAutoOUConce = false;                     //if ends normally (fully charged) and not Auto OuC Monitoring
                    if (param.isOUCMonitoring)
                    {
                        isTriedAutoOUConce = true;                  //if Auto OuC Monitoring and ends normally then stop restart
                        wasSmartChargeEndedThisHalfHour = true;
                    }
                    runState = D_GOTOSTANDBY;                       //really should send a special event for this - devices charged
                }
            }
        }
    } //end if runState == D_CHARGED_ON or D_CHARGED_ON_AUTO or D_AUTO_ON
}

// helper to send full rate charging event V130
void helperSendFullRateChargingEvent()
{
    Log.info("helperSendFullRateChargingEvent");
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Full rate Charging Smart AC"); //V130
    writer.name("R").value(runStateInt);
    writer.name("LA").value((double) powerdata.ampsrms,3);
    writer.name("LV").value((double) powerdata.voltsrms,3);
    writer.name("KL").value(chargeState);   //V130
    writer.endObject();
    PublishQueuePosix::instance().publish(eventvarchanged,dataStr, 50, PRIVATE);        //send smart charge data
}

// helper to send rate monitoring charging event V130
void helperSendRateMonitoringChargingEvent()
{
    Log.info("helperSendRateMonitoringChargingEvent");
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Rate monitoring started"); //V130
    writer.name("R").value(runStateInt);
    writer.name("LA").value((double) powerdata.ampsrms,3);
    writer.name("LV").value((double) powerdata.voltsrms,3);
    writer.name("KL").value(chargeState);   //V130
    writer.endObject();
    PublishQueuePosix::instance().publish(eventvarchanged,dataStr, 50, PRIVATE);        //send smart charge data
}

// helper to send charging done event V130
void helperSendChargingDoneEvent()
{
    Log.info("helperSendChargingDoneEvent");
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Rate Monitoring Done"); //V130
    writer.name("R").value(runStateInt);
    writer.name("LA").value((double) powerdata.ampsrms,3);
    writer.name("LV").value((double) powerdata.voltsrms,3);
    writer.name("KL").value(chargeState);   //V130
    writer.endObject();
    PublishQueuePosix::instance().publish(eventvarchanged,dataStr, 50, PRIVATE);        //send smart charge data
}

// helper Smart AC/USBC Charging for Mains Off or Overheated
void helperSmartMainsOffOverheated(int _context)
{
    stoptimerCharging();                            //stop the charging timer
    if (isOverheated) 
    {
        param.isAuto = false;
    }
    else
    {
        param.resumeMinutes = chargeMins;           //save the current charge time
        param.resumeState = runState;               //added so that if mains off then after a period it will sleep and may go off if no battery power left
    }
    putParameters();

    ACRelaysOff(_context);                          //turn off relays and send event related to mains off or overheated
}

#if LVSUNCHARGER
// entry from: runState = D_GOTOCHARGED_ON_USBC
// function  : prepare for on until charged cycle USBC with Hub
// exit to   : runState = D_CHARGED_ON_USBC or D_GOTOSTANDBY if no mains power
void goToChargedOnUSBCController()
{
    goToChargedOnUSBCAutoController();
}

// entry from: runState = D_GOTOCHARGED_ON_USBC_AUTO
// function  : prepare for Smart Charge USBC or Smart Charge USBC Auto 
// exit to   : runState = D_CHARGED_ON_USBC_AUTO, D_CHARGED_ON_USBC or D_GOTOSTANDBY
void goToChargedOnUSBCAutoController()
{
    powerState = powerStateCheck();                                             //check power supply V104
    if (powerState == W_MAINS_ON || powerState == W_CHARGING)                   //correction to add powerState = W_CHARGING as now handling other than just standby/mains on
    {
        Log.info("goToChargedOnUSBCAutoController entry runState %i powerState %i", runState, powerState);
        if (runState == D_GOTOCHARGED_ON_USBC_AUTO)
        {
            switch (prevRunState)                                               //manage transitions from previous runState to standby
            {
                case D_STANDBY:
                    break;
                case D_ALWAYS_ON:
                    stoptimerCharging();
                    ACRelaysOff(Z_ONC);
                    break;
                case D_TIMED_ON:
                    stoptimerCountdown();
                    ACRelaysOff(Z_TIMED);
                    break;
                case D_CHARGED_ON:
                    stoptimerCharging();
                    ACRelaysOff(Z_ONTIL);
                    break;
                case D_CHARGED_ON_USBC:
                    stoptimerCharging();
                    ACRelaysOff(Z_USBC);
                    break;
                default:
                    break;
            }

            helperGoToUSBCCharged(R_USBC);
            runState = D_CHARGED_ON_USBC_AUTO;
            prevRunState = runState;            
        }
        else                                                                    //CHARGED_ON_USBC
        {
            helperGoToUSBCCharged(R_USBC); 
            if (runState == D_GOTOCHARGED_ON_USBC) runState = D_CHARGED_ON_USBC;//traps the condition that the web command standby has been sent during smart charge startup
            prevRunState = runState;                                            //V104
        }
    }
    else
    {
        if (runState == D_GOTOCHARGED_ON_USBC)
        {
            runState = D_GOTOSTANDBY;
        }
        else
        {
            runState = prevRunState;                                            //go back to previous state and handle the MAINS_OFF there, userState unchanged
        }
    }
}

// helper to prepare for on until charge cycle
void helperGoToUSBCCharged(int _context)
{
    chargeMins = 0;
    chargeState = C_CHARGING;
    chargingDevices = -1;
    totalAdaptors = 0;
    oucState = ON_UNTIL_USBC;
    ACRelaysOn(_context);
    helperDelayDRUP();                       //V116
    starttimerCharging();
    isChargingStarted = true;                //indicate charging has started to reset the update event timer V108    
    hubupdate = millis();                    //set the hub update time
    charginginitialupdate = millis();            //set the initial update time V107
    Log.info("helperGoToUSBCCharged exit oucState %i resume mins %i", oucState, param.resumeMinutes);
}

// entry from: runState = D_CHARGED_ON_USBC or D_CHARGED_ON_USBC_AUTO 
// function  : monitor for devices fully charged - charging suspends on mains off and resumes when on, maximum on charge period overrides charge status from Hub
// exit to   : runState = various if web command
void chargedOnUSBCController()
{
    powerState = powerStateCheck();                                             //check power supply V104
    if (runState == D_CHARGED_ON_USBC || runState == D_CHARGED_ON_USBC_AUTO)    // || runState == D_AUTO_ON) don't understand why this is here
    {
        helperCheckFirstDRUP();                                                 //V116

        if (isUpdateTimer)                                                      //one minute timer ended
        {
            chargeMins++;
            Log.info("chargedOnUSBCController isUpdateTimer chargeMins %i oucState %i", chargeMins, oucState);
            isUpdateTimer = false;
        }
        if (oucState == ON_UNTIL_USBC || oucState == ON_UNTIL_CHARGE)           //charging or confirming end of charging V106
        {
            if (chargeMins >= param.maxTimeOn)                                  //and maximum time on charge reached => stop
            {
                wasSmartChargeEndedThisHalfHour = true; 
                stoptimerCharging();
                ACRelaysOff(M_USBC);                                            //turn off relays and send event
                chargeState = C_NOT_CHARGING;                                   //set chargeState to C_NOT_CHARGING
                runState = D_GOTOSTANDBY;                                       //max time reached switching off
            }
        }
        else if (oucState == ON_UNTIL_OFF)                                      //web 'aux' command received
        {
            wasSmartChargeEndedThisHalfHour = true;                             //V115
            stoptimerCharging();
            ACRelaysOff(W_USBC);                                                //turn off relays and send event
            chargeState = C_NOT_CHARGING;                                       //V106
            runState = D_GOTOSTANDBY;                                           //standby
        }
        else                                                                    //maximum time not exceeded or suspended/not yet started
        {
            if (!timerCharging.isActive() && powerState != W_MAINS_OFF && !isOverheated && (oucState < ON_UNTIL_SENSE || oucState == ON_UNTIL_CHARGE)) //Mains Power and suspended/not yet started
            {
                starttimerCharging();                                           //sets chargeMins = 0
                int context = R_USBC;                                           //regular start
                if (param.resumeMinutes >= 0)                                   //only if resume condition to On [amended to include 0 & -1 non resume]
                {
                    if (wasOverheated)                                          //overheated condition ended
                    {
                        wasOverheated=false;
                        context = X_ONTIL;
                        chargeMins = param.resumeMinutes;                       //set minutes counted to value at reset
                        helperCleanUpResumeData();
                    }
                    else                                                        //mains restored (even if a short time off)
                    {
                        mainsPowerRestoredEvent();                                   //send mains restored event
                        if (param.resumeState == D_CHARGED_ON_USBC_AUTO) runState = D_GOTOCHARGED_ON_USBC_AUTO;       //restart the smart charge auto cycle
                        else                                             runState = D_GOTOCHARGED_ON_USBC;            //restart the smart charge cycle
                        helperCleanUpResumeData();
                        return;                                                 //get out of the function 
                    }
                }
                ACRelaysOn(context);                                            //turn on relays and send event according to context
            }
        }

        if (runState != D_WEBGOTOSTANDBY)                                       //only do the rest if there has been no web command to go to standby during soft start
        {
            if (powerState == W_MAINS_OFF && param.resumeMinutes < 0)           //mains removed and not on suspend then suspend charging and stop timer
            {
                Log.info("chargedOnUSBCController Mains Power Off Detected runState %i", runState);
                param.resumeState = runState;
                mainsPowerOffEvent();
                helperSmartMainsOffOverheated(X_USBC);
                mainsofftime = millis();                                        //set the time the mains power is lost
            }
            else if (isOverheated && !wasOverheated)                            //has just become overheated
            {
                wasOverheated = true;
                helperSmartMainsOffOverheated(X_USBC);
                runState = D_GOTOSTANDBY;
            }
            else if (powerState == W_MAINS_OFF && ((millis() - mainsofftime) > GOTOSLEEPDELAY))
            {
                param.resumeState = runState;                                   //added so that if mains off then after a period it will sleep and may go off if no battery power left
                param.resumeMinutes = 0;                                        //smart charge and auto smart will always need to restart the cycle
                putParameters();
                Log.info("chargedOnUSBCController go to sleep runState %i", runState);
                runState = D_GOTOSLEEP;
            }
            else if (powerState != W_MAINS_OFF && !isOverheated)                //only do this checking if not mains off AND not overheated
            {
                if (oucState == ON_UNTIL_USBC)
                {
                    if (millis() - hubupdate >= HUB_UPDATE_INTERVAL)            //time to check the charging port status V106
                    {
                        hubupdate = millis();
                        if (chargeMins > 0 && sampleHubPorts()) {Log.info("charging done"); chargeState = C_CHARGING_DONE; oucState = ON_UNTIL_CHARGE;}    //sample Hub port statuses if all off or green then chargeState = C_CHARGING_DONE
                    }
                }

                if (chargeState == C_CHARGING_ENDED)                            //charging completed for USBC V106
                {
                    Log.info("chargedOnUSBCController Charging Completed runState %i", runState);
                    ACRelaysOff(R_USBC);                                        //turn off relays and send event
                    isTriedAutoOUConce = false;                                 //if ends normally (fully charged) and not Auto OuC Monitoring
                    if (param.isOUCMonitoring)
                    {
                        isTriedAutoOUConce = true;                              //if Auto OuC USBC Monitoring and ends normally then stop restart
                        wasSmartChargeEndedThisHalfHour = true;
                    }
                    chargeState = C_NOT_CHARGING;                               //set chargeState to C_NOT_CHARGING
                    runState = D_GOTOSTANDBY;                                   //really should send a special event for this - devices charged
                }
                else if (chargeState == C_CHARGING_DONE)                        //fully charged, now checked again after HUB_COMPLETION_DELAY
                {
                    if (millis() - hubupdate >= HUB_COMPLETION_DELAY)           //time to recheck the charging port status for completion V106
                    {
                        hubupdate = millis();
                        if (sampleHubPorts())                                   //if all off or green then chargeState = C_CHARGING_DONE
                        {
                            oucState = ON_UNTIL_END;
                            chargeState = C_CHARGING_ENDED;                     //set chargeState to C_CHARGING_ENDED V106
                            stoptimerCharging();                                //stop charge on timer
                            Log.info("chargedOnUSBCController All Ports Green or Off runState %i", runState);
                        }
                        else
                        {
                            oucState = ON_UNTIL_USBC;                           //else stay in charging state
                            Log.info("chargedOnUSBCController Ports Not All Green runState %i", runState);
                        }
                    }
                }
            }
        }
    }
}

// helper to sample all configured Hub ports and return false if any are red
bool sampleHubPorts()
{
    bool allGreenOrOff = true;

    char lvsun[4][17] = {0};
    unsigned long timeout = millis();

    for (int8_t ch = 0; ch < hubdata.channelsIn; ch++)
    {
        while(isHUBbusy(I2C_ADDRESS) && (millis() - timeout < 500)) {delay(50);}
        strcpy(&lvsun[ch][0], receiveMessage(I2C_ADDRESS, ch+1).c_str());
        timeout = millis();
    }

    for (int8_t ch = 0; ch < hubdata.channelsIn; ch++)
    {
        for (int8_t p = 0; p < hubdata.portsIn; p++)
        {
            if (lvsun[ch][p] == 'R') //if any port is red
            {
                allGreenOrOff = false;
                break;              //break out of the port loop
            }
        }   
    }
    Log.info("sampleHubPorts allGreenOrOff %c", allGreenOrOff ? 'Y' : 'N');  //V106
    return allGreenOrOff;
}

#endif // LVSUNCHARGER

// entry from: runState = D_WEBGOTOTIMED in Remote_Admin
// function  : web timed command received
// exit to   : runState = D_GOTOTIMED_ON
void webGoToTimedController()
{
    if (param.isAuto)
    {
        param.isAuto = false;
        putParameters();
    }

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("R").value((int)W_TIMED_ON);
    writer.endObject();
    PublishQueuePosix::instance().publish(eventtimedweb, dataStr, 50, PRIVATE); 
    previousStateHandler(prevRunState);
    prevRunState = runState; 
    runState = D_GOTOTIMED_ON;
    isChargingStarted = true;                //reset update event timer so that DRUP isn't sent V131    

}

// entry from: runState = D_WEBGOTOAUTO in Remote_Admin
// function  : web auto command received
// exit to   : runState = D_GOTOAUTO
void webGoToAutoController()
{
    if (!param.isAuto)
    {
        param.isAuto = true;
        putParameters();
    }

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("R").value((int)W_AUTO_OFF);
    writer.endObject();
    PublishQueuePosix::instance().publish(eventautoweb, dataStr, 50, PRIVATE); 
    previousStateHandler(prevRunState);
    prevRunState = AUTOANDOFF; 
    runState = D_GOTOAUTO;
}

// entry from: runState = D_WEBGOTOON in Remote_Admin
// function  : web always on command received
// exit to   : runState = D_GOTOALWAYS_ON
void webGoToOnController()
{
    if (param.isAuto)
    {
        param.isAuto = false;
        putParameters();
    }

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("R").value((int)W_ON);
    writer.endObject();
    PublishQueuePosix::instance().publish(eventonweb, dataStr, 50, PRIVATE); 
    previousStateHandler(prevRunState);
    prevRunState = runState; 
    runState = D_GOTOALWAYS_ON;
    isChargingStarted = true;                //reset update event timer so that DRUP isn't sent V131    
}

// entry from: runState = D_WEBGOTOCHARGED or D_WEBGOTOCHARGEDUSBC in Remote_Admin
// function  : web on until charged command received for either AC or USBC
// exit to   : runState = D_GOTOCHARGED_ON or D_GOTOCHARGED_ON_USBC
void webGoToChargedController()
{
    if (param.isAuto)
    {
        param.isAuto = false;
        putParameters();
    }
    Log.info("webGoToChargedController");
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    if (runState == D_WEBGOTOCHARGED)   writer.name("R").value((int) W_CHARGED_ON);
    else                                writer.name("R").value((int) W_CHARGED_ON_USBC);
    //writer.name("KL").value((int) C_CHARGING);  //V103/V130
    writer.endObject();
    PublishQueuePosix::instance().publish(eventchargeweb, dataStr, 50, PRIVATE);
    previousStateHandler(prevRunState);
    if (runState == D_WEBGOTOCHARGED)   runState = D_GOTOCHARGED_ON;
    else                                runState = D_GOTOCHARGED_ON_USBC;
    isChargingStarted = true;                //reset update event timer so that DRUP isn't sent V131    
}

// entry from: runState = D_WEBOUCAUTO in Remote_Admin
// function  : web user OUC auto command received - this only sends an event and sets the 
// exit to   : runState = previous runState
void webGoToOUCAutoController()
{
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Enable");
    writer.name("AO").value(1);
    writer.endObject();
    PublishQueuePosix::instance().publish(eventoucautoweb, dataStr, 50, PRIVATE);
    helperAutoSetup();
}

// helper to consolidate duplicated code
void helperAutoSetup()
{
    param.isOUCMonitoring = true;
    putParameters();
    runState = prevRunState;                                        //go back to what it was doing and wait for auto green schedule check to pick up start
    isTriedAutoOUConce = false;                                     //need to clear this flag when Auto Smart Charge
    if (param.hubBoard == 0)    oucState = ON_UNTIL_AUTO;           //set value of oucState if Smart AC charging V111
    else                        oucState = ON_UNTIL_USBC;           //set value of oucState if Smart USBC charging V111
}

// entry from: runState = D_WEBOUCAEXIT in Remote_Admin V085J
// function  : web user OUC auto exit command received
// exit to   : runState = previous runState
void webOUCAExitController()
{
    previousStateHandler(prevRunState);

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("R").value(W_STANDBY);
    writer.name("AO").value(0);
    writer.name("CX").value("Disable");
    writer.endObject();
    PublishQueuePosix::instance().publish(eventoucstopweb, dataStr, 50, PRIVATE);

    isTriedAutoOUConce = false;
    runState = prevRunState;
    oucState = ON_UNTIL_OFF;
    if (runState == D_TIMED_ON || runState == D_ALWAYS_ON || runState == D_CHARGED_ON)
    {
        runState = D_GOTOSTANDBY;                               //standby in the cases where these states and was monitoring for auto start smart charge
    }
}

// entry from: runState = D_WEBGOTOHARDRESTART in Remote_Admin
// function  : web factory reset command received
// exit to   : runStart = D_RESTART
void webGoToFactoryResetController()
{
    param.resumeFlag = HARD_RESET_CMD; 
    memset(param.bleA, 0, sizeof(param.bleA)); 
    memset(param.macA, 0, sizeof(param.macA));
    memset(param.ethA, 0, sizeof(param.ethA));

    bool result = performConfiguration(); // perform the factory reset V062
    param.resumeCause = RESTART_HARD_RESET; //V076
    if (result) putParameters();
    else        Log.error("Factory reset failed");
    restartdata.resumeReason = RESTART_HARD_RESET;
    restartdata.resumeState = prevRunState;
    saveRestartDataToRam();

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Web Factory Reset");
    writer.name("CX").value(result ? "success" : "failed");
    writer.endObject();
    PublishQueuePosix::instance().publish(eventrestartweb, dataStr, 50, PRIVATE);

    waitForQueueToEmpty();      //wait for the queue to empty if possible before going to sleep

    runState = D_RESTART;
}

// entry from: runState = D_WEBGOTOHIBERNATE in Remote_Admin
// function  : web hibernate command received
// exit to   : Hibernating until WKP pin pulse V081
void webGoToHibernateController()
{
    param.resumeCause = RESTART_HIBERNATE;
    putParameters();
    restartdata.resumeReason = RESTART_HIBERNATE;
    restartdata.resumeState = param.powerOnState;
    saveRestartDataToRam();

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Web Hibernate");
    writer.name("R").value((int)W_SLEEPING);
    writer.name("C").value(0); 
    writer.endObject();
    PublishQueuePosix::instance().publish(eventrestartweb, dataStr, 50, PRIVATE);

    waitForQueueToEmpty();      //wait for the queue to empty if possible before going to sleep

    delay (1s);

    Watchdog.stop();                            //stop watchdog timer
    SystemSleepConfiguration config;
    config.mode(SystemSleepMode::HIBERNATE).gpio(P2_PDU_WAKE, RISING);
    System.sleep(config);

}

// entry from: runState = D_WEBGOTORESTART in Remote_Admin
// function  : web restart command received
// exit to   : runStart = D_RESTART
void webGoToRestartController()
{
    #if RESET_AUTO_SMART_MONITORING                 //V110
    if (param.isAuto || param.isOUCMonitoring)
    {
        param.isAuto = false;
        param.isOUCMonitoring = false;
        putParameters();
    }
    #endif // RESET_AUTO_SMART_MONITORING           //V110
    //oucState = ON_UNTIL_OFF;
    restartdata.resumeReason = RESTART_WEBCMD;
    restartdata.resumeState = prevRunState;
    param.resumeCause = RESTART_WEBCMD; //V076
    putParameters();
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Web Restart");
    writer.endObject();
    PublishQueuePosix::instance().publish(eventrestartweb, dataStr, 50, PRIVATE);
    runState = D_RESTART;
}

// entry from: runState = D_WEBGOTOSTANDBY in Remote_Admin V363
// function  : web standby command received - this does not change the isAuto flag but simply stops current activity and goes to standby
// exit to   : runState = D_GOTOSTANDBY
void webGoToStandbyController()
{
    previousStateHandler(prevRunState);                                         //moved here before acknowledgement of web command so that XXSE event sent with Auto Off state consistent with ACRelaysOff() not needing another context
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("R").value((int)W_STANDBY);                                     //V056
    writer.name("CX").value("Web Cmd Standby");
    writer.endObject();
    PublishQueuePosix::instance().publish(eventstandbyweb, dataStr, 50, PRIVATE);
    runState = D_GOTOSTANDBY;
}

// entry from: runState = D_GOTOSLEEP
// function  : Prepare to Sleep mode logic function
// exit to   : runState = D_SLEEPING
void goToSleepController()
{
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("CX").value("Go to Sleep");
    writer.endObject();
    PublishQueuePosix::instance().publish(eventsleep, dataStr, 50, PRIVATE); 
    chargeState = 0;                    //to avoid endless looping with powerState being set as W_CHARGING when just W_MAINS_ON
    runState = D_SLEEPING;
}

// entry from: runState = D_SLEEPING
// function  : suspend until a WKP pin
// exit to   : runState = D_RESTART
void sleepingController()
{
    powerState = powerStateCheck();                     //check power supply just before sleep in case power has come back on
    if (powerState == W_MAINS_OFF)                      //definitely no mains power
    {
        float maxtemp = boardTemp; // default to board temperature
        #if EXT_TEMP_SENSOR
        maxtemp = max(boardTemp, xtemp); // take the maximum of the two sensors
        #endif // EXT_TEMP_SENSOR
        connectedStatus = 0; 
        memset(dataStr, 0, sizeof(dataStr));
        JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
        writer.beginObject();
        writer.name("date").value((const char*)getCreatedTime());
        writer.name("C").value(connectedStatus);
        writer.name("R").value(W_SLEEPING);
        writer.name("TMP").value(maxtemp,1);
        writer.name("POS").value(param.powerOnState);   //V072
        writer.name("CX").value("Sleep until AC power restored");
        writer.endObject();
        PublishQueuePosix::instance().publish(eventvarchanged,dataStr, 50, PRIVATE);
        waitForQueueToEmpty();                              //wait for the queue to empty if possible before going to sleep

        if (powerStateCheck() == W_MAINS_OFF)               //check again that mains is still off
        {
            restartdata.resumeReason = RESUME_MAINS_OFF;    //save to backup RAM in case it never wakes up and is restarted
            param.resumeCause = RESUME_MAINS_OFF;           //V092
            putParameters();                                //save the parameters to flash V072
            restartdata.resumeState = prevRunState;         //V092 in standby controller needed to ensure prevRunState set when mains off
            restartdata.powerOnState = param.powerOnState;  //need to determine values
            if (param.hubBoard == 1) restartdata.isHubBoard = true;
            else restartdata.isHubBoard = false;
            saveRestartDataToRam();

            MCP7940.clearPowerFail(); // Clear power fail flag

            BLEWiFiSetupManager::instance().stopStartAdvertising(false); //stop BLE advertising V063

            Watchdog.stop();                                //stop watchdog timer
            System.enableUpdates();                         //enable firmware updates

            if (powerStateCheck() == W_MAINS_OFF)
            {
                pinResetFast(P2_PDU_3V3_SW_EN);                 //turn off switched 3V3 power
                pinResetFast(P2_PDU_5V_I2C_EN);                 //turn off switched 5V power
                delay(10);                                      //V084

                if (digitalRead(P2_PDU_WAKE) == LOW)            //only if the WAKE pin is LOW go to HIBERNATE V108
                {
                    SystemSleepConfiguration config;                //V061
                    config.mode(SystemSleepMode::HIBERNATE).gpio(P2_PDU_WAKE, RISING); //change to HIBERNATE V083
                    System.sleep(config);                           //V061
                }
                else
                {
                    Log.info("WKP pin HIGH so not going to sleep");
                }
            }
        }

        Watchdog.start();                               //start watchdog timer V045
        restoreRestartDataFromRam();                    //restore the restart data from backup RAM V072

        BLEWiFiSetupManager::instance().stopStartAdvertising(true); //restart BLE advertising V063

        powerState = powerStateCheck();                 //check power supply
        if (powerState == W_MAINS_ON)                   //mains restored just go to standby as if there was mains outage during charging this was too long ago
        {
            pinSetFast(P2_PDU_3V3_SW_EN);               //turn on switched 3V3 power
            pinSetFast(P2_PDU_5V_I2C_EN);               //turn on switched 5V power


            /*
            Log.info("reestablish BLE advertising");
            provision_state = next_provision_state = STATE_IDLE;
            
            BLEWiFiSetupManager::instance().setup((const char*) param.itemName);

            if (WiFi.hasCredentials() && !(WiFi.ready()))
            {
                provision_state = next_provision_state = STATE_CREDENTIALS;
            }
            */

            // may need to reestablish comms with I2C devices (temperature sensors (on board and external), etc.)
            // may need to reestablish comms with ethernet adaptor if fitted
            // implement logic to restart according to poweronState V072
            if (param.resumeMinutes >= 0)
            {
                System.disableUpdates();                //disable firmware updates when not in D_STANDBY
                runState = param.resumeState;           //go directly back to charging controller to resume charging
            }
            else
            {
                isSleepWake = true;                     //set the flag to indicate that this is a wake from sleep and not a restart V072
                runState = D_GOTOSTANDBY;               //go to standby
            }
            mainsPowerRestoredEvent();
        }
        else                                            //still no mains power then do a DEEP SLEEP
        {
            Log.info("still no mains power so going to DEEP SLEEP");
            restartdata.resumeReason = RESTART_DEEP_POWER_DOWN;
            restartdata.resumeState = D_STANDBY;        //V072
            saveRestartDataToRam();                     //save the restart data to backup RAM V072
            param.resumeCause = RESTART_DEEP_POWER_DOWN;//V076
            putParameters();                            //save the parameters to flash V072
            Watchdog.stop();                            //stop watchdog timer
            SystemSleepConfiguration config;
            config.mode(SystemSleepMode::STOP).gpio(WKP, RISING); //was HIBERNATE
            System.sleep(config);
            restoreRestartDataFromRam();                    //restore the restart data from backup RAM V072

            BLEWiFiSetupManager::instance().stopStartAdvertising(true); //restart BLE advertising V063

            powerState = powerStateCheck();                 //check power supply
            if (powerState == W_MAINS_ON)                   //mains restored just go to standby as if there was mains outage during charging this was too long ago
            {
                pinSetFast(P2_PDU_3V3_SW_EN);               //turn on switched 3V3 power
                pinSetFast(P2_PDU_5V_I2C_EN);               //turn on switched 5V power

                if (param.resumeMinutes >= 0)
                {
                    System.disableUpdates();                //disable firmware updates when not in D_STANDBY
                    runState = param.resumeState;           //go directly back to charging controller to resume charging
                }
                else
                {
                    isSleepWake = true;                     //set the flag to indicate that this is a wake from sleep and not a restart V072
                    runState = D_GOTOSTANDBY;               //go to standby
                }
                mainsPowerRestoredEvent();
            }
        }
    }
    else if (powerState == W_MAINS_ON)                  //powerState = MAINS_ON just before sleep called so recover as if sleep had been called and ended
    {
        helperAfterSleep();
    }
}

// helper to check for event queue empty or timeout
void waitForQueueToEmpty()
{
    unsigned long startchecking = millis();
    const unsigned long TIMEOUT = 5000UL;

    while ((!PublishQueuePosix::instance().getCanSleep()) && (millis() - startchecking < TIMEOUT))         //wait for the queue to empty before going to sleep
    {
        PublishQueuePosix::instance().loop();
        delay(1);
    }
}

// wake after gotoSleep either because mains restored just before sleep called or mains restored after sleep
void helperAfterSleep()
{
    getParameters();

    powerState = powerStateCheck();                 //check power supply

    if (param.resumeState > D_STANDBY)              //mains outage during charging and power now restored signalled
    {
        runState = param.resumeState;               //go directly back to charging controller to resume charging and send WAKE/RESUME message
    }
}

// helper to update on doors state (locked/unlocked) param.doorSensors defines the number 0, 1 or 2 of sensors
// all doors locked = true, one or more doors unlocked = false - TO DO now that sensors are I2C will need to complete later
bool doorsLockedCheck()
{
    bool isLocked = true;
    if (param.doorSensors > 0)
    {
        isLocked = true;
        if (param.doorSensors > 1) {isLocked = isLocked && true;}
    }
    return isLocked;
}

// Background checker for stopping whatever trolley is currently doing and starting scheduled On Until Charged

void checkScheduledStartUntilCharged()
{
    if (param.isOUCMonitoring)                                              //auto start On Until Charged monitoring is engaged
    {
        switch (runState)                                                   //on the basis of the runState - positive test i.e. check when in these runStates
        {
            case D_TIMED_ON:
            case D_ALWAYS_ON:
            case D_CHARGED_ON:
            case D_STANDBY:
                {
                    if (Time.isValid())                                         //auto start monitoring can only work with a valid time
                    {
                        isAutoTimeInValidSent = false;
                        if (hasScheduleExpired())                               //schedule for OUC starting has expired V085 V090B V185
                        {
                            oucState = ON_UNTIL_OFF;
                            memset(dataStr, 0, sizeof(dataStr));
                            JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
                            writer.beginObject();
                            writer.name("CX").value("Smart AC or USBC Schedule Expired");
                            writer.endObject();
                            PublishQueuePosix::instance().publish(eventschedulexp, dataStr, 50, PRIVATE);
                            param.isOUCMonitoring = false;                      //stop checking
                            putParameters();
                        }
                        else
                        {
                            int i = 6*(Time.weekday()-1)+(Time.hour()/4);
                            uint8_t block = param.schedule[i];                  //find the byte in the schedule array for the weekday and 4 hour block
                            int j = 7-(2*(Time.hour()%4)+(Time.minute()>29?1:0));
                            uint8_t bit = (block>>j);                           //shift right to move the bit corresponding to the 1/2 hour reqd to LSB
                            if ((bit & 0x01) == 0x01)                           //schedule is for START (1) and not already started this half hour
                            {
                                isTriedAutoOUConce = checkIfChangedPeriod(i, j);//keep a track of whether i and/or j have changed to determine whether in a new time block or schedule changed
                                Log.info("checkScheduledStartUntilCharged schedule bit for start is 1 i %i j %i isTriedAutoOUConce %c", i, j, isTriedAutoOUConce ? 'Y' : 'N'); //V115
                                if (!isTriedAutoOUConce)                        //not started once but exited due to no devices to charge under the same schedule block
                                {
                                    prevRunState = runState;                    //save current runState as goToChargedOnAutoController or goToChargedOnUSBCAutoController will handle the stopping of current activity
                                    if (param.hubBoard == 0)    runState = D_GOTOCHARGED_ON_AUTO;   //V111
                                    else                        runState = D_GOTOCHARGED_ON_USBC_AUTO;
                                }
                            }
                            else                                                //start cleared so clear flag to avoid never restarting
                            {
                                isTriedAutoOUConce = false;
                            }
                        }
                    }
                    else
                    {
                        if (!isAutoTimeInValidSent)                                         //just do once
                        {
                            isAutoTimeInValidSent = true;
                            memset(dataStr, 0, sizeof(dataStr));
                            JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
                            writer.beginObject();
                            writer.name("CX").value("Time invalid");
                            writer.endObject();
                            PublishQueuePosix::instance().publish(eventschedulexp, dataStr, 50, PRIVATE);
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
}

// helper to check if schedule period (30 minutes) has changed since last check returns false if changed otherwise true if 
bool checkIfChangedPeriod(int _i, int _j)
{
    bool decision = true;
    static int ij = 0;      //for the very first check whatever _i and _j values
    _i++;                   //add 1 to _i to avoid multiply by 0 issue
    _j++;                   //add 1 to _j to avoid multiply by 0 issue

    if (isSmartScheduleChanged)
    {
        Log.info("smart schedule changed");
        ij = _i * _j;       //if the schedule has changed then update the last period to be this period
        isSmartScheduleChanged = false;
        wasSmartChargeEndedThisHalfHour = false;    //clear this flag as schedule changed so can restart if required V115
        decision = false;   //decision is false as schedule changed V115
    }
    else if (_i * _j != ij) //if the product of _i and _j is not the same as the last check then it has changed
    {
        Log.info("half hour period changed");
        ij = _i * _j;       //update the last period to be this period
        Log.info("period changed wasSmartChargeEndedThisHalfHour %c", wasSmartChargeEndedThisHalfHour ? 'Y' : 'N'); //V115
        wasSmartChargeEndedThisHalfHour = false; //clear this flag as in a new half hour period so can restart if required V115
        decision = false; //decision is false as period changed V115
    }
    else if (wasSmartChargeEndedThisHalfHour)  //if already ended a charge this same half hour
    {
        decision = true;   //decision is true as already ended a charge this half hour V115
    }
    Log.info("check decision %c", decision ? 'Y' : 'N'); //V115
    return decision;
}

// helper to convert the parameter settings and the instantaneous current and slope readings to a chargeState value 0-10
int determineChargeState(int _chargeState, float _slope, float _amps, int _devices, int _minutes)
{
    int charge_state = _chargeState;
    static int chargeMinutes = 0;

    if (_chargeState == C_CHARGING_DONE)                                                //rate monitoring has ended either on time or slope
    {
        if (_minutes - chargeMinutes >= param.extraChargingTime)
        {
            charge_state = C_CHARGING_ENDED;                                            //charging ended after extra time
            Log.info("determineChargeState charging ended CAmps %f Cdevices %i CState %i Mins %i", _amps, _devices, _chargeState, _minutes);
        }
    }
    else if (_chargeState == C_CHARGING_ENDED)
    {
        charge_state = _chargeState;
    }
    else if (_chargeState == C_RATE_CHARGING)                                           //rate charging then check if rate of charge change signifies charging completed V130
    {
        if (_slope < 0.0) _slope *=-1.0;                                                //abs not working
        if (_slope < param.minChargeRate)                                               //rate of change in charging amps is almost zero = charging finished
        {
            charge_state = C_CHARGING_DONE;
            chargeMinutes = _minutes;                                                   //reset the chargeMinutes
            Log.info("determineChargeState charging done by slope %f CAmps %f at Mins %i", _slope, _amps, _minutes);
        }
        else if (_minutes - chargeMinutes >= param.maxMonitoringTime)                   //if rate monitoring for maxMonitoringTime minutes
        {
            charge_state = C_CHARGING_DONE;                                             //then assume charging done 
            chargeMinutes = _minutes;                                                   //reset the chargeMinutes
            Log.info("determineChargeState charging done by maxMonitoringTime %i", _minutes);
        }
        else
        {
            charge_state = C_RATE_CHARGING; /*C_RATE_CHARGING_PLUS;*/                   //continue in rate charging state by returning this V130
        }
    }
    else if (startChargeRateMonitor(_amps, _devices, _chargeState))                     //put in a function - switch to charge rate of change monitoring
    {
        if (_chargeState == C_CHARGING_FULL)                                            //to avoid continually setting dataPoints = 0 and ptrNow = 0 V130
        {
            dataPoints = 0;                                                             //initialise the pointer to the amps array and number of points
            ptrNow = 0;
            chargeMinutes = _minutes;                                                   //save the current minutes count to determine how long in rate charging
            charge_state = C_RATE_CHARGING; 
            Log.info("determineChargeState start rate monitoring CAmps %f Cdevices %i CState %i Mins %i", _amps, _devices, _chargeState, _minutes);
        }
    }
    else                                                                                //must be charging  
    {
        Log.info("determineChargeState none of above CAmps %f Cdevices %i CState %i", _amps, _devices, _chargeState);
        charge_state = _chargeState;                                                    //otherwise return existing chargeState
    }

   return charge_state;
}

// helper to return bool true if charge rate monitoring should start V119
bool startChargeRateMonitor(float _amps, int _devices, int _chargeState)
{
    bool result = false;
    float thresholdCurrent = _devices * param.minCurrent;
    if (_devices > 0 && _amps < thresholdCurrent)             //only if there are devices connected and total current less than threshold
    {
        Log.info("startChargeRateMonitor CAmps %f Cdevices %i CState %i Thresh %f", _amps, _devices, _chargeState, thresholdCurrent);
        if (_chargeState == C_CHARGING_FULL || _chargeState == C_RATE_CHARGING)
        {
            result = true;
        }
    }
    return result;
}

// helper to calculate the number of adaptors for charging calculations V119
int numberOfAdaptors()
{
    int devices = 0;
    for (int i = 0; i < 4; i++)                   //check all 4 outlets/sockets
    {
        devices += param.devicesPerOutlet[i];
    }

    if (devices == 0)                             //devices per outlet has not been set
    {
        for (int i = 0; i < 4; i++)               //check all 4 outlets/sockets
        {
            devices += param.numSpaces[i];        //assume one device per space
        }
    }
    return devices;
}

// function to check which is the next runState and userState for the given circumstances
int nextRunStateCheck()
{
    int _runState = D_GOTOSTANDBY;
    if (param.isAuto && !hasScheduleExpired()) _runState = D_GOTOAUTO;   //at startup and schedule has not expired/invalid V185 V301
    return _runState;
}

// function to return the power state of the device W_MAINS_ON or W_MAINS_OFF or W_CHARGING
int powerStateCheck()
{
    int _powerstate = W_MAINS_ON;
    if (!isACSupplyPresent())
    {
        _powerstate = W_MAINS_OFF;
    }
    else if (chargeState > C_NOT_CHARGING)
    {
        _powerstate = W_CHARGING;      //chargeState is only measured when charging is on
    }
    return _powerstate;
}

// used in webGoToXXXControllers according to the previous runState stop timers, turn off relays and send events
void previousStateHandler(int _prev)
{
    Log.info("previousStateHandler: %i", _prev);
    switch (_prev)
    {
        case D_AUTO_ON:
            stoptimerCharging();
            ACRelaysOff(W_AUTO); //was R_AUTO ?
            break;
        case D_AUTO_OFF:
            ACRelaysOff(W_AUTO); //was R_AUTO ?
            break;
        case D_TIMED_ON:
            stoptimerCountdown();
            ACRelaysOff(W_TIMED);
            break;
        case D_ALWAYS_ON:
            stoptimerCharging();
            ACRelaysOff(W_ONC); 
            break;
        case D_GOTOCHARGED_ON: 
        case D_CHARGED_ON:
            stoptimerCharging();
            ACRelaysOff(W_ONTIL);   //What if _ON_AUTO?
            break;
        case D_GOTOCHARGED_ON_USBC:
        case D_CHARGED_ON_USBC:
            stoptimerCharging();
            ACRelaysOff(W_USBC);    //What if _ON_USBC_AUTO?
            break;
        default:
            break;
    }
}

// Linear Regression function V050
// simplified computation to return only the slope and not the intercept or correlation R
// inputs n  - number of points
//        x,y- array of data pairs
// returns slope of best fit line
float linearRegression(int n, float x[], float y[])
{
    float sumx  = 0.0;
    float sumx2 = 0.0;
    float sumxy = 0.0;
    float sumy  = 0.0;
    if (n < 6) return 1.0;                      //cannot fit a line with less than 6 [V090E] points V056 return 1.0 rather than 0.0 as immediately charging is done!
    for (int i = 0; i < n; i++)
    {
        sumx    += x[i];
        sumx2   += (x[i] * x[i]);
        sumxy   += (x[i] * y[i]);
        sumy    += y[i];
    }
    float denom = ((float)n * sumx2) - (sumx * sumx);
    if (denom == 0.0) return 0.0;               //avoid divide by zero error
    float slope = (((float)n * sumxy) - (sumx * sumy))/denom;      //V090E
    return fabs(slope);
}

// helper to send message Mains Power Off
void mainsPowerOffEvent()
{
    powerStateInt = powerState;
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("C").value(1);
    writer.name("Z").value(W_MAINS_OFF);
    writer.endObject();
    PublishQueuePosix::instance().publish(eventmainsoff, dataStr, 50, PRIVATE);
}

// helper to send message Mains Power Resumed
void mainsPowerRestoredEvent()
{
    powerStateInt = powerState;
    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
    writer.beginObject();
    writer.name("date").value((const char*)getCreatedTime());
    writer.name("C").value(1);
    writer.name("Z").value(W_MAINS_ON);
    writer.endObject();
    PublishQueuePosix::instance().publish(eventmainsresum, dataStr, 50, PRIVATE);
}

// helper clean up resume Minutes and State after a resume
void helperCleanUpResumeData()
{
    param.resumeMinutes = -1;               //clear resume minutes
    param.resumeState = D_STARTUP;          //clear resume state
    param.resumeCause = 0;                  //clear resume cause
    putParameters();                        //save to eeprom to avoid repeated resume at restart
}

// handler for timerCountdown ended countdown timer
void timing_countdown()
{
    isUpdateCountdown = true;
}

// handler for timerReset ended
void resetTimeout()
{
   isResetTimeout = true;
   timerReset.stop();
}

// start timerCountdown countdown timer
void starttimerCountdown()
{
    timerCountdown.start();
    istimerCountdown_ended = false;
    isUpdateCountdown = false;
}

// stop timerCountdown countdown timer
void stoptimerCountdown()
{
    istimerCountdown_ended = true;
    timerCountdown.stop();
    isUpdateCountdown = false;
}

// handler for timerCharging ended
void charge_timer()
{
    isUpdateTimer = true;
}

// start timerCharging
void starttimerCharging()
{
    timerCharging.start();
    isUpdateTimer = false;
}

// stop timerCharging
void stoptimerCharging()
{
    timerCharging.stop();
    isUpdateTimer = false;
}

// initialisation of software timers and flags
void initTimers()
{
    timerCountdown.stop();
    timerCharging.stop();
    istimerCountdown_ended = false;
    isUpdateTimer = false;
    timerMins = 0; 
    chargeMins = 0; 
}

// switch AC solenoids on if off (with phased start) and publish event depending upon context - timed/auto/etc.
void ACRelaysOn(int context)
{
    prevPowerState = powerState;  
    powerState = powerStateInt = W_CHARGING;
    for (size_t i = 0; i < sizeof(P2_PDU_RELAYS) / sizeof(P2_PDU_RELAYS[0]); i++)
    {
        pinSetFast(P2_PDU_RELAYS[i]);
        delay(1000 * SSDELAY);  // phased start of relays to avoid inrush current issues V126
    }

    #if LVSUNCHARGER
    delay(5s); // wait for LVSUN chargers to start up V059
    for (int8_t i = 0; i < hubdata.channelsIn; i++)
    {
        LVSUNInputChannelsOnOff(I2C_ADDRESS, (uint8_t) i+1, true); // turn on LVSUN input channels V059
        delay(1000 * SSDELAY);  // phased start of relays to avoid inrush current issues V126
    }
    if (hubdata.channelsIn > 0) Log.info("LVSUN Input Channels turned on");
    #endif // LVSUNCHARGER

    chargeState = C_CHARGING;

    float maxtemp = boardTemp; // default to board temperature
    #if EXT_TEMP_SENSOR
    maxtemp = max(boardTemp, xtemp); // take the maximum of the two sensors
    #endif // EXT_TEMP_SENSOR

    onViewRunState();

    sampleCurrent();                                                    //update ampsrms for use in events

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);

    switch (context)
    {
        case X_ONC:
        case X_AUTO:
        case X_TIMED:
        case X_ONTIL:
        case X_USBC:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            if (context == X_ONTIL) writer.name("CX").value("Resume Warm-up Smart AC"); //V130
            else                    writer.name("CX").value("Resume After Suspend"); //V130
            writer.name("R").value(runStateInt);
            writer.name("Z").value(powerStateInt);
            writer.name("LA").value((double) powerdata.ampsrms,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V113
            if (context == X_ONTIL) writer.name("KL").value(chargeState);   //V130
            writer.name("TMP").value((double)maxtemp,1);
            writer.endObject();
        break;
        case R_AUTO:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("CX").value("Normal Start");
            writer.name("R").value(W_AUTO_ON);
            writer.name("Z").value(powerStateInt);
            writer.name("LA").value((double) powerdata.ampsrms,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V113
            writer.name("TMP").value((double)maxtemp,1);
            writer.name("KL").value(chargeState);
            writer.endObject();
            break;
        case R_ONTIL:
        case R_USBC:
        case R_ONC:
        default:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            if (context == R_ONTIL) writer.name("CX").value("Normal Warm-up Smart AC"); //V130
            else if (context == R_USBC) writer.name("CX").value("Normal Start USB-C");
            else writer.name("CX").value("Normal Start");
            writer.name("R").value(runStateInt);
            writer.name("Z").value(powerStateInt);
            writer.name("LA").value((double) powerdata.ampsrms,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V113
            writer.name("TMP").value((double)maxtemp,1);
            writer.name("KL").value(chargeState);
            writer.endObject();
            break;
    }

    switch (context)
    {
        case X_AUTO:
        case R_AUTO:
            PublishQueuePosix::instance().publish(eventscheduleon,dataStr, 50, PRIVATE);
            break;
        case X_ONC:
        case R_ONC:
            PublishQueuePosix::instance().publish(eventonstart,dataStr, 50, PRIVATE);
            helperDelayDRUP();                              //V116
            break;
        case X_TIMED:
        case R_TIMED:
            PublishQueuePosix::instance().publish(eventtimerstart,dataStr, 50, PRIVATE);
            helperDelayDRUP();                              //V116
            break;
        case X_ONTIL:
        case R_ONTIL:
        case X_USBC:
        case R_USBC:
            PublishQueuePosix::instance().publish(eventontilstart,dataStr, 50, PRIVATE);
            helperDelayDRUP();                              //V116
            break;
    }
}

// switch AC solenoids off if on and publish event depending upon context - timed/auto/none - V409 added last charge data
void ACRelaysOff(int context)
{
    for (size_t i = 0; i < sizeof(P2_PDU_RELAYS) / sizeof(P2_PDU_RELAYS[0]); i++)
    {
        pinResetFast(P2_PDU_RELAYS[i]);
    }

    #if LVSUNCHARGER
    char lvsun[4][17] = {0};        // V091
    for (int8_t i = 0; i < hubdata.channelsIn; i++)
    {
        LVSUNInputChannelsOnOff(I2C_ADDRESS, (uint8_t) i+1, false); // turn off LVSUN input channels V059
        Log.info("LVSUN Input Channel %i turned off", i+1);
        for (int8_t j = 0; j < hubdata.portsIn; j++) {lvsun[i][j] = 'O';}
    }
    
    LVSUNLEDsOff();                 // V053
    
    #endif // LVSUNCHARGER

    if (powerState == W_CHARGING) powerState = powerStateInt = W_MAINS_ON;

    int kvalue = chargeMins + 1;    //V097

    float maxtemp = boardTemp; // default to board temperature
    #if EXT_TEMP_SENSOR
    maxtemp = max(boardTemp, xtemp); // take the maximum of the two sensors
    #endif // EXT_TEMP_SENSOR

    memset(dataStr, 0, sizeof(dataStr));
    JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);

    switch (context)
    {
        case R_NONE:
        break;
        case X_ONC:
        case X_AUTO:
        case X_TIMED:
        case X_ONTIL:
        case X_USBC:
            if (context == X_TIMED) kvalue = timerMins;
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("CX").value("Suspended Mains Off or Overheated");
            writer.name("R").value(runStateInt);
            writer.name("Z").value(powerStateInt);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            writer.name("K").value(kvalue);
            writer.name("TMP").value((double)maxtemp,1);
            writer.name("KL").value(chargeState);
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        case Z_ONC:
        case Z_TIMED:
        case Z_ONTIL:
        case Z_USBC:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("CX").value("Stopped for Auto Smart Charge");
            writer.name("R").value(W_STANDBY);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        /*case M_ONC:*/
        case M_ONTIL:
        case M_USBC:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("CX").value("Stopped at Maximum Time Charging");
            writer.name("R").value(W_STANDBY);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            writer.name("KL").value(chargeState);   //V130
            writer.name("K").value(param.maxTimeOn);
            writer.name("TMP").value((double)maxtemp,1);
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        case W_ONC:
        case R_ONC:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            if (context == R_ONC) writer.name("CX").value("Normal");
            else writer.name("CX").value("Web Cmd Standby");
            writer.name("R").value(W_STANDBY);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            writer.name("K").value(chargeMins);
            writer.name("TMP").value((double)maxtemp,1);
            writer.name("KL").value(0);
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        case R_AUTO:
        case W_AUTO:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            if (context == R_AUTO) writer.name("CX").value("Normal");
            else writer.name("CX").value("Web Cmd Standby");
            writer.name("R").value(W_AUTO_OFF);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            writer.name("K").value(kvalue);     //V097
            writer.name("TMP").value((double)maxtemp,1);
            writer.name("KL").value(0);
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        case W_TIMED:
        case R_TIMED:
            kvalue = timerMins;
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            if (context == R_TIMED) writer.name("CX").value("Normal Period");
            else writer.name("CX").value("Web Cmd Standby");
            writer.name("R").value(W_STANDBY);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            writer.name("K").value(kvalue);
            writer.name("TMP").value((double)maxtemp,1);
            writer.name("KL").value(0);
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        case R_ONTIL:
        case W_ONTIL:
        case R_USBC:
        case W_USBC:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            if (context == R_ONTIL) writer.name("CX").value("Extra time ended");
            else writer.name("CX").value("Normal end");
            writer.name("R").value(W_STANDBY);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(/*(double) powerdata.ampsrms*/0.0,3);   //V121
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            writer.name("K").value(kvalue);
            writer.name("TMP").value((double)maxtemp,1);
            writer.name("KL").value(C_CHARGING_ENDED); //V130
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        case T_AUTO:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("CX").value("Suspended as no valid time");
            writer.name("R").value(W_STANDBY);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
        case AUTOANDOFF:
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("CX").value("Auto Control with schedule off");
            writer.name("R").value(W_AUTO_OFF);
            writer.name("Z").value(W_MAINS_ON);
            writer.name("LA").value(0.0,3);
            writer.name("LV").value((double) powerdata.voltsrms,3);         //V121
            writer.name("TMP").value((double)maxtemp,1);
            #if LVSUNCHARGER
            if (hubdata.channelsIn > 0)                     //V091
            {
                writer.name("LV0").beginArray();
                writer.value(1);
                writer.value((const char*) &lvsun[0][0]);
                writer.value(2);
                writer.value((const char*) &lvsun[1][0]);
                writer.value(3);
                writer.value((const char*) &lvsun[2][0]);
                writer.value(4);
                writer.value((const char*) &lvsun[3][0]);
                writer.endArray();
            }
            #endif //LVSUNCHARGER
            writer.endObject();
            break;
    }

    if (chargeState != C_NOT_CHARGING) chargeState = C_NOT_CHARGING;

    switch (context)
    {
        case R_NONE:
        break;
        case X_TIMED:
        case R_TIMED:
        case Z_TIMED:
        case W_TIMED:
            PublishQueuePosix::instance().publish(eventtimerended, dataStr, 50, PRIVATE);
            break;
        case R_ONTIL:
        case X_ONTIL:
        case M_ONTIL:
        case W_ONTIL:
        case Z_ONTIL:
        case R_USBC:
        case X_USBC:
        case M_USBC:
        case W_USBC:
        case Z_USBC:
            PublishQueuePosix::instance().publish(eventontilended, dataStr, 50, PRIVATE);
            break;
        case X_ONC:
        case R_ONC:
        case M_ONC:
        case Z_ONC:
        case W_ONC:
            PublishQueuePosix::instance().publish(eventonended, dataStr, 50, PRIVATE);
            break;
        case T_AUTO:
            PublishQueuePosix::instance().publish(eventschedulexp, dataStr, 50, PRIVATE);
            break;
        case R_AUTO:
        case X_AUTO:
        case W_AUTO:
        case AUTOANDOFF:
            PublishQueuePosix::instance().publish(eventscheduloff, dataStr, 50, PRIVATE);
            break;
    }
}

// helper function to test if valid auto schedule i.e. not expired and not all zero = none
bool hasInvalidSchedule(uint8_t *sched)
{
    uint16_t ss = 0;
    if (Time.isValid())
    {
        for (int i = 0; i < ARRAYSIZE; i++) {ss += sched[i];}
        if (hasScheduleExpired() || ss == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}

// helper to unpack schedule in C (char array) string pval and place in the uint8_t array pointed to with unpacked return true if done or false if empty schedule array
bool unpackSchedule(char* pval, uint8_t* unpacked)
{
    char char_array_4[57];
    strncpy(char_array_4, (const char*) pval, 57);             // copy string to char array to simplify access
    for (int i = 0, j = 0; i < 56;)
    {
        unpacked[j]   = (decodeBase64(char_array_4[i]) << 2) + ((decodeBase64(char_array_4[i+1]) & 0x30) >> 4); 
        unpacked[j+1] = ((decodeBase64(char_array_4[i+1]) & 0xf) << 4) + ((decodeBase64(char_array_4[i+2]) & 0x3c) >> 2);
        unpacked[j+2] = ((decodeBase64(char_array_4[i+2]) & 0x3) << 6) + (decodeBase64(char_array_4[i+3]) & 0x3f);
        i+=4;
        j+=3;
    }
    uint16_t ss = 0;
    for (int i = 0; i < ARRAYSIZE; i++) {ss += unpacked[i];}      //empty schedule array if total = 0
    return ss > 0 ? true: false;
}

// search for index of character in the base64 lookup table uses char array base64chars to determine value of base64 character
byte decodeBase64(char b64chr)
{
    char base64chr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";            //do not change
    byte index = 0;
    while (base64chr[index] != b64chr && index < 64) index++;
    return index;
}

// helper function to read the current from power sensor
void sampleCurrent()
{
    mySensor.readRMS(&powerdata.voltsrms, &powerdata.ampsrms);      // Read the RMS voltage and current
    //mySensor.readPowerActiveReactive(&powerdata.apowerwatt, &powerdata.rpowerwatt); // Read the active power
    //powerdata.rpowerwatt -= RPOWER_OFFSET;                          // remove offset from reactive power reading
    if (powerdata.voltsrms < 100.0) powerdata.voltsrms = 0.0;       // if voltage is less than noise then no voltage
    powerdata.ampsrms -= AMPS_OFFSET;                               // remove offset from current reading          

    if (powerdata.voltsrms == 0.0) powerdata.ampsrms = 0.0;         // if no voltage then no current
    else if (powerdata.ampsrms < AMPS_DEADBAND && powerdata.ampsrms > -AMPS_DEADBAND) powerdata.ampsrms = 0.0;    // if current in noise then zero
    else powerdata.ampsrms *= AMPS_GAIN;    //param.powerMeterFactor;               // V079 scale current reading

    if (powerdata.voltsrms > VOLTS_CLAMP) powerdata.voltsrms = VOLTS_CLAMP;     // if voltage is greater than 250V then set to 240V V054

    if (powerState != W_CHARGING) powerdata.ampsrms = 0.0;          // if not charging then current will be offset rather than zero V055

    //Log.info("Volts (RMS): %4.1f Amps(RMS): %5.3f" /*Active Power(W): %4.2f Reactive Power(W): %4.2f*/, powerdata.voltsrms, powerdata.ampsrms /*, powerdata.apowerwatt, powerdata.rpowerwatt*/);
}

// BQ25185 battery management system
//                    CHARGING1 CHARGING2
// Charging complete      HIGH     HIGH
// Charging               HIGH     LOW
// Recoverable fault      LOW      HIGH
// Non-recoverable fault  LOW      LOW
void sampleBMS()
{
    bool isCharging1 = digitalRead(P2_PDU_BATT_CHARGING1);
    bool isCharging2 = digitalRead(P2_PDU_BATT_CHARGING2);
    batterydata.isCharging = isCharging1 && !isCharging2;
    batterydata.isCharged = isCharging1 && isCharging2;
    batterydata.isFault = !isCharging1;
    batterydata.isFaultRecoverable = isCharging2;
    int adcBat = analogRead(P2_PDU_VBAT_MEAS);
    batterydata.batvolts = (float) adcBat * 0.0012418;                   // convert ADC reading to voltage calibrated V086
    battv = (double) batterydata.batvolts;
}

// helper function to test if valid schedule and returns type C, O or N if none or expired schedule
char validSchedule()
{
    char _result = NONESCHEDULE;
    if (Time.isValid())
    {
        if (!hasScheduleExpired())
        {
            _result = param.scheduleType;
            Log.info("Schedule valid of type %c", _result);
        }
    }
    return _result;
}

// helper to check whether device update to cloud should be sent
void checkDeviceUpdate()
{
    static timer_t deviceupdate = 0;
    timer_t DUPCHK = DCHK;

    if (runState == D_STANDBY || runState == D_AUTO_OFF) DUPCHK = SDCHK;    //Slow down DEUP send rate when in standby or Auto_off

    if (isSleepWake) {isSleepWake = false; deviceupdate = 0;}               //force immediate update after wake from sleep V072

    if (isChargingStarted) {isChargingStarted = false; deviceupdate = millis();}   //reset update after charging initiated to suppress sending V108

    if (isChargingJustStarted) {isChargingJustStarted = false; deviceupdate = 0;}   //force immediate update after charging just started V107

    if (deviceupdate == 0 || (millis()-deviceupdate) >= DUPCHK)             //Slow down DEUP send rate when in standby or Auto_off but first time immediately
    {
        deviceupdate = millis();                                            //moved here to keep the timing precise
        int mcd = 0;                                                        //V090 default to 0 
        int door = isDoorLocked?1:0;

        int wifi_rssi = 0;
        float strength = 0.0, quality = 0.0;

        int ouc = param.isOUCMonitoring?1:0;
        int jst = param.isDst?1:0;
        char temp[2] = {0};
        temp[0] = param.scheduleType;

        onViewRunState();

        if (runState == D_CHARGED_ON || runState == D_CHARGED_ON_AUTO) 
        {
            if (oucState >= ON_UNTIL_SS2) mcd = chargingDevices;
        }

        if (Particle.connected())   connectedStatus = 1;
        else                        connectedStatus = 0;

        char bleAddr[18] = {0};
        BleAddress bacAddress = BLE.address();
        for (int i = 0; i < 6; i++) {param.bleA[i] = bacAddress[5-i];}  //V052
        snprintf(bleAddr, sizeof(bleAddr), "%02X:%02X:%02X:%02X:%02X:%02X", param.bleA[0], param.bleA[1], param.bleA[2], param.bleA[3], param.bleA[4], param.bleA[5]);

        char macAddr[18] = {0};
        char ethAddr[18] = {0};
        char locIP[16] = {0};
        char conSSID[33] = {0};    //maximum SSID length is 32 char plus \0 terminator

        int activenetwork = EthernetWiFi::instance().getActiveInterface();

        if (activenetwork == (int) EthernetWiFi::ActiveInterface::ETHERNET)
        {
            connectedStatus = 1;
            wifi_rssi = 10;
            strength = 100.0;
            quality = 100.0;
            Ethernet.macAddress(ethAddress);
            for (int i = 0; i < 6; i++) {param.ethA[i] = ethAddress[i];}
            snprintf(ethAddr, sizeof(ethAddr), "%02X:%02X:%02X:%02X:%02X:%02X", param.ethA[0], param.ethA[1], param.ethA[2], param.ethA[3], param.ethA[4], param.ethA[5]);

        }
        else if (activenetwork == (int) EthernetWiFi::ActiveInterface::WIFI)
        {
            connectedStatus = 1;
            wifi_rssi = WiFi.RSSI();
            WiFiSignal sig = WiFi.RSSI();
            strength = sig.getStrength();
            quality = sig.getQuality();
            WiFi.macAddress(macAddress);
            for (int i = 0; i < 6; i++) {param.macA[i] = macAddress[i];}
            snprintf(macAddr, sizeof(macAddr), "%02X:%02X:%02X:%02X:%02X:%02X", param.macA[0], param.macA[1], param.macA[2], param.macA[3], param.macA[4], param.macA[5]);
            if (WiFi.ready())
            {
                strncpy((char *)locIP, String(WiFi.localIP()), sizeof(locIP));
                strncpy((char *)conSSID, String(WiFi.SSID()), sizeof(conSSID));
            }
        }

        boardTemp = temperatureFromSensor();

        float maxtemp = boardTemp; // default to board temperature
        
        #if EXT_TEMP_SENSOR
        maxtemp = max(boardTemp, xtemp); // take the maximum of the two sensors
        #endif // EXT_TEMP_SENSOR

        powerStateInt = powerStateCheck();

        #if LVSUNCHARGER
        char lvsun[4][17] = {0};
        unsigned long timeout = millis();

        if (powerStateInt == W_MAINS_OFF)
        {
            Log.info("LVSUN Input Channels turned off as Mains Power Off - do not poll");
        }
        else
        {
            for (int8_t ch = 0; ch < hubdata.channelsIn; ch++)         //V060
            {
                while(isHUBbusy(I2C_ADDRESS) && (millis() - timeout < 500)) {delay(50);}
                strcpy(&lvsun[ch][0], receiveMessage(I2C_ADDRESS, ch+1).c_str());
                Log.info("LVSUN Input Channel %i: %s", ch+1, &lvsun[ch][0]);
                timeout = millis();
            }
        }

        /*
        float hubtemp = 0.0;
        hubtemp = HubTemperature(I2C_ADDRESS); // get the hub temperature V059
        Log.info("LVSUN Hub Temperature: %4.1f", hubtemp);
        */

        mcd = 0;
        for (int8_t ch = 0; ch < hubdata.channelsIn; ch++)
        {
            for (uint8_t p = 0; p < hubdata.portsIn; p++)
            {
                if (lvsun[ch][p] == 'R' || lvsun[ch][p] == 'G') mcd++;
            }
        }
        #endif //LVSUNCHARGER

        sampleCurrent();                                                    //update powerdata

        sampleBMS();                                                        //update batterydata

        memset(dataStr, 0, sizeof(dataStr));
        JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);

        switch (runState) 
        {
            case D_TIMED_ON:
            case D_ALWAYS_ON:
            case D_CHARGED_ON:
            case D_CHARGED_ON_AUTO:
            case D_CHARGED_ON_USBC:
            case D_CHARGED_ON_USBC_AUTO:
            case D_AUTO_ON:
            case D_AUTO_OFF:
                writer.beginObject();
                writer.name("date").value((const char*)getCreatedTime());
                writer.name("C").value(connectedStatus);
                writer.name("R").value(runStateInt);
                writer.name("Z").value(powerStateInt);
                writer.name("D").value(door);

                if (wifi_rssi < 0)                  //V090 do not include in JSON if values 0
                {
                    writer.name("Q").value(wifi_rssi);
                    writer.name("SS").value(strength, 2);
                    writer.name("SQ").value(quality, 2);
                }

                writer.name("TMP").value((double)maxtemp,1);
                writer.name("AO").value(ouc);
                writer.name("KL").value(chargeState);
                writer.name("MCD").value(mcd);
                /*writer.name("J").value(jst);*/
                writer.name("LA").value((double)powerdata.ampsrms,3);
                writer.name("LV").value((double)powerdata.voltsrms, 1);     //V113
                /*writer.name("AP").value((double)powerdata.apowerwatt, 2);*/
                /*writer.name("RP").value((double)powerdata.rpowerwatt, 2);*/
                writer.name("VBT").value((double)batterydata.batvolts, 3);
                writer.name("CHG").value(batterydata.isCharging);
                writer.name("CHD").value(batterydata.isCharged);
                writer.name("BFT").value(batterydata.isFault);
                writer.name("RBF").value(batterydata.isFaultRecoverable);
                #if LVSUNCHARGER
                if (powerStateInt != W_MAINS_OFF && hubdata.channelsIn > 0)
                {
                    writer.name("LV0").beginArray();
                    writer.value(1);
                    writer.value((const char*) &lvsun[0][0]);
                    writer.value(2);
                    writer.value((const char*) &lvsun[1][0]);
                    writer.value(3);
                    writer.value((const char*) &lvsun[2][0]);
                    writer.value(4);
                    writer.value((const char*) &lvsun[3][0]);
                    writer.endArray();
                }
                #endif //LVSUNCHARGER
                writer.endObject();
                break;
            default:
                writer.beginObject();
                writer.name("date").value((const char*)getCreatedTime());
                writer.name("C").value(connectedStatus);
                writer.name("R").value(runStateInt);
                writer.name("Z").value(powerStateInt);
                writer.name("D").value(door);

                if (wifi_rssi < 0)                  //V090 do not include in JSON if values 0
                {
                    writer.name("Q").value(wifi_rssi);
                    writer.name("SS").value(strength, 2);
                    writer.name("SQ").value(quality, 2);
                }

                #if GOOGLE_LOCATE 
                writer.name("Y").value((double)latitude, 4);
                writer.name("X").value((double)longitude, 4);
                #endif //GOOGLE_LOCATE

                writer.name("TMP").value((double)maxtemp,1);
                writer.name("AO").value(ouc);
                writer.name("KL").value(chargeState);
                writer.name("MCD").value(mcd);
                writer.name("LA").value((double)powerdata.ampsrms, 3);
                writer.name("LV").value((double)powerdata.voltsrms, 1); //V113
                //writer.name("AP").value((double)powerdata.apowerwatt, 2);
                //writer.name("RP").value((double)powerdata.rpowerwatt, 2);
                writer.name("J").value(jst);
                writer.name("ST").value((const char*)temp);
                writer.name("VBT").value((double)batterydata.batvolts, 3);
                writer.name("CHG").value(batterydata.isCharging);
                writer.name("CHD").value(batterydata.isCharged);
                writer.name("BFT").value(batterydata.isFault);
                writer.name("RBF").value(batterydata.isFaultRecoverable);
                writer.endObject();
                break;
        }
        PublishQueuePosix::instance().publish(eventregularupd, dataStr, 50, PRIVATE);
    }
}

// network information event when there is a change V069/070
void networkInfoEvent()
{
    static char conSSID[33] = {0};    //maximum SSID length is 32 char plus \0 terminator
    static int prevActiveNetwork = 0;

    int activenetwork = EthernetWiFi::instance().getActiveInterface();
    if (prevActiveNetwork != activenetwork || (activenetwork == (int) EthernetWiFi::ActiveInterface::WIFI && strncmp((const char*)conSSID, WiFi.SSID(), sizeof(conSSID)) != 0))
    {
        prevActiveNetwork = activenetwork;
        Log.info("Network change detected, active network: %s", activenetwork == (int) EthernetWiFi::ActiveInterface::ETHERNET ? "Ethernet" : activenetwork == (int) EthernetWiFi::ActiveInterface::WIFI ? "WiFi" : "None");    //V089
        if (activenetwork == (int) EthernetWiFi::ActiveInterface::WIFI)
        {
            strncpy((char *)conSSID, String(WiFi.SSID()), sizeof(conSSID));
        }
        else
        {
            memset(conSSID, 0, sizeof(conSSID)); // reset SSID if not connected to WiFi
        }

        if ((activenetwork == (int) EthernetWiFi::ActiveInterface::ETHERNET) || (activenetwork == (int) EthernetWiFi::ActiveInterface::WIFI))  //to avoid an event with only C=1 V072
        {
            memset(dataStr, 0, sizeof(dataStr));
            JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
            char macAddr[18] = {0};
            char ethAddr[18] = {0};
            char locIP[16] = {0};
            char bleAddr[18] = {0};
            BleAddress bacAddress = BLE.address();
            for (int i = 0; i < 6; i++) {param.bleA[i] = bacAddress[5-i];}  //V095
            snprintf(bleAddr, sizeof(bleAddr), "%02X:%02X:%02X:%02X:%02X:%02X", param.bleA[0], param.bleA[1], param.bleA[2], param.bleA[3], param.bleA[4], param.bleA[5]);

            if (activenetwork == (int) EthernetWiFi::ActiveInterface::ETHERNET)
            {
                Ethernet.macAddress(ethAddress);
                snprintf(ethAddr, sizeof(ethAddr), "%02X:%02X:%02X:%02X:%02X:%02X", ethAddress[0], ethAddress[1], ethAddress[2], ethAddress[3], ethAddress[4], ethAddress[5]);
            }
            else if (activenetwork == (int) EthernetWiFi::ActiveInterface::WIFI)
            {
                WiFi.macAddress(macAddress);
                snprintf(macAddr, sizeof(macAddr), "%02X:%02X:%02X:%02X:%02X:%02X", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
                if (WiFi.ready())
                {
                    strncpy((char *)locIP, String(WiFi.localIP()), sizeof(locIP));
                    strncpy((char *)conSSID, String(WiFi.SSID()), sizeof(conSSID));
                }
            }

            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("C").value(1);
            if (macAddr[0] != 0) writer.name("M").value((const char*)macAddr);  //V076
            if (conSSID[0] != 0) writer.name("W").value((const char*)conSSID);
            if (locIP[0] != 0) writer.name("2").value((const char*)locIP);
            if (ethAddr[0] != 0) writer.name("ETH").value((const char*)ethAddr);
            if (bleAddr[0] != 0) writer.name("BLE").value((const char*)bleAddr); //V095
            if (wifiChannel != 0) writer.name("CH").value(wifiChannel);
            writer.endObject();
            PublishQueuePosix::instance().publish(eventvarchanged, dataStr, 50, PRIVATE);
        }
        else    //V089
        {
            memset(dataStr, 0, sizeof(dataStr));
            JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
            writer.beginObject();
            writer.name("date").value((const char*)getCreatedTime());
            writer.name("C").value(0);
            writer.endObject();
            PublishQueuePosix::instance().publish(eventvarchanged, dataStr, 50, PRIVATE);
        }
    }
}

// this function is automagically called upon a matching POST request aligns with product specification
int remoteAdmin(const char * command)
{
    onViewRunState();                                           //update web state for condition checks
    char num[16];
    strncpy(num, command, sizeof(num)-1);
    if      (strncmp(command, "stb", 3) == 0)
    {
        if (runStateInt == W_STANDBY)                           //command not allowed due to runState
        {
            return -1;                                          // command error
        }
        else
        {
            prevRunState = runState;
            runState = D_WEBGOTOSTANDBY;
            return 1;                                           // command OK
        }
    }
    else if (strncmp(command, "rst", 3) == 0)
    {
        if (runStateInt == W_STANDBY)                           //command only allowed when in standby
        {
            prevRunState = runState;
            runState = D_WEBGOTORESTART;
            return 3;                                           // command OK
        }
        else
        {
            return -3;                                          // command error
        }
    }
    else if (strncmp(command, "hst", 3) == 0)                   //web factory reset
    {
        if (runStateInt == W_STANDBY)                           //command only allowed when in standby
        {
            prevRunState = runState;
            runState = D_WEBGOTOHARDRESTART;
            return 88;                                          // command OK
        }
        else
        {
            return -88;                                         // command error
        }
    }
    else if (strncmp(command, "auo", 3) == 0)                   //combined with autoontil for V090H
    {
        if (hasInvalidSchedule(param.schedule))                 //hasScheduleExpired() = true or None schedule V185
        {
            return -4;                                          // command rejected as no schedule
        }
        else
        {
            switch (param.scheduleType)
            {
            case CHARGEONOFF:                                   //charging on/off schedule loaded
                if (runStateInt == W_STANDBY)                   //command only allowed when in standby
                {
                    prevRunState = runState;
                    runState = D_WEBGOTOAUTO;
                    return 4;                                   // command OK
                }
                else
                {
                    return -4;                                  //command not allowed
                }
                break;
            case OUC_START:                                     //on until charged auto start schedule loaded
                if (runStateInt == W_SLEEPING || runStateInt == W_CHARGED_ON_AUTO || param.isOUCMonitoring)   //command not allowed due to runState or already monitoring V095H
                {
                    return -4;                                  // command error
                }
                else
                {
                    prevRunState = runState;                    //removed setting of isOUCMonitoring to the webGoToOUCAutoController
                    runState = D_WEBOUCAUTO;
                    return 4;                                   // command OK
                }
                break;
            default:
                return -4;                                      // error
            }
        }
    }
    else if (strncmp(command, "tmc", 3) == 0)
    {
        if (runStateInt == W_STANDBY)       //command only allowed when
        {
            prevRunState = runState;
            runState = D_WEBGOTOTIMED;
            return 5;                       // command OK
        }
        else
        {
            return -5;                      // command error
        }
    }
    else if (strncmp(command, "cnc", 3) == 0)
    {
        if (runStateInt == W_STANDBY)       //command only allowed when
        {
            prevRunState = runState;
            runState = D_WEBGOTOON;
            return 6;                       // command OK
        }
        else
        {
            return -6;                      // command error
        }
    }
    else if (strncmp(command, "ouc", 3) == 0)
    {
        if (runStateInt == W_STANDBY)       //command only allowed when
        {
            prevRunState = runState;
            runState = D_WEBGOTOCHARGED;
            return 7;                       // command OK
        }
        else
        {
            return -7;                      // command error
        }
    }
    else if (strncmp(command, "usb", 3) == 0)
    {
        if (runStateInt == W_STANDBY)       //command only allowed when
        {
            prevRunState = runState;
            runState = D_WEBGOTOCHARGEDUSBC;
            return 27;                      // command OK
        }
        else
        {
            return -27;                     // command error
        }
    }
    else if (strncmp(command, "aux", 3) == 0)                   //need to amend to support Auto Charge Off command 
    {
        switch (param.scheduleType)
        {
        case CHARGEONOFF:                                   //charging on/off schedule loaded
            if (runStateInt == W_AUTO_ON || runStateInt == W_AUTO_OFF) //command only allowed if in Auto Charging on/off
            {
                param.isAuto = false;
                param.isOUCMonitoring = false;
                putParameters();
                prevRunState = runState;
                runState = D_WEBGOTOSTANDBY;
                return 9;                                   // command OK
            }
            else
            {
                return -9;                                  //command not allowed
            }
            break;
        case OUC_START:                                     //on until charged auto start schedule loaded
            if (param.isOUCMonitoring)
            {
                param.isAuto = false;                       //V363
                param.isOUCMonitoring = false;              //V363
                putParameters();                            //V363
                prevRunState = runState;                    //removed clearing of isOUCMonitoring to the goToWebOUCAExitController V148E
                runState = D_WEBOUCAEXIT;
                return 9;
            }
            else
            {
                return -9;                                  // command error
            }
            break;
        default:
            return -9;                                      //V095G
        }
    }
    else if (strncmp(command, "hib", 3) == 0)                   //hibernate device V081
    {
        if (runStateInt == W_STANDBY)                           //command only allowed when in standby
        {
            prevRunState = runState;
            runState = D_WEBGOTOHIBERNATE;                      //V081
            return 10;                                          // command OK
        }
        else
        {
            return -10;                                         // command error
        }
    }
    else
    {
        return -99;                                                                         // command not recognised/supported
    }
}

// this function is automagically called upon a matching POST request aligns with product specification
int remoteParam(const char * command)
{
    char num[620];                                      //need larger buffer to hold 8 schedules
    //int len, socket = 0;
    //char *token;                                        //used for strtok
    //const char s[2] = ",";                              //delimiter

    strncpy(num, command, sizeof(num)-1);

    if      (strncmp(command, "dst", 3) == 0)           //summer time dst
    {
        if (strlen(command) == 3) return -23;           //no argument
        int i = atoi((const char*) &num[3]);
        if (i == 0)
        {
            param.isDst = false;
            param.isAutoDst = false;                    //now means auto DST update off
            Time.endDST();
        }
        else if (i == 1)
        {
            param.isDst = true;                 
            param.isAutoDst = false;                    //now means auto DST update off
            Time.beginDST();
        }
        else if (i == 2)
        {
            param.isAutoDst = true;                     //now means auto DST update on
        }
        else
        {
            return -23;                                 //request OK but value out of valid range 0, 1, 2 valid
        }
        putParameters();
        saveTimeSettingsToRam();                        // V029 update time settings in RAM
        return 23;                                      // request and value OK
    }
    else if (strncmp(command, "tzo", 3) == 0)           //time zone tzo
    {
        if (strlen(command) == 3) return -3;            //no argument
        float j = atof((const char*) &num[3]);
        if (j < -12.0 || j > 14.0) return -3;           // request OK but value out of valid range
        param.timeZone = j;
        putParameters();
        Time.zone(param.timeZone);                      // set timeZone on internal clock so that events show correct (local time)
        saveTimeSettingsToRam();                        // V029 update time settings in RAM
        return 3;                                       // request and value OK
    }
    else if (strncmp(command, "nam", 3) == 0)           //device (item) name nam
    {
        int i = strlen((const char*) &num[3]);
        if (i == 0 || i > MAXLEN) return -17;           // request OK but no name or too long
        strncpy(param.itemName, (const char*) &num[3], i+1);
        ItemName = String((const char*) param.itemName);
        putParameters();

        BLEWiFiSetupManager::instance().setDeviceName((const char*) param.itemName); // set device name in ble advertising V063

        return 17;                                      // request and value OK
    }
    else if (strncmp(command, "tip", 3) == 0)           //timerperiod tip
    {
        int i = atoi((const char*) &num[3]);
        if (i < 5 || i > 600) return -1;                // request OK but value out of valid range V112
        //if (i >= param.maxTimeOn) return -1;          // default timer period must be less than the maximum timer on period V099
        if (runState == D_TIMED_ON) return-100;         // currently RunState = timing
        param.timerPeriod = i;
        putParameters();
        return 1;                                       // request and value OK
    }
    else if (strncmp(command, "sch", 3) == 0)           //schedule auto on/off valid to date/time sch - format should be DD/MM/YYYY
    {
        int i = strlen((const char*) &num[3]);
        if (i != 10) return -4;                         //to ensure backward compatibility with use of "sch"
        param.valid2day = 10*(num[3] - '0');
        param.valid2day += (num[4] - '0');
        param.valid2month = 10*(num[6] - '0');
        param.valid2month += (num[7] - '0');
        param.valid2year = 1000*(num[9] - '0');
        param.valid2year += 100*(num[10] - '0');
        param.valid2year +=  10*(num[11] - '0');
        param.valid2year +=     (num[12] - '0');
        return 4;                                       // all OK
    }
    else if (strncmp(command, "sda", 3) == 0)           //schedule data sda
    {
        isScheduleChanged = false;              // assume not required  fails change until positive return
        if (strlen((const char*) &num[3]) != 56) return -4;   // if not 56 then return error -4
        if (param.isOUCMonitoring) return -4;   // if running an auto OuC schedule then disallow change to timed charge
        if (unpackSchedule(&num[3], param.schedule))    //returns true from unpacking means not a none schedule
        {
            if (param.scheduleType == CHARGEONOFF) isScheduleChanged = true;    // existing timed schedule hence schedule changed
            param.scheduleType = CHARGEONOFF;   //V090B
        }
        else
        {
            param.scheduleType = NONESCHEDULE;  //if empty schedule
        }
        putParameters();
        return 4;                               // all OK
    }
    else if (strncmp(command, "oda", 3) == 0)           //schedule data - OUC
    {
        isSmartScheduleChanged = false;                       // assume not required  fails change until positive return V115
        if (strlen((const char*) &num[3]) != 56) return -6;   // if not 56 then return error -6
        if (runStateInt == W_AUTO_OFF || runStateInt == W_AUTO_ON) return -6;   //if running a timed schedule then disallow change to smart or green charge
        if (unpackSchedule(&num[3], param.schedule))
        {
            param.scheduleType = OUC_START;             //currently this needs to be pushed into auto with a Remote_Admin command
            isSmartScheduleChanged = true;              //schedule changed V115
            wasSmartChargeEndedThisHalfHour = false;
        }
        else
        {
            param.scheduleType = NONESCHEDULE;  // if empty schedule
            if (param.isOUCMonitoring)
            {
                param.isOUCMonitoring = false;
                prevRunState = runState;
                runState = D_WEBOUCAEXIT;
            }
        }
        putParameters();
        return 6;                                       // all OK
    }
    else if (strncmp(command, "mxo", 3) == 0)           //maximum time to smart charge
    {
        int i = atoi((const char*) &num[3]);
        if (i < 5 || i > 600) return -28;               //request OK but value out of valid range V112
        /*if (i > 0 && i <= param.timerPeriod) return -28;*///maximum timer period must be more than the default timer period V099
        if (runState == D_CHARGED_ON_USBC || runState == D_CHARGED_ON_USBC_AUTO || runState == D_CHARGED_ON || runState == D_CHARGED_ON_AUTO) return-100; // currently in RunState no change allowed V112
        param.maxTimeOn = i;
        putParameters();
        return 28;                                      // request and value OK
    }
    else if (strncmp(command, "mxt", 3) == 0)           //maximum temperature for alarm 
    {
        int i = atoi((const char*) &num[3]);
        if (i < 1 || i > 99)  return -30;               // request OK but value out of valid range
        param.maxTemp = i;
        putParameters();
        return 30;                                      // request and value OK
    }
    else if (strncmp(command, "mnc", 3) == 0)           //minimum current 
    {
        float j = atof((const char*) &num[3]);
        if (j < 0.001 || j > 0.06) return -34;          // request OK but value out of valid range
        param.minCurrent = j;
        putParameters();
        sense0 =  param.minCurrent;
        return 34;                                      // all OK
    }
    else if (strncmp(command, "mmt", 3) == 0)           //maximum monitoring time V118 
    {
        int j = atoi((const char*) &num[3]);
        if (j < 0 || j > 60) return -35;                // request OK but value out of valid range
        param.maxMonitoringTime = j;                    //V118
        putParameters();
        return 35;                                      // all OK
    }
    else if (strncmp(command, "mnr", 3) == 0)           //minimum charging rate to stop 
    {
        float j = atof((const char*) &num[3]);
        if (j <= 0.0 || j > 0.06) return -36;          // request OK but value out of valid range
        param.minChargeRate = j;
        putParameters();
        return 36;                                      // all OK
    }
    else if (strncmp(command, "ext", 3) == 0)           //extra charging time from done to end V118
    {
        int j = atoi((const char*) &num[3]);
        if (j < 0 || j > 30) return -41;          // request OK but value out of valid range
        param.extraChargingTime = j;
        putParameters();
        return 41;                                      // all OK
    }
    else if (strncmp(command, "wum", 3) == 0)           //warmup minutes 
    {
        int i = atoi((const char*) &num[3]);
        if (i < 1 || i > 10) return -60;                // request OK but value out of valid range
        param.warmupMins = i;
        putParameters();
        return 60;                                      // all OK
    }
    else if (strncmp(command, "dpo", 3) == 0)           //devices per outlet V118 format dpo4,4,4,4
    {
        if (strlen(command) == 3) return -61;           //no argument
        char *token;
        char *rest = (char*)&num[3];
        int i = 0;
        while ((token = strtok(i == 0 ? rest : NULL, ",")) != NULL && i < 4)
        {
            int j = atoi(token);
            if (j < 0 || j > 10) j = 0; // value out of valid range
            param.devicesPerOutlet[i++] = j;
        }
        if (i != 4) return -61; // not enough values
        param.numDevices = numberOfAdaptors();          //recalculate total number of adaptors V119
        putParameters();
        return 61;                                      // all OK
    }
    else if (strncmp(command, "pos", 3) == 0)           //power on state pos
    {
        int i = atoi((const char*) &num[3]);
        if (i < 0 || i > 255) return -69;               //request OK but value out of valid range
        param.powerOnState = i;
        putParameters();
        return 69;                                      // all OK
    }
    else if (strncmp(command, "loc", 3) == 0)           //set to local mode operation V128
    {
        EthernetWiFi::instance().setAutomaticInterface(false);
        param.isLocalMode = true;
        putParameters();
        return 70;                                      // all OK
    }
    else
    {
        return -99;                                     // request not recognised/supported
    }
}

// this function is automagically called upon a matching POST request and supports universal charging command V049
int remoteCharge(const char * command)
{
    int _startcondition, _mode, _endcondition, _maxtime;
    uint16_t _overrideconditions;
    uint8_t _outlets;

    char str[20];
    char* token;
    char* rest = str;
    const char delimiter[] = ",";                       //delimiter
    int count = 0;                                      //used to count the number of tokens
    
    strncpy(str, command, sizeof(str)-1);

    while((token = strtok_r(rest, delimiter, &rest)))
    {
        count++;
        switch (count)
        {
            case 1:                                     //first token is the start condition
                _startcondition = atoi(token);
                if (_startcondition < 0 || _startcondition > 3) return -1; // request OK but value out of valid range
                break;
            case 2:                                     //second token is the charging mode
                _mode = atoi(token);
                if (_mode < 0 || _mode > 3) return -2; // request OK but value out of valid range
                break;
            case 3:                                     //third token is the end condition
                _endcondition = atoi(token);
                if (_endcondition < 0 || _endcondition > 3) return -3;
                break;
            case 4:                                     //fourth token is the maximum charging time
                _maxtime = atoi(token);
                if (_maxtime < 0 || _maxtime > 600) return -4; // request OK but value out of valid range
                break;
            case 5:                                     //fifth token is the override conditions flag
                _overrideconditions = atoi(token);
                if (_overrideconditions < 0 || _overrideconditions > 3) return -5; // request OK but value out of valid range
                break;
            case 6:                                     //sixth token is the outlets to control
                _outlets = (uint8_t) strtol(token, NULL, 2);
                if (_outlets < 0 || _outlets > 15) return -6; // request OK but value out of valid range
                break;
            default:
                return -1;                              // command error
        }
    }
    if (count == 6)
    {
        //check if temporary variables can be included in new command[] struct
        return 1;                           // command OK
    }
    else
    {
        return -7;                          // command error
    }
}

// helper function to translate internal runState D_ to cloud var runStateInt for web app W_
void onViewRunState()
{
    switch (runState)
    {
    case D_STARTUP:
    case D_RESTART:
    case D_STANDBY:
    case D_GOTOSTANDBY:
    case D_WEBGOTOSTANDBY:
    case D_WEBGOTORESTART:
    case D_WEBGOTOHARDRESTART:
        runStateInt = W_STANDBY; 
        break;
    case D_GOTOSLEEP:
    case D_SLEEPING:
    case D_WEBGOTOHIBERNATE: //V081
        runStateInt = W_SLEEPING;
        break;
    case D_WEBGOTOTIMED:
    case D_TIMED_ON:
    case D_GOTOTIMED_ON:
        runStateInt = W_TIMED_ON;
        break;
    case D_WEBGOTOAUTO:
    case D_GOTOAUTO:
    case D_AUTO_OFF:
        runStateInt = W_AUTO_OFF;
        break;
    case D_AUTO_ON:
        runStateInt = W_AUTO_ON;
        break;
    case D_WEBGOTOON:
    case D_GOTOALWAYS_ON:
    case D_ALWAYS_ON:
        runStateInt = W_ON;
        break;
    case D_WEBGOTOCHARGED:
    case D_CHARGED_ON:
    case D_GOTOCHARGED_ON:
        runStateInt = W_CHARGED_ON;
        break;
    case D_WEBGOTOCHARGEDUSBC:
    case D_CHARGED_ON_USBC:
    case D_GOTOCHARGED_ON_USBC:
        runStateInt = W_CHARGED_ON_USBC;
        break;
    case D_GOTOCHARGED_ON_AUTO:
    case D_CHARGED_ON_AUTO:
        runStateInt = W_CHARGED_ON_AUTO;
        break;
    case D_GOTOCHARGED_ON_USBC_AUTO:
    case D_CHARGED_ON_USBC_AUTO:
        runStateInt = W_CHARGED_ON_USBC_AUTO;
        break;
    default:
        runStateInt = W_STANDBY;
    }
}

// test for configuration data sync'd from Cloud to Device Ledger
void configurationAvailableCheck()
{
    static unsigned long lastConfigTime = 0;
    if (millis() - lastConfigTime > LAST_CONFIG)    // check for configuration every 20 seconds
    {
        lastConfigTime = millis();

        if (isConfigurationAvailable)
        {
            checkForConfiguration();
            //Log.info("Configuration Available");
        }
    }
}

// ledger sync call back handler - not used in this example
void syncCallback(Ledger ledger)
{
    isConfigurationAvailable = true;
    //Log.info("syncCallback %s", ledger.name());
}

// check for the configuration data from Cloud to Device Ledger and if present read and update settings
void checkForConfiguration()
{
    if (!isResetPending)
    {
        //Log.info("checkForConfiguration if done before");
        Variant verVariant;
        int ver = 0;

        if (getValue("VER", verVariant))
        {
            if (!verVariant.isNull())
            {
                ver = verVariant.toInt();
                //Log.info("VER = %i", ver);
                
                if (ver > getConfigurationStatus())         // more recent configuration data available
                {
                    if (performConfiguration())
                    {
                        setConfigurationDone(ver);
                        putParameters();
                    }
                    else
                    {
                        memset(dataStr, 0, sizeof(dataStr));
                        JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
                        writer.beginObject();
                        writer.name("date").value((const char*)getCreatedTime());
                        writer.name("CX").value("performConfiguration Error");
                        writer.endObject();
                        PublishQueuePosix::instance().publish(eventdiagnostic, dataStr, 50, PRIVATE);
                    }
                }
            }
        }
    }
}

// perform the initial configuration of the software based on the Cloud to device Ledger values return true if successful
bool performConfiguration()
{
    bool success = false; // all configuration done correctly
    //Log.info("performConfiguration");

    Variant snVariant, prcVariant, namVariant, mxoVariant, mncVariant, extVariant,/*mxcVariant,*/ mxtVariant, mnrVariant, /*mpcVariant,*//*wumVariant,*/ mmtVariant, /*hrfVariant,*/ numVariant, tmpVariant, drsVariant, posVariant, pmfVariant, dopVariant, s1Variant, s2Variant, s3Variant, s4Variant;

    for (int i = 0; i < 6; i++) {param.macA[i] = 0x00;} // clear mac address

    if (getValue("SN", snVariant))
    {
        if (!snVariant.isNull())
        {
            SerialNum = snVariant.toString();
            SerialNum.toCharArray(param.serialNum, SERLEN+1);    //copy SerialNum string to param.serialNum char array
            success = true;
        }
        else
        {
            success = false;
            Log.info("No Serial Number found");
        }
    }

    if (getValue("PRC", prcVariant))
    {
        if (!prcVariant.isNull())
        {
            product_code = prcVariant.toString();
            product_code.toCharArray(param.productCode, PRODLEN);    //copy product_code string to param.productCode char array
            success = success && true;
            if (strstr(param.productCode,"UC") != NULL) param.hubBoard = 1; // set hubBoard to 1 if "UC" is found V076
            else                                        param.hubBoard = 0; // set hubBoard to 0 if "UC" is not found V076
        }
        else
        {
            success = false;
            Log.info("No Product Code found");
        }
    }

    if (getValue("NAM", namVariant))
    {
        if (!namVariant.isNull())
        {
            ItemName = namVariant.toString();
            ItemName.toCharArray(param.itemName, MAXLEN+1);    //copy ItemName string to param.itemName char array
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No Item Name found");
        }
    }

    if (getValue("MXO", mxoVariant))
    {
        if (!mxoVariant.isNull())
        {
            param.maxTimeOn = mxoVariant.toInt();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No maximum time on found");
        }
    }

    if (getValue("MXT", mxtVariant))
    {
        if (!mxtVariant.isNull())
        {
            param.maxTemp = mxtVariant.toInt();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No maximum temperature on found");
        }
    }

    if (getValue("MNC", mncVariant))
    {
        if (!mncVariant.isNull())
        {
            param.minCurrent = (float) mncVariant.toDouble();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No minimum current found");
        }
    }

    /*if (getValue("MXC", mxcVariant)) //V118
    {
        if (!mxcVariant.isNull())
        {
            param.maxCurrent = (float) mxcVariant.toDouble();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No maximum current found");
        }
    }*/

    if (getValue("EXT", extVariant))  //V118
    {
        if (!extVariant.isNull())
        {
            param.extraChargingTime = extVariant.toInt();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No maximum current found");
        }
    }

    if (getValue("MNR", mnrVariant))
    {
        if (!mnrVariant.isNull())
        {
            param.minChargeRate = (float) mnrVariant.toDouble();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No minimum charging rate found");
        }
    }

    /*if (getValue("MPC", mpcVariant)) //V118
    {
        if (!mpcVariant.isNull())
        {
            param.minPluggedCurrent = (float) mpcVariant.toDouble();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No minimum plugged current found");
        }
    }*/

    /*if (getValue("HRF", hrfVariant)) //V118
    {
        if (!hrfVariant.isNull())
        {
            param.halfRateFactor = (float) hrfVariant.toDouble();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No half rate factor found");
        }
    }*/

    /*if (getValue("WUM", wumVariant)) //V118
    {
        if (!wumVariant.isNull())
        {
            param.warmupMins = wumVariant.toInt();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No warm up minutes found");
        }
    }*/

    if (getValue("MMT", mmtVariant))    //V118
    {
        if (!mmtVariant.isNull())
        {
            param.maxMonitoringTime = mmtVariant.toInt();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No maximum monitoring time found");
        }
    }

    if (getValue("NUM", numVariant))
    {
        if (!numVariant.isNull())
        {
            for (int index = 0; index < numVariant.asArray().size(); index++) // array is 0 based
            {
                param.numSpaces[index] = numVariant.asArray().at(index).toInt(); // copy each array element to param.numSpaces which is 0 based
            }
            success = success && true;
            Log.info("param.numSpaces = %i,%i,%i,%i", param.numSpaces[0], param.numSpaces[1], param.numSpaces[2], param.numSpaces[3]);
            param.numDevices = numberOfAdaptors();          //recalculate total number of adaptors V119
        }
        else
        {
            success = false;
            Log.info("No bays/spaces found");
        }
    }
    
    if (getValue("TMP", tmpVariant))
    {
        if (!tmpVariant.isNull())
        {
            param.tempSensors = tmpVariant.toInt();
            Log.info("param.tempSensors = %i", param.tempSensors);
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No temperature sensors found");
        }
    }

    if (getValue("DRS", drsVariant))
    {
        if (!drsVariant.isNull())
        {
            param.doorSensors = drsVariant.toInt();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No door sensors found");
        }
    }

    if (getValue("POS", posVariant))
    {
        if (!posVariant.isNull())
        {
            param.powerOnState = posVariant.toInt();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No power on state found");
        }
    }

    if (getValue("PMF", pmfVariant))
    {
        if (!pmfVariant.isNull())
        {
            param.powerMeterFactor = (float) pmfVariant.toDouble();
            success = success && true;
        }
        else
        {
            success = false;
            Log.info("No power meter factor found");
        }
    }

    return success;
}

//function to restore parameters from EEPROM in setup() V096
void restoreParameters()
{
    int eepromMagicNumber;
    EEPROM.get(PARAM_ADDR, eepromMagicNumber);
    Log.info("EEPROM number %08X PARAM number %08X", eepromMagicNumber, param.magicNumber);
    if (eepromMagicNumber == param.magicNumber)     //magic number is the same (no struct change)
    {
        EEPROM.get(PARAM_ADDR, param);              //read whole struct from eeprom into RAM
        //consider check checksum is correct against calculated checksum or use getParameters()?
        ItemName = String((const char*) param.itemName); //this appears to be failing to copy occasionally V096
        SerialNum = String((const char*) param.serialNum);
        product_code = String((const char*) param.productCode);
        Log.info("restoreParameters EEPROM read OK %s %s %s", param.serialNum, param.productCode, param.itemName);
    }
    else                                            //if changed then default values will have been defined within struct and putParameters() will write to EEPROM at the end of setup() unless this is an upgrade situation
    {
        //checkForConfiguration();
        restoreConfigurationItems();
    }
}

// restore the configuration values, serialNum, productCode, itemName, etc. from EEPROM if required due to upgrade of application and change in param structure V096
void restoreConfigurationItems()
{
    /*
    const int serialNumIndex   = 75;
    const int productCodeIndex = 147;
    const int itemNameIndex    = 177;
    const int config1Index     = 291;   //config2 +2 config3 +4 config4 +8 config5 +12 config6 +21 config7 +23 config8 +73 config9 +76
    const int config1IndexN    = 582;   //config2 +2 config3 +4 config4 +8 config5 +12 config6 +21 config7 +23 config8 - value 0 config9 - value 0
    char oldeeprom[610] = {0};

    EEPROM.get(PARAM_ADDR, oldeeprom);                                                                      //copy EEPROM into temporary buffer
    Log.info("restoreConfigurationFileItems Magic number: %02X Old EEPROM: %02X", ((param.magicNumber>>24) & 0xFF), oldeeprom[0]);
    if (((param.magicNumber>>24) & 0xFF) == 0x99 && (oldeeprom[0] == 0))                                   //the 1st byte of the magic number in the code is 0x99 meaning this is a struct upgrade
    {
        //for (int i = 73; i < 445; i++) {Serial.printlnf("Index: %i %02X %c", i, oldeeprom[i], oldeeprom[i]>='0'?oldeeprom[i]:' ');}
        //Serial.println("Restore settings from old structure");
        strncpy(param.serialNum, &(oldeeprom[serialNumIndex]), sizeof(param.serialNum));                    //copy original serialNum field (in eeprom) to param struct
        //Serial.printlnf("serialNum  : %s", param.serialNum);
        strncpy(param.productCode, &(oldeeprom[productCodeIndex]), sizeof(param.productCode));              //copy original productType field (in eeprom) to param struct
        //Serial.printlnf("productCode: %s", param.productCode);
        strncpy(param.itemName, &(oldeeprom[itemNameIndex]), sizeof(param.itemName));                       //copy original productType field (in eeprom) to param struct
        //Serial.printlnf("itemName: %s", param.itemName);

        strncpy(param.config1, &(oldeeprom[config1Index]), sizeof(param.config1));                          //copy original config1 field (in eeprom) to param struct
        //Serial.printlnf("cfg1  : %s", param.config1);
        strncpy(param.config2, &(oldeeprom[config1Index+2]), sizeof(param.config2));                        //copy original config2 field (in eeprom) to param struct
        //Serial.printlnf("cfg2  : %s", param.config2);
        strncpy(param.config3, &(oldeeprom[config1Index+4]), 2);                                            //copy original config3 field (in eeprom) to param struct
        Serial.printlnf("config3  : %s", param.config3);
        strncpy(param.config4, &(oldeeprom[config1Index+6]), 2);                                            //copy original config4 field (in eeprom) to param struct
        Serial.printlnf("config4  : %s", param.config4);
        strncpy(param.config5, &(oldeeprom[config1Index+8]), sizeof(param.config5));                        //copy original config5 field (in eeprom) to param struct
        Serial.printlnf("config5  : %s", param.config5);
        strncpy(param.config6, &(oldeeprom[config1Index+17]), sizeof(param.config6));                       //copy original config6 field (in eeprom) to param struct
        Serial.printlnf("config6  : %s", param.config6);
        strncpy(param.config7, &(oldeeprom[config1Index+19]), 2);                                           //copy original config7 field (in eeprom) to param struct
        Serial.printlnf("config7  : %s", param.config7);
        param.config8[0] = '0';
        param.config8[1] = '0';
        param.config8[2] = '\0';
        Serial.printlnf("config8  : %s", param.config8);
        param.config9[0] = '0';
        param.config9[1] = '\0';
        Serial.printlnf("config9  : %s", param.config9);

        union Converts
        {
            char chrs[4];
            float realf;
        };

        Converts con;

        strncpy(param.config3, &(oldeeprom[config1Index+4]), 2);                                            //copy original config3 field (in eeprom) to param struct
        //Serial.printlnf("cfg3  : %s", param.config3);
        strncpy(param.config4, &(oldeeprom[config1Index+6]), 2);                                            //copy original config4 field (in eeprom) to param struct
        //Serial.printlnf("cfg4  : %s", param.config4);
        strncpy(param.config5, &(oldeeprom[config1Index+8]), sizeof(param.config5));                        //copy original config5 field (in eeprom) to param struct
        //Serial.printlnf("cfg5  : %s", param.config5);
        strncpy(param.config6, &(oldeeprom[config1Index+17]), sizeof(param.config6));                       //copy original config6 field (in eeprom) to param struct
        //Serial.printlnf("cfg6  : %s", param.config6);
        strncpy(param.config7, &(oldeeprom[config1Index+19]), 2);                                           //copy original config7 field (in eeprom) to param struct
        //Serial.printlnf("cfg7  : %s", param.config7);
        strncpy(param.config8, &(oldeeprom[config1Index+128]), 3);                                          //copy original config8 field (in eeprom) to param struct
        //Serial.printlnf("cfg8  : %s", param.config8);
        strncpy(param.config9, &(oldeeprom[config1Index+131]), 2);                                          //copy original config8 field (in eeprom) to param struct
        //Serial.printlnf("cfg9  : %s", param.config9);

        con.chrs[0] = oldeeprom[config1Index+136];
        con.chrs[1] = oldeeprom[config1Index+137];
        con.chrs[2] = oldeeprom[config1Index+138];
        con.chrs[3] = oldeeprom[config1Index+139];
        param.config10 = con.realf;
        //Serial.printlnf("cfg10 : %f", param.config10);
        con.chrs[0] = oldeeprom[config1Index+140];
        con.chrs[1] = oldeeprom[config1Index+141];
        con.chrs[2] = oldeeprom[config1Index+142];
        con.chrs[3] = oldeeprom[config1Index+143];
        param.config11 = con.realf;
        //Serial.printlnf("cfg11 : %f", param.config11);
        con.chrs[0] = oldeeprom[config1Index+144];
        con.chrs[1] = oldeeprom[config1Index+145];
        con.chrs[2] = oldeeprom[config1Index+146];
        con.chrs[3] = oldeeprom[config1Index+147];
        param.config12 = con.realf;
        //Serial.printlnf("cfg12 : %f", param.config12);

        putParameters();
    }
    else if (((param.magicNumber>>24) & 0xFF) != 0x99)                                                     //different but not a struct upgrade - eventually this can be removed for 
    {
        for (int i = 71; i < 204; i++) {Serial.printlnf("Index: %i %02X %c", i, oldeeprom[i], oldeeprom[i]>='0'?oldeeprom[i]:' ');}
        strncpy(param.serialNum, &(oldeeprom[serialNumIndex-3]), sizeof(param.serialNum));                 //copy original serialNum field (in eeprom) to param struct
        Serial.printlnf("serNum  : %s", param.serialNum);
        strncpy(param.productCode, &(oldeeprom[productCodeIndex-3]), sizeof(param.productCode));           //copy original productType field (in eeprom) to param struct
        Serial.printlnf("prodCode: %s", param.productCode);
        strncpy(param.itemName, &(oldeeprom[itemNameIndex-3]), sizeof(param.itemName));                    //copy original productType field (in eeprom) to param struct
        Serial.printlnf("iName: %s", param.itemName);
        for (int i = 580; i < 610; i++) {Serial.printlnf("Index: %i %02X %c", i, oldeeprom[i], oldeeprom[i]>='0'?oldeeprom[i]:' ');}
        strncpy(param.config1, &(oldeeprom[config1IndexN]), sizeof(param.config1));                         //copy original config1 field (in eeprom) to param struct
        Serial.printlnf("cfg1  : %s", param.config1);
        strncpy(param.config2, &(oldeeprom[config1IndexN+2]), sizeof(param.config2));                       //copy original config2 field (in eeprom) to param struct
        Serial.printlnf("cfg2  : %s", param.config2);
        strncpy(param.config3, &(oldeeprom[config1IndexN+4]), sizeof(param.config3));                       //copy original config3 field (in eeprom) to param struct
        strncpy(param.config4, &(oldeeprom[config1IndexN+8]), sizeof(param.config4));                       //copy original config4 field (in eeprom) to param struct
        strncpy(param.config6, &(oldeeprom[config1IndexN+21]), sizeof(param.config6));                      //copy original config6 field (in eeprom) to param struct
        strncpy(param.config7, &(oldeeprom[config1IndexN+23]), sizeof(param.config7));                      //copy original config7 field (in eeprom) to param struct
        //param.config3[0] = CURRENTENABLED;              //V457
        //param.config3[1] = 0;
        Serial.printlnf("cfg3  : %s", param.config3);
        param.config4[0] = '8';                         //V457
        param.config4[1] = 0;
        Serial.printlnf("cfg4  : %s", param.config4);
        strncpy(param.config5, &(oldeeprom[config1IndexN+12]), sizeof(param.config5));                      //copy original config5 field (in eeprom) to param struct
        Serial.printlnf("cfg5  : %s", param.config5);
        param.config6[0] = 'C';                         //V457
        param.config6[1] = 0;
        Serial.printlnf("cfg6  : %s", param.config6);
        param.config7[0] = 'E';
        param.config7[1] = 0;
        Serial.printlnf("cfg7  : %s", param.config7);
        param.config8[0] = '0';
        param.config8[1] = '0';
        param.config8[2] = '\0';
        Serial.printlnf("cfg8  : %s", param.config8);
        param.config9[0] = '0';
        param.config9[1] = '\0';
        Serial.printlnf("cfg9  : %s", param.config9);
        putParameters();
    }
    */
}

// Helper to calculate the checkSum of parameters from structure param in EEPROM and return true if checks to that stored, false if not refactored
bool checkSumParameters()
{
    uint8_t eeprom_data[sizeof(param)];
    int _chkSum = 0;
    int paramCheckSum;
    EEPROM.get(PARAM_ADDR + sizeof(param) - sizeof(paramCheckSum), paramCheckSum); // read what is stored in the EEPROM copy of checksum
    for (size_t i = 0; i < sizeof(param); i++)
    {
        eeprom_data[i] = EEPROM.read(PARAM_ADDR + i);
    }
    return (calcCheckSum(eeprom_data, sizeof(param) - sizeof(_chkSum)) == paramCheckSum);
}

// Helper to calculate a checksum - returns checksum as an int
int calcCheckSum(void *data, size_t len)
{
    int retVal = 0;
    for (size_t i = 0; i < len; i++)
    {
        retVal += *((uint8_t *)data + i);
    }
    return retVal;
}

// Helper to calculate the checkSum of parameters for the param struct
int calcCheckSumParameters()
{
    uint8_t *_data = (uint8_t *)&param;
    int _chkSum = 0;
    return calcCheckSum(_data, sizeof(param) - sizeof(_chkSum));
}

// function to write parameters to EEPROM with checkSum
void putParameters()
{
    param.checkSum = calcCheckSumParameters();
    EEPROM.put(PARAM_ADDR, param);
}

// function to get parameters from EEPROM if checkSum correct
void getParameters()
{
    if (checkSumParameters()) // checkSum matches
    {
        EEPROM.get(PARAM_ADDR, param);
    }
    else
    {
        memset(dataStr, 0, sizeof(dataStr));
        JSONBufferWriter writer(dataStr, sizeof(dataStr) - 1);
        writer.beginObject();
        writer.name("date").value((const char*)getCreatedTime());
        writer.name("CX").value("getParameter Checksum Error");
        writer.endObject();
        PublishQueuePosix::instance().publish(eventdiagnostic, dataStr, 50, PRIVATE);
    }
}

// This helper function gets a configuration value or returns false if not found or available
bool getValue(const char *key, Variant &value)
{
    bool hasKey = false;

    LedgerData deviceData = cloudconfigurationtodevice.get();
    if (deviceData.has(key))
    {
        value = deviceData.get(key);
        hasKey = true;
    }
    return hasKey;    
}

// This helper function returns the configuration Status ledger version value
int getConfigurationStatus()
{
    Variant value;
    int ver = 0;

    LedgerData data = configurationstatus.get();

    if (data.has("CVER"))
    {
        value = data.get("CVER");
        ver = value.toInt();
    }
    //Log.info("configurationStatus get CVER = %i", ver);
    return ver;
}

// This helper functions sets a configuration value for VER to the value in the Cloud to Device to show configuration has been done
void setConfigurationDone(int _ver)
{
    Variant data;

    //Log.info("configurationStatus setValue CVER = %i", _ver);
    data.set("CVER",_ver);
    configurationstatus.set(data);
}

// helper function gets the WiFi signal strength
void checkForWiFiHealth()
{
}

// callback function to update WiFi channel and frequency band
void updateCallback()
{
    //replace the Log.info with update of variables to be used in DEUP event
    wifiChannel = WiFiChannelRK::instance().getWiFiChannel(); // get the current WiFi channel
    //Log.info("updateCallback channel=%d, 2.4GHz=%d, 5GHz=%d",WiFiChannelRK::instance().getWiFiChannel(),(int) WiFiChannelRK::instance().isWiFi_2_4_GHz(),(int) WiFiChannelRK::instance().isWiFi_5_GHz());
}

#if CMD_BUFFER

// Put runState data into the runState (rs_buffer) buffer within a Cloud Function Handler
// Returns status of action bool true = done false = error
bool putStateBuffer(int _runState, int nextState, uint8_t _data1, uint8_t _data2)
{
    enum BufferStatus stat = BUFFER_OK;
    uint32_t item;
    uint8_t prev_state;
    stat = bufferPeek(&rs_buffer, &item);                               //peek first item in buffer
    if (stat == BUFFER_EMPTY)   prev_state = (uint8_t) _runState;       //buffer empty hence no previous action in queue and prevRunState = current runState
    else                        prev_state = ((item >>8) & 0xFF);       //buffer not empty thus take previous action queued prevRunState and use in this item
    ((uint8_t *)&item)[0] = (uint8_t) nextState;                        //pack the uint32_t item
    ((uint8_t *)&item)[1] = prev_state;
    ((uint8_t *)&item)[2] = _data1;
    ((uint8_t *)&item)[3] = _data2;
    stat = bufferWrite(&rs_buffer, item);
    return (stat == BUFFER_OK);
}

// Get runState and other data from the runState (rs_buffer) buffer in the loop and before switch()
// Returns runState - unchanged from supplied (current) value if nothing in the queue or from the queue
int getRunState(int _runState, int *_prevState, uint8_t *_data1, uint8_t *_data2)
{
    uint32_t item;
    int run_state;
    enum BufferStatus stat = BUFFER_OK;
    stat = bufferRead(&rs_buffer, &item);               //read next item from buffer
    if (stat == BUFFER_EMPTY) return _runState;         //buffer empty hence return current runState
    run_state = (int) (item & 0xFF);                    //else unpack buffer item pulled
    *_prevState = (int) ((item >>8) & 0xFF);
    *_data1 = ((item >>16) & 0xFF);
    *_data2 = ((item >>24) & 0xFF);
    return run_state;
}

// Buffer Write for any buffer
enum BufferStatus bufferWrite(volatile struct Buffer *buffer, uint32_t item)
{
    uint8_t next_index = (((buffer->newest_index)+1) % BUFFER_SIZE);
    if (next_index == buffer->oldest_index) return BUFFER_FULL;
    buffer->data[buffer->newest_index] = item;
    buffer->newest_index = next_index;
    return BUFFER_OK;
}

// Buffer Read for any buffer
enum BufferStatus bufferRead(volatile struct Buffer *buffer, uint32_t *item)
{
    if (buffer->newest_index == buffer->oldest_index) return BUFFER_EMPTY;
    *item = buffer->data[buffer->oldest_index];
    buffer->oldest_index = ((buffer->oldest_index+1) % BUFFER_SIZE);
    return BUFFER_OK;
}

// Buffer Peek
enum BufferStatus bufferPeek(volatile struct Buffer *buffer, uint32_t *item)
{
    uint8_t last_index = ((BUFFER_SIZE + (buffer->newest_index) - 1) % BUFFER_SIZE);
    if (buffer->newest_index == buffer->oldest_index) return BUFFER_EMPTY;
    *item = buffer->data[last_index];
    return BUFFER_OK;
}

#endif  //CMD_BUFFER

#if LVSUNCHARGER
// V048
/*
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OOOOOOOOOR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OOOOOOOOOR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OOOOOOOOOR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OOOOOOOOOR"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OOOOOOOORG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OOOOOOOORG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OOOOOOOORG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OOOOOOOORG"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OOOOOOORGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OOOOOOORGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OOOOOOORGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OOOOOOORGR"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OOOOOORGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OOOOOORGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OOOOOORGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OOOOOORGRG"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OOOOORGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OOOOORGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OOOOORGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OOOOORGRGR"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OOOORGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OOOORGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OOOORGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OOOORGRGRG"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OOORGRGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OOORGRGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OOORGRGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OOORGRGRGR"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "OORGRGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "OORGRGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "OORGRGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "OORGRGRGRG"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "ORGRGRGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "ORGRGRGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "ORGRGRGRGR"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "ORGRGRGRGR"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
    (void) sendMessage(I2C_ADDRESS, 0xED, 1, "RGRGRGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 2, "RGRGRGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 3, "RGRGRGRGRG"); // Send a message to the I2C slave
    (void) sendMessage(I2C_ADDRESS, 0xED, 4, "RGRGRGRGRG"); // Send a message to the I2C slave
    delay(2000); // Wait for 2 seconds
*/

// function to set the LVSUN LEDs to off V053/V057 - TO DO make this aligned with Hub setup channels and ports
void LVSUNLEDsOff()
{
    unsigned long timeout = millis();
    char LEDstatus[hubdata.portsLED + 1] = {0};
    memset(LEDstatus, 'O', hubdata.portsLED); // Initialize the LED status to 'O' (off)

    for (int8_t ch = 1; ch <= hubdata.channelsLED; ch++) // loop through all channels
    {
        while(isHUBbusy(I2C_ADDRESS) && (millis() - timeout < 500)) {delay(50);}
        (void) sendMessage(I2C_ADDRESS, 0xED, ch, LEDstatus); // Send a message to the I2C slave
        Log.info("LVSUN LEDs ch%i %s", ch, LEDstatus);
        timeout = millis();
    }
}

// i2Csetup() called from setup() runs once, when the device is first turned on
void i2cSetup()
{
    if (!Wire.isEnabled())
    {
        Wire.begin();   // Initialize I2C central
    }
}

// function to receive Hub board version and configuration data
uint32_t hubBoardVersion()
{
    const size_t bufferSize = 9; // Size of the buffer
    uint8_t result[bufferSize] = {0};
    size_t index = 0;
    byte _error = 0;

	if (!Wire.isEnabled()) Wire.begin(); 

    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(I2C_ADDRESS);        //start transmission to device
        Wire.write(0x1D);                           //return board ID and version
        _error = Wire.endTransmission();
    }

    if (_error != 0) return 0;

    Wire.requestFrom(I2C_ADDRESS, bufferSize);      //request bufferSize bytes from peripheral device address

    while(Wire.available() && index < bufferSize)   //while there are bytes available and index is less than buffer size
    {
        result[index] = Wire.read();                //receive a byte at a time into the result array
        index++;
    }
    return (uint32_t) (1000000000*result[8] + 100000000*result[7] + 10000000*result[6] + 1000000*result[5] + 100000*result[4] + 10000*result[3] + 1000*result[2] + 100*result[1] + result[0]);       //result[8] = LVSUNOUTPUT, result[7] = LVSUNINPUT, result[6] = NUMLVSUNLEDS ,result[5] = LVSUNLEDS,result[4] = NUMLVSUN, result[3] = LVSUNPORTS, result[2] = doorlock sensors, result[1] = hw version, result[0] = sw version
}

/* helper to send a message to the I2C slave address returns the result of the transmission
* 0	SYSTEM_ERROR_NONE	                        success
* 1	SYSTEM_ERROR_I2C_BUS_BUSY	                busy timeout upon entering endTransmission()
* 2	SYSTEM_ERROR_I2C_ARBITRATION_FAILED	START   bit generation timeout
* 3	SYSTEM_ERROR_I2C_TX_ADDR_TIMEOUT	        end of address transmission timeout
* 4	SYSTEM_ERROR_I2C_FILL_DATA_TIMEOUT	        data byte transfer timeout
* 5	SYSTEM_ERROR_I2C_TX_DATA_TIMEOUT	        data byte transfer succeeded, busy timeout immediately after
* 6	SYSTEM_ERROR_I2C_STOP_TIMEOUT	            timeout waiting for peripheral to clear stop bit
* 7	SYSTEM_ERROR_INVALID_STATE	                Using endTransmission in slave mode, or I2C not enabled
* 7	SYSTEM_ERROR_NOT_ENOUGH_DATA	            Incorrect number of bytes returned (can be returned for incorrect addresss)
* 7	SYSTEM_ERROR_CANCELLED	                    Transaction aborted
* 7	SYSTEM_ERROR_INTERNAL	                    Internal error
*/
byte sendMessage(uint8_t address, byte cmd, byte channel, const char *message)
{
    byte result = 0;                                //default return is 0
    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(WireTransmission(address).timeout(100ms));
        Wire.write(cmd);                                // Send the message command
        Wire.write(channel);                            // Send the message channel
        for (size_t i = 0; i < strlen(message); i++)
        {
            Wire.write(message[i]);                     // Send the message
        }
        result = Wire.endTransmission();                // End transmission
    }
    //if (result == 0)    Log.info("Message sent successfully to address: 0x%02X len %i message %s", address, strlen(message), message);
    //else                Log.error("Failed to send message to address: 0x%02X, error code: %d", address, result);
    return result;                                 // Return the result of the transmission
}

// helper to receive a message from the I2C slave
String receiveMessage(uint8_t address, uint8_t _channel)
{
    size_t length = hubdata.portsIn;                // Set the length of the message to be received based on the number of ports in
    byte result = 0;

    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(WireTransmission(address).timeout(100ms));
        Wire.write(0xDE);                           // Send the message command
        Wire.write(_channel);                       // Send the input channel
        result = Wire.endTransmission();            // End transmission
    }

    String message = "";                            // Initialize an empty string to store the message
    if (result != 0)
    {
        Log.error("Failed to send message to address: 0x%02X, error code: %d", address, result);
        return message;                             // Return an empty string if the transmission failed
    }

    delay(10);                                      // Wait for the slave to process the command
    Wire.requestFrom(address, length);              // Request data from the I2C slave

    while (Wire.available())                        // While there is data available
    {
        char c = Wire.read();                       // Read a byte
        message += c;                               // Append the byte to the message
    }

    return message;                                 // Return the received message
}

// helper to request the status of the Hub device as bool true (1 = busy) false (0 = not busy)
bool isHUBbusy(uint8_t address)
{
    uint8_t result = 0;                             //default return is false/not busy
    Wire.requestFrom(address, 1);                   //request 1 byte from peripheral device address

    while(Wire.available())
    {
        result = Wire.read();                       //receive a byte
    }
    return (result == 1);
}

// helper to set the LVSUN Input Channels off or on - when relay controlled V059
void LVSUNInputChannelsOnOff(uint8_t address, uint8_t _channel, bool _on)
{
    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(WireTransmission(address).timeout(100ms));
        Wire.write(0xAC);                           // Send the message command
        Wire.write(_channel);                       // Send the input channel
        Wire.write(_on ? 1 : 0);                    // Send the on/off command
        Wire.endTransmission();                     // End transmission
    }
}

// helper to return Hub board temperature Pico Core V059 tested and working
float HubTemperature(uint8_t address)
{
    float temperature = 0.0;                        //default return is 0
    byte data[4] = {0};                             //buffer to hold the received data
    byte result = 0;                                //default return is 0
    WITH_LOCK(Wire)
    {
        Wire.beginTransmission(WireTransmission(address).timeout(100ms));
        Wire.write(0x0C);                           // Send the message command to request temperature
        result = Wire.endTransmission();            // End transmission
    }

    if (result != 0) return temperature;
    
    Wire.requestFrom(address, 4);                   //request 4 bytes from peripheral device address

    int i = 0;
    while(Wire.available() && i < 4)                //while there are bytes available and i is less than 4
    {
        data[i++] = Wire.read();                    //receive a byte
    }
    memcpy(&temperature, data, sizeof(data));       //copy to the float temperature variable
    return temperature;                             //return the temperature value

}

#endif //LVSUNCHARGER