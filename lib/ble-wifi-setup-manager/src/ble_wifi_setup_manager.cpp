/**********************************************************************************************************************************************
 * This class and library is used with P2/Photon2 to enable WiFi credentials setup using BLE.
 * 
 * Credit to Dan Kouba for ble_wifi_setup_manager library on which this is based.
 * 
 * The software was provided under an MIT License (MIT).
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * You should use system thread ENABLED (now default) and SEMI_AUTOMATIC mode.
 * 
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 * 
 * From application setup() you must call: ble_wifi_setup_manager::instance().setup();
 * 
 * From application loop() you must call: ble_wifi_setup_manager::instance().loop();
 * 
 * Have added the ability to handle new commands to switch off WiFi use and to report the connected WiFi details.
 * Have added the ability to set the device name without setup.
 * Have added the ability to stop/start advertising.
 * 
 ********************************************************************************************************************************************/

#include "ble_wifi_setup_manager.h"
#include "EthernetWiFi.h"

Logger BLEWiFiSetupManagerLogger("app.BLEWiFiSetupManager");

BLEWiFiSetupManager *BLEWiFiSetupManager::_instance = nullptr;

static const char * const security_strings[] = {
    "Unsecured",
    "WEP",
    "WPA",
    "WPA2",
    "WPA Enterprise",
    "WPA2 Enterprise",
};

const timer_t CONNECTION_TIMEOUT = 40000UL;

// callback function for output from wifi scan
static inline void wifi_scan_callback(WiFiAccessPoint* wap, void* data)
{
    BLEWiFiSetupManager::instance().wifi_scan_handler(wap);
}

// function to handle data received via BLE
static inline void onDataReceived(const uint8_t* rx_data, size_t len, const BlePeerDevice& peer, void* self)
{
    BLEWiFiSetupManager::instance().ble_data_rx_handler(rx_data, len);
}

// Constructor
BLEWiFiSetupManager::BLEWiFiSetupManager() 
  : config_state(STATE_CONFIG_SETUP),
    next_config_state(STATE_CONFIG_SETUP),
    provisionCb(nullptr)
{}

// setup() method to be called from the main application setup()
void BLEWiFiSetupManager::setup(const char* deviceName)
{
    // Set up the BLE service and characteristics
    rxCharacteristic = new BleCharacteristic("rx", BleCharacteristicProperty::NOTIFY, readUUID, serviceUUID);
    txCharacteristic = new BleCharacteristic("tx", BleCharacteristicProperty::WRITE_WO_RSP, writeUUID, serviceUUID, onDataReceived, nullptr);

    BLE.addCharacteristic(*rxCharacteristic);
    BLE.addCharacteristic(*txCharacteristic);

    // Advertise our custom configuration service UUID so the webapp can detect compatible devices
    BleAdvertisingData advData;
    advData.appendServiceUUID(serviceUUID);
    BLEWiFiSetupManagerLogger.trace("BLE Device Name: %s", deviceName);
    advData.appendLocalName(deviceName); // This appears to stop pairing scan from working but leaves device.name as PSeries-XXXX
    BLE.setAdvertisingParameters(400, 0, BleAdvertisingEventType::CONNECTABLE_SCANNABLE_UNDIRECRED);
    BLE.setDeviceName(deviceName);
    BLE.advertise(&advData);

    BLEWiFiSetupManagerLogger.trace("Bluetooth Address: %s", BLE.address().toString().c_str());

    BLE.on();

    WiFi.on();    // WiFi must be on for this library to work
}

// setDeviceName() method to set the BLE device name
void BLEWiFiSetupManager::setDeviceName(const char* name)
{
    BLE.setDeviceName(name);
    BLEWiFiSetupManagerLogger.trace("BLE Device Name Set: %s", name);
}

// stopStartAdvertising() method to start or stop BLE advertising
void BLEWiFiSetupManager::stopStartAdvertising(bool start)
{
    if (start) {
        BLE.advertise();
        BLEWiFiSetupManagerLogger.trace("BLE Advertising Started");
    } else {
        BLE.stopAdvertising();
        BLEWiFiSetupManagerLogger.trace("BLE Advertising Stopped");
    }
}

// loop() method to be called from the main application loop()
void BLEWiFiSetupManager::loop()
{
    // Run state machine
    switch(config_state)
    {
        case STATE_CONFIG_SETUP: {
            next_config_state = STATE_CONFIG_IDLE;
            break;
        }

        case STATE_CONFIG_IDLE: {
            if (device_receive_msg_queue.empty()) {
                next_config_state = STATE_CONFIG_IDLE;
            } else {
                next_config_state = STATE_CONFIG_PARSE_MSG;
            }
            break;
        }

        case STATE_CONFIG_PARSE_MSG: {
            parse_message();
            next_config_state = STATE_CONFIG_IDLE;
            break;
        }
    }

    if (config_state != next_config_state)
    {
        BLEWiFiSetupManagerLogger.trace("State Transition: %u -> %u", config_state, next_config_state);
        config_state = next_config_state;
    }

    char tmp_buf[150];  // Need: ~64 chars + SSID length + null terminator
    while (!wifi_scan_response_queue.empty()) {
        WiFiAccessPoint ap = wifi_scan_response_queue.front();
        int len = sprintf(tmp_buf, 
            "{\"msg_t\":\"scan_resp\", \"ssid\":\"%s\", \"sec\":\"%s\", \"ch\":%d, \"rssi\":%d}", 
            ap.ssid, security_strings[(int)ap.security], (int)ap.channel, ap.rssi
        );
        rxCharacteristic->setValue((uint8_t*)tmp_buf, len);
        wifi_scan_response_queue.pop();
    }
}

// callback function for output from wifi scan
void BLEWiFiSetupManager::wifi_scan_handler(WiFiAccessPoint* wap)
{
    wifi_scan_response_queue.push(*wap);
}

// connection status message function
void BLEWiFiSetupManager::status_message(const char* msg)
{
    char tmp_buf[150];
    int len = sprintf(tmp_buf, "{\"msg_t\":\"status\", \"msg\":\"%s\"}", msg);
    BLEWiFiSetupManager::instance().rxCharacteristic->setValue((uint8_t*)tmp_buf, len);
}

