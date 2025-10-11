#ifndef __WIFICHANNELRK_H
#define __WIFICHANNELRK_H

#include "Particle.h"

/**
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 * 
 * From global application setup you must call:
 * WiFiChannelRK::instance().setup();
 */
class WiFiChannelRK {
public:
    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use WiFiChannelRK::instance() to instantiate the singleton.
     */
    static WiFiChannelRK &instance();

    /**
     * @brief Perform setup operations; call this from global application setup()
     * 
     * You typically use WiFiChannelRK::instance().setup();
     */
    void setup();

    /**
     * @brief Optional function to register a callback to be called when the channel is known
     * 
     * @param cb 
     * @return WiFiChannelRK& 
     * 
     * The callback function has this prototype:
     * 
     * void callback()
     * 
     * For example usage, see 1-simple.cpp that registers a function and prints the results to USB debugging.
     */
    WiFiChannelRK &withUpdateCallback(std::function<void()> cb) { updateCallback = cb; return *this; };

    /**
     * @brief Set the delay from WiFi.ready() to doing the WiFi.scan(). Default: 4 seconds
     * 
     * @param scanDelay 
     * @return WiFiChannelRK& 
     */
    WiFiChannelRK &withScanDelay(std::chrono::milliseconds scanDelay) { this->scanDelay = scanDelay; return *this; };

    /**
     * @brief Locks the mutex that protects shared resources
     * 
     * This is compatible with `WITH_LOCK(*this)`.
     * 
     * The mutex is not recursive so do not lock it within a locked section.
     */
    void lock() { os_mutex_lock(mutex); };

    /**
     * @brief Attempts to lock the mutex that protects shared resources
     * 
     * @return true if the mutex was locked or false if it was busy already.
     */
    bool tryLock() { return os_mutex_trylock(mutex); };

    /**
     * @brief Unlocks the mutex that protects shared resources
     */
    void unlock() { os_mutex_unlock(mutex); };

    /**
     * @brief Returns the current Wi-Fi channel connected to
     * 
     * @return int channel number or 0 if not known
     */
    int getWiFiChannel() const { return channel; };

    /**
     * @brief Returns true if the current WiFi channel is known or false if not
     * 
     * @return true 
     * @return false 
     */
    bool hasWiFiChannel() const { return channel != 0; };

    /**
     * @brief Returns true if the currently connected channel is a 2.4 GHz channel
     * 
     * @return true 
     * @return false 
     */
    bool isWiFi_2_4_GHz() const;

    /**
     * @brief Returns true if the currently connected channel is a 5 GHz channel
     * 
     * @return true 
     * @return false 
     */
    bool isWiFi_5_GHz() const;

protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use WiFiChannelRK::instance() to instantiate the singleton.
     */
    WiFiChannelRK();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~WiFiChannelRK();

    /**
     * This class is a singleton and cannot be copied
     */
    WiFiChannelRK(const WiFiChannelRK&) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    WiFiChannelRK& operator=(const WiFiChannelRK&) = delete;

    /**
     * @brief Worker thread function
     * 
     * This method is called to perform operations in the worker thread.
     * 
     * You generally will not return from this method.
     */
    os_thread_return_t threadFunction(void);

    /**
     * @brief Mutex to protect shared resources
     * 
     * This is initialized in setup() so make sure you call the setup() method from the global application setup.
     */
    os_mutex_t mutex = 0;

    /**
     * @brief Worker thread instance class
     * 
     * This is initialized in setup() so make sure you call the setup() method from the global application setup.
     */
    Thread *thread = 0;

    /**
     * @brief Set to true if WiFi.ready() was most recently true. Used to check for connected state change.
     */
    bool wifiReady = false;
    
    /**
     * @brief The BSSID of the AP that we are connected to
     * 
     */
    byte bssid[6];

    /**
     * @brief The last channel number found, or 0 if not known
     */
    int channel = 0;

    /**
     * @brief Used for delaying a short period after WiFi.ready (see scanDelay)
     */
    unsigned long stateTime = 0;

    /**
     * @brief Amount of time to wait after WiFi.ready() before doing WiFi.scan()
     */
    std::chrono::milliseconds scanDelay = 4s;

    /**
     * @brief State handler when idle
     */
    void stateIdle();

    /**
     * @brief State handler when waiting to scan and during scan
     */
    void stateScanWait();

    /**
     * @brief Finite state machine state handler
     */
    std::function<void(WiFiChannelRK&)> stateHandler = &WiFiChannelRK::stateIdle;

    /**
     * @brief Internal method called when a Wi-Fi access point is found during WiFi.scan
     * 
     * @param wap Information about this Wi-Fi access point (SSID, BSSID, channel, RSSI) from the Device OS API
     */
    void wifiScanCallback(WiFiAccessPoint* wap);

    /**
     * @brief Internal static method called when a Wi-Fi access point is found during WiFi.scan
     * 
     * @param wap Information about this Wi-Fi access point (SSID, BSSID, channel, RSSI) from the Device OS API
     * 
     * @param data The data is not used to find the object instance; this object is a singleton so it's found from that.
     */
    static void wifiScanCallbackStatic(WiFiAccessPoint* wap, void* data);

    /**
     * @brief Function called after the channel is known or has changed
     */
    std::function<void()> updateCallback = nullptr;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static WiFiChannelRK *_instance;

};
#endif  /* __WIFICHANNELRK_H */