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
 * In the main application file, include the library header: #include "ble_wifi_setup_manager.h"
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

#ifndef __BLE_WIFI_SETUP_H
#define __BLE_WIFI_SETUP_H

#include "Particle.h"
#include "ble_wifi_constants.h"
#include <queue>

class BLEWiFiSetupManager {

    typedef enum {
        STATE_CONFIG_SETUP = 0,
        STATE_CONFIG_IDLE,
        STATE_CONFIG_PARSE_MSG,
    } ConfigState_t;

    typedef void (provisionCb_t)(void);
    typedef std::queue<char*> device_receive_msg_queue_t;
    typedef std::queue<WiFiAccessPoint> wifi_scan_response_queue_t;

    public:
        /**
         * @brief Singleton class instance access for BLEWiFiSetupManager.
         *
         * @return BLEWiFiSetupManager&
         */
        static BLEWiFiSetupManager& instance() {
            if (!_instance) {
                _instance = new BLEWiFiSetupManager();
            }
            return *_instance;
        }

        void setup(const char* name);
        void setDeviceName(const char* name);
        void stopStartAdvertising(bool start);
        void loop();
        void status_message(const char* msg);
        void wifi_scan_handler(WiFiAccessPoint* wap);
        void ble_data_rx_handler(const uint8_t* rx_data, size_t len);

        void setProvisionCallback(provisionCb_t* cb);

    private:
        BLEWiFiSetupManager();

        // Singleton instance
        static BLEWiFiSetupManager* _instance;

        ConfigState_t config_state;
        ConfigState_t next_config_state;

        provisionCb_t *provisionCb;

        void parse_message();

        wifi_scan_response_queue_t wifi_scan_response_queue;
        device_receive_msg_queue_t device_receive_msg_queue;

        BleCharacteristic *rxCharacteristic;
        BleCharacteristic *txCharacteristic;
};

#endif  /* __BLE_WIFI_SETUP_H */