// parse message function
void BLEWiFiSetupManager::parse_message()
{
    // Pull our message off the queue, copy it locally, and free the original message
    // Probbaly not ideal since we don't really need to copy, but since we parse and have conditional
    // actions later, this may prevent accidental memory leaks with the addition of other conditional paths in this code 
    // e.g. returning somewhere down below before the free() call.
    char *msg_buf = device_receive_msg_queue.front();
    BLEWiFiSetupManagerLogger.trace("Parse RX: %s", msg_buf);
    JSONValue outerObj = JSONValue::parseCopy(msg_buf);
    device_receive_msg_queue.pop();
    free(msg_buf);

    // Process our received message
    JSONObjectIterator iter(outerObj);
    while(iter.next())
    {
        BLEWiFiSetupManagerLogger.info("key=%s value=%s", 
            (const char *) iter.name(), 
            (const char *) iter.value().toString());
        
        if (iter.name() == "msg_type")
        {
            // Scan for visible WiFi Access Points (WAPs)
            if (strcmp((const char *)iter.value().toString(), "scan") == 0)
            {
                WiFi.scan(wifi_scan_callback);
                BLEWiFiSetupManagerLogger.info("WiFi Scan Complete");
            }
            // Set WiFi credentials and attempt to connect to WiFi and Particle Cloud
            else if (strcmp((const char *)iter.value().toString(), "set_creds") == 0)
            {
                JSONString ssid, pass;
                while(iter.next()) {
                    if (iter.name() == "ssid") {
                        ssid = iter.value().toString();
                        BLEWiFiSetupManagerLogger.info("Set WiFi SSID: %s", ssid.data());
                    }
                    else if (iter.name() == "password") {
                        pass = iter.value().toString();
                        BLEWiFiSetupManagerLogger.info("Set WiFi Password: %s", pass.data());
                    }
                    else {
                        BLEWiFiSetupManagerLogger.warn("Unrecognized key while parsing WiFi credentials: %s", (const char *)iter.name());
                    }
                }
                // If we have both an SSID and password, set the credentials and try to connect
                if (!ssid.isEmpty() && !pass.isEmpty())
                {
                    WiFi.setCredentials(ssid.data(), pass.data());
                    if (provisionCb != nullptr) {provisionCb();}
                    // Credentials loaded - attempt to connect to WiFi and Particle Cloud
                    if (WiFi.hasCredentials())
                    {
                        BLEWiFiSetupManagerLogger.info("WiFi credentials set");
                        status_message("WiFi Credentials Set");
                        WiFi.listen(false); //stop listening
                        WiFi.connect();
                        timer_t wtimeout = millis();
                        status_message("Connecting to WAP...");

                        while (!WiFi.ready() && (millis() - wtimeout < CONNECTION_TIMEOUT)) {};

                        if (WiFi.ready())
                        {
                            BLEWiFiSetupManagerLogger.info("WiFi Connected");
                            status_message("Connected to WAP");
                            status_message("Connecting to Cloud...");
                            Particle.connect();
                            timer_t ptimeout = millis();

                            while (!Particle.connected() && (millis() - ptimeout < CONNECTION_TIMEOUT)) {}; //Refresh Watchdog to avoid reset not possible here so increase Watchdog time to be more than CONNECTION_TIMEOUT x2

                            if (Particle.connected())
                            {
                                BLEWiFiSetupManagerLogger.info("Cloud Connected");
                                status_message("Connected to Cloud");
                            }
                            else
                            {
                                BLEWiFiSetupManagerLogger.info("Cloud Connection Timeout");
                                status_message("CloudConnection Timeout");
                            }
                        }
                        else
                        {
                        BLEWiFiSetupManagerLogger.info("WAP connection Timeout");
                        status_message("WAP Connection Timeout");
                        }
                    }
                    else
                    {
                        BLEWiFiSetupManagerLogger.info("Error WiFi credentials not set");
                        status_message("Error WiFi credentials not set");
                    }
                }
                else
                {
                    BLEWiFiSetupManagerLogger.warn("Failure parsing WiFi credentials");
                    status_message("Failure parsing WiFi credentials");
                }
            }
            // Clear WiFi credentials and disconnect from WiFi and Particle Cloud
            else if (strcmp((const char *)iter.value().toString(), "clr_creds") == 0)
            {
                if (WiFi.hasCredentials())
                {
                    BLEWiFiSetupManagerLogger.info("Has WiFi Credentials");
                    status_message("WiFi Credentials set");
                    if (Particle.connected()) {Particle.disconnect(); delay(100);}
                    if (WiFi.ready())   {WiFi.disconnect(); delay(200);}
                    else                {WiFi.on(); delay(200);}

                    WiFi.clearCredentials();
                    BLEWiFiSetupManagerLogger.info("WiFi Credentials cleared");
                    status_message("WiFi Credentials cleared");
                }
                else
                {
                    BLEWiFiSetupManagerLogger.info("No WiFi Credentials");
                    status_message("No WiFi Credentials");
                }
            }
            // Get WiFi Access Point (WAP) information
            else if (strcmp((const char *)iter.value().toString(), "get_wap") == 0)
            {
                if (WiFi.hasCredentials())
                {
                    if (WiFi.ready())
                    {
                        char tmp_buf[150];
                        bool isCloudConnected = Particle.connected();   //V102
                        int len = sprintf(tmp_buf, "{\"msg_t\":\"WiFi Credentials, Connected\", \"ssid\":\"%s\", \"cloud\":\"%s\"}", WiFi.SSID(), isCloudConnected ? "true" : "false"); //V102
                        BLEWiFiSetupManagerLogger.info("Connected to WAP: %s", tmp_buf);
                        rxCharacteristic->setValue((uint8_t*)tmp_buf, len);
                    }
                    else
                    {
                        BLEWiFiSetupManagerLogger.info("WiFi Credentials, Not Connected");
                        status_message("WiFi Credentials, Not Connected");
                    }
                }
                else
                {
                    BLEWiFiSetupManagerLogger.info("No WiFi Credentials");
                    status_message("No WiFi Credentials");
                }
            }
            // Get the operation mode [Local or Connected]
            else if (strcmp((const char *)iter.value().toString(), "get_mode") == 0)
            {
                int activenetwork = EthernetWiFi::instance().getActiveInterface();

                if (activenetwork == (int) EthernetWiFi::ActiveInterface::OFF)
                {
                    BLEWiFiSetupManagerLogger.info("Local mode");
                    status_message("Local mode operation");
                }
                else
                {
                    BLEWiFiSetupManagerLogger.info("Connected mode");
                    status_message("Connected mode operation");
                }
            }
            // Set operation mode Local
            else if (strcmp((const char *)iter.value().toString(), "set_mode_local") == 0)
            {
                BLEWiFiSetupManagerLogger.info("Set operation mode: Local");
                EthernetWiFi::instance().setAutomaticInterface(false);
                status_message("Operation mode Local");
            }
            // Set operation mode Connected
            else if (strcmp((const char *)iter.value().toString(), "set_mode_conn") == 0)
            {
                BLEWiFiSetupManagerLogger.info("Set operation mode: Connected");
                EthernetWiFi::instance().setAutomaticInterface(true);
                status_message("Operation mode Connected");
            }
            // Unrecognized message type
            else
            {
                BLEWiFiSetupManagerLogger.warn("Unrecognized message type: %s", (const char *)iter.value().toString());
            }
        }
    }
}

// The underlying BLE lib reuses the receive buffer, and will not terminate it properly for a string
// Add some manual processing and properly terminate for string parsing
void BLEWiFiSetupManager::ble_data_rx_handler(const uint8_t* rx_data, size_t len)
{
    if( len > 0 )
    {
        char *msg_buf = (char*)malloc(len+1);
        memcpy(msg_buf, rx_data, len);
        msg_buf[len] = 0;   // Null-terminate string
        device_receive_msg_queue.push(msg_buf);
        BLEWiFiSetupManagerLogger.trace("Added message to the queue: %s", msg_buf);
        return;
    }
}

// Callback function for when a new message is received
void BLEWiFiSetupManager::setProvisionCallback(provisionCb_t* cb)
{
    provisionCb = cb;
}

