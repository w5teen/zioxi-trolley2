/**********************************************************************************************************************************************
 * This class and library is used with P2/Photon2 that also have
 * Ethernet connectivity. It is used for the case that you want to default to Ethernet,
 * but fall back to WiFi if the device loses its Ethernet connection.
 * 
 * This requires additional hardware such as the Ethernet FeatherWing or the equivalent WizNET W5500 board.
 * 
 * This class is necessary because Device OS will default to Ethernet, however it will
 * only fall back to WiFi if Ethernet is unplugged (no link or no DHCP). The 
 * built-in behaviour will not switch if the Ethernet LAN loses Internet connectivity and
 * is only providing local LAN access. By adding this class, you can make the device
 * fall back to WiFi on losing Internet on the Ethernet LAN.
 * 
 * One caveat to this is that only one network interface can be active at a time. You should
 * expect that it will take a minute or two to fall back to WiFi and connect (depending
 * on settings). The default is to check every 5 minutes if Ethernet has come back. The
 * caveat to this is that cloud connectivity is lost during the retry attempts, so you 
 * don't want to make the period too short, but making it very long will cause you to stay
 * on WiFi longer than necessary.
 * 
 * Credit to Rickkas7 for EthernetCellularRK library this is based upon.
 * 
 * In the main application file, include the library header: #include "EthernetWiFi.h"
 * 
 * You must use system thread enabled (deprecated 6.2.0 on) and SEMI_AUTOMATIC mode for this library to function properly!
 * 
 * This class is a singleton; you do not create one as a global, on the stack, or with new.
 * 
 * From application setup() you must call: EthernetWiFi::instance().setup();
 * 
 * From application loop() you must call: EthernetWiFi::instance().loop();
 * 
 * If you want to override default settings, use methods like: EthernetWiFi::instance().withRetryEthernetPeriod(10min);
 * 
 * Modified for try Ethernet to replace cyan blinking or breathing of the RGB status LED with yellow. 
 * WiFi is kept as the cyan color.
 ********************************************************************************************************************************************/

#ifndef __ETHERNETWIFI_H
#define __ETHERNETWIFI_H

#include "Particle.h"

class EthernetWiFi {
public:
    enum class ActiveInterface : int {OFF = -1, NONE = 0, ETHERNET, WIFI};

    /**
     * @brief Gets the singleton instance of this class, allocating it if necessary
     * 
     * Use EthernetWiFi::instance() to instantiate the singleton.
     */
    static EthernetWiFi &instance();

    /**
     * @brief Perform setup operations; call this from global application setup()
     * 
     * You typically use EthernetWiFi::instance().setup();
     */
    void setup();

    /**
     * @brief Perform application loop operations; call this from global application loop()
     * 
     * You typically use EthernetWiFi::instance().loop();
     */
    void loop();

    /**
     * @brief Set an Ethernet keep-alive value (default: 25 seconds)
     * 
     * @param value The value as a chrono-literal, such as 25s for 25 seconds or 5min for 5 minutes.
     * 
     * When the network is Ethernet, a keep-alive is required to keep the UDP port forwarding active so the
     * cloud can communicate with the device. This could be as short as 30 seconds or be minutes or even
     * hours. Since the data in Ethernet is rarely metered, this defaults to 25 seconds but can be made
     * longer on a site-specific basis.
     * 
     * If the limit is too long, at the end of the period the cloud will not longer be able to communicate
     * with the device if the connection is otherwise idle.
     * 
     * If you are publishing, reading variables, or calling functions, this will reset the keep-alive timer
     * so if you are frequently sending data they keep-alive may never be reached.
     * 
     * If you want to pass in a regular integer (`int`) instead of a chrono literal, you can use a construct
     * like: int keepAliveSec = 300;
     * 
     * EthernetWiFi::instance().withEthernetKeepAlive(std::chrono::seconds(keepAliveSec));
     */
    EthernetWiFi &withEthernetKeepAlive(std::chrono::seconds value) { ethernetKeepAlive = value; return *this; };

    /**
     * @brief Returns the Ethernet keep-alive value (in seconds)
     */
    int getEthernetKeepAlive() const { return (int)ethernetKeepAlive.count(); };

    /**
     * @brief Set an WiFi keep-alive value (default: 25 seconds)
     * 
     * @param value The value as a chrono-literal, such as 25s for 25 seconds.
     * 
     * When the network is Cellular, a keep-alive is required to keep the UDP port forwarding active so the
     * cloud can communicate with the device. This rarely needs to be set when using the Particle SIM, 
     * but is almost certainly required for a 3rd-party SIM. With a 3rd-party SIM the value could be as low 
     * as 30 seconds, but could be a few minutes.
     * 
     * If the limit is too long, at the end of the period the cloud will not longer be able to communicate
     * with the device if the connection is otherwise idle.
     * 
     * If you are publishing, reading variables, or calling functions, this will reset the keep-alive timer
     * so if you are frequently sending data they keep-alive may never be reached.
     * 
     * If you want to pass in a regular integer (`int`) instead of a chrono literal, you can use a construct
     * like: int keepAliveSec = 300;
     * 
     * EthernetWiFi::instance().withEthernetKeepAlive(std::chrono::seconds(keepAliveSec));
     */
    EthernetWiFi &withWiFiKeepAlive(std::chrono::seconds value) { WiFiKeepAlive = value; return *this; };

    /**
     * @brief Returns the WiFi keep-alive value (in seconds)
     */
    int getWiFiKeepAlive() const { return (int)WiFiKeepAlive.count(); };

    /**
     * @brief Set the period to try switching back to Ethernet from WiFi. Default: 5 minutes.
     * 
     * @param value The value as a chrono-literal, such as 25s for 25 seconds or 5min for 5 minutes.
     * 
     * Because the Ethernet and WiFi networks cannot be on at the same time, after switching to WiFi
     * backup we need to periodically switch back to Ethernet to see if it's back up. This will interrupt
     * network connectivity for a short period of time, so you don't want to make it too short. However,
     * you also won't switch back until this period is reached, so you'll stay on backup longer than 
     * necessary if you set it too long. The default is 5 minutes.
     * 
     * If you want to pass in a regular integer (`int`) instead of a chrono literal, you can use a construct
     * like: int retryEthernetSecs = 300;
     * 
     * EthernetWiFi::instance().withRetryEthernetPeriod(std::chrono::milliseconds(retryEthernetSecs));
     */ 
    EthernetWiFi &withRetryEthernetPeriod(std::chrono::milliseconds value) { retryEthernetPeriod = value; return *this; };

    /**
     * @brief Returns the  period to try switching back to Ethernet from WiFi (in milliseconds)
     */
    int getRetryEthernetPeriod() const { return (int)retryEthernetPeriod.count(); };

    /**
     * @brief Set the maximum time to connect to WiFi (blinking green). Default: 5 minutes.
     * 
     * @param value The value as a chrono-literal, such as 25s for 25 seconds or 5min for 5 minutes.
     * 
     * The recommended value is from 5 to 10 minutes. Setting it to short values may prevent ever being able to connect
     * to WiFi.
     * 
     * If you want to pass in a regular integer (`int`) instead of a chrono literal, you can use a construct
     * like: int timeoutMillisecs = 60000;
     * 
     * EthernetWiFi::instance().withWiFiConnectTimeout(std::chrono::milliseconds(timeoutMillisecs));
     */ 
    EthernetWiFi &withWiFiConnectTimeout(std::chrono::milliseconds value) { WiFiConnectTimeout = value; return *this; };

    /**
     * @brief Returns the maximum time to connect to WiFi (blinking green) (in milliseconds)
     */
    int getWiFiConnectTimeout() const { return (int)WiFiConnectTimeout.count(); };

    /**
     * @brief Set the maximum time to connect to the cloud while connected to WiFi (blinking cyan). Default: 2 minutes.
     * 
     * @param value The value as a chrono-literal, such as 25s for 25 seconds or 5min for 5 minutes.
     * 
     * The default is 2 minutes. It's normally fast, but in areas with low signal strength it make take longer.
     * 
     * If you want to pass in a regular integer (`int`) instead of a chrono literal, you can use a construct
     * like: int timeoutMillisecs = 60000;
     * 
     * EthernetWiFi::instance().withWiFiCloudConnectTimeout(std::chrono::milliseconds(timeoutMillisecs));
     */ 
    EthernetWiFi &withWiFiCloudConnectTimeout(std::chrono::milliseconds value) { WiFiCloudConnectTimeout = value; return *this; };

    /**
     * @brief Returns the maximum time to connect to the cloud while connected to WiFi (blinking cyan) (in milliseconds)
     */
    int getWiFiCloudConnectTimeout() const { return (int)WiFiCloudConnectTimeout.count(); };

    /**
     * @brief Set the maximum time to connect to Ethernet (blinking green). Default: 30 seconds.
     * 
     * @param value The value as a chrono-literal, such as 25s for 25 seconds or 5min for 5 minutes.
     * 
     * This should normally be really fast, but if for some reason your Ethernet network takes a long
     * time to establish a connection you could make the timeout longer.
     * 
     * If you want to pass in a regular integer (`int`) instead of a chrono literal, you can use a construct
     * like:
     * 
     * ```
     * int timeoutMillisecs = 60000;
     * 
     * EthernetWiFi::instance().withEthernetConnectTimeout(std::chrono::milliseconds(timeoutMillisecs));
     * ```
     */ 
    EthernetWiFi &withEthernetConnectTimeout(std::chrono::milliseconds value) { ethernetConnectTimeout = value; return *this; };

    /**
     * @brief Returns the maximum time to connect to Ethernet (blinking green) (in milliseconds)
     */
    int getEthernetConnectTimeout() const { return (int)ethernetConnectTimeout.count(); };

    /**
     * @brief Set the maximum time to connect to the cloud while connected to Ethernet (blinking cyan). Default: 30 seconds.
     * 
     * @param value The value as a chrono-literal, such as 25s for 25 seconds or 5min for 5 minutes.
     * 
     * The default is 30 seconds. It should normally only take a few seconds.
     * 
     * If you want to pass in a regular integer (`int`) instead of a chrono literal, you can use a construct
     * like: int timeoutMillisecs = 60000;
     * 
     * EthernetWiFi::instance().withEthernetCloudConnectTimeout(std::chrono::milliseconds(timeoutMillisecs));
     */ 
    EthernetWiFi &withEthernetCloudConnectTimeout(std::chrono::milliseconds value) { ethernetCloudConnectTimeout = value; return *this; };

    /**
     * @brief Returns the maximum time to connect to the cloud while connected to Ethernet (blinking cyan) (in milliseconds)
     */
    int getEthernetCloudConnectTimeout() const { return (int)ethernetCloudConnectTimeout.count(); };

    /**
     * @brief Sets the status LED color when using WiFi backup, replacing blinking or breathing cyan (default: yellow)
     * 
     * @param value Color value, such as RGB_COLOR_CYAN, RGB_COLOR_YELLOW, or RGB_COLOR_ORANGE
     * 
     * When switching to WiFi backup, the cloud connection color (typically blinking cyan, followed
     * by breathing cyan) can be overridden. The default in this class is to use yellow (blinking yellow
     * followed by breathing yellow) when on WiFi backup so you can tell it's on backup, and that 
     * color is not currently used by Device OS.
     * 
     * If you don't want the status LED color to be overridden, make this call with no parameter or
     * use `RGB_COLOR_CYAN` to use the Device OS default.
     * 
     * Note that this does not override the blinking green (connecting to network) which will be blinking green
     * for both WiFi and Ethernet, however it's normally not in this state for very long and there are
     * only so many available colors.
     */
    EthernetWiFi &withWiFiBackupColor(uint32_t value = RGB_COLOR_CYAN) { WiFiBackupColor = value; return *this; };

    /**
     * @brief Returns the status LED color when using WiFi backup
     * 
     * Value is an RGB color as a uint32_t of the form 0x00RRGGBB, matching the format of constants like
     * RGB_COLOR_CYAN, RGB_COLOR_YELLOW, or RGB_COLOR_ORANGE.
     */
    uint32_t getWiFiBackupColor() const { return WiFiBackupColor; };

    /**
     * @brief Returns the enumeration for the currently active interface (OFF, NONE, WiFi, or ETHERNET)
     */
    int getActiveInterface() const { return (int) activeInterface; };

    /**
     * @brief Sets interface to OFF if false or automatically selected (NONE, WiFi, or ETHERNET)
     */
    EthernetWiFi &setAutomaticInterface(bool value = true) {automaticSelection = value; return *this;};

    /**
     * @brief Sets a notification callback for when the active interface changes
     * 
     * You can only have one interface change callback.
     */
    EthernetWiFi &withInterfaceChangeCallback(std::function<void(ActiveInterface oldInterface, ActiveInterface newInterface)> callback)
    {
        interfaceChangeCallback = callback;
        return *this;
    }

protected:
    /**
     * @brief The constructor is protected because the class is a singleton
     * 
     * Use EthernetWiFi::instance() to instantiate the singleton.
     */
    EthernetWiFi();

    /**
     * @brief The destructor is protected because the class is a singleton and cannot be deleted
     */
    virtual ~EthernetWiFi();

    /**
     * This class is a singleton and cannot be copied
     */
    EthernetWiFi(const EthernetWiFi&) = delete;

    /**
     * This class is a singleton and cannot be copied
     */
    EthernetWiFi& operator=(const EthernetWiFi&) = delete;

    /**
     * @brief Sets the active interface and calls the notification handler if necessary
     * 
     * Internal use only
     */
    void setActiveInterface(ActiveInterface newActiveInterface);

    /**
     * @brief Starting state at boot
     * 
     * Next state:
     * - stateTryEthernet if Ethernet is present
     * - stateTryWiFi if no Ethernet is present
     */
    void stateStart();

    /**
     * @brief Starting point for switching to Ethernet
     * 
     * - Disconnects from WiFi
     * - Connects to Ethernet
     * - Sets the cloud connection RGB status LED theme to default (cyan)
     * 
     * Next State:
     * - stateWaitEthernetReady (always)
     */
    void stateTryEthernet();

    /**
     * @brief Waits until Ethernet is ready or times out
     * 
     * If ready:
     * - Does a Particle.connect to reconnect to the Particle cloud
     * 
     * Next State:
     * - stateWaitEthernetCloud if ready
     * - stateTryWiFi if ethernetConnectTimeout is reached without ready
     */
    void stateWaitEthernetReady();

    /**
     * @brief Waits until cloud connected while on Ethernet
     * 
     * If connected;
     * - Sets the keep-alive to ethernetKeepAlive
     * 
     * Next State:
     * - stateEthernetCloudConnected if connected to the Particle cloud
     * - stateTryWiFi if timeout of ethernetCloudConnectTimeout is reached
     * 
     * This is separate from connecting to Ethernet, because it's possible for 
     * Ethernet to be ready if the local LAN is up, but not be able to reach 
     * the Particle cloud if the Internet connection to the LAN is down. 
     */
    void stateWaitEthernetCloud();

    /**
     * @brief State for when cloud connected
     * 
     * Next State;
     * - stateWaitEthernetCloud if cloud connection is lost
     */
    void stateEthernetCloudConnected();


    /**
     * @brief Starting point for switching to WiFi
     * 
     * - Disconnects from Ethernet
     * - Connects to WiFi
     * - Sets the cloud connection RGB status LED theme
     * 
     * Next State:
     * - stateWaitWiFiReady (always)
     */
    void stateTryWiFi();


    /**
     * @brief Waits until WiFi is ready or times out
     * 
     * If ready:
     * - Does a Particle.connect to reconnect to the Particle cloud
     * 
     * Next State:
     * - stateWaitWiFiCloud if ready
     * - stateTryEthernet if WiFiConnectTimeout is reached without ready and ethernetPresent is true
     * 
     * If there is no Ethernet controller, this state will be remained in forever until
     * connected to WiFi.
     */
    void stateWaitWiFiReady();

    /**
     * @brief Waits until cloud connected while on WiFi
     * 
     * If connected;
     * - Sets the keep-alive to WiFiKeepAlive
     * 
     * Next State:
     * - stateWiFiCloudConnected if connected to the Particle cloud
     * - stateTryEthernet if WiFiConnectTimeout is reached without ready and ethernetPresent is true
     */
    void stateWaitWiFiCloud();

    /**
     * @brief State for when cloud is connected via WiFi
     * 
     * Next State;
     * - stateWaitWiFiCloud if cloud connection is lost
     * - stateWiFiWaitDisconnectedThenTryEthernet if retryEthernetPeriod is reached and ethernetPresent is true
     */
    void stateWiFiCloudConnected();

    /**
     * @brief Waits for Particle.connected to be false then tries Ethernet again
     * 
     * Next state: 
     * - stateTryEthernet after Particle.connected() is false
     */
    void stateWiFiWaitDisconnectedThenTryEthernet();

    /**
     * @brief do nothing whilst automaticSelection is false
     * 
     * Next state: 
     *
     * - if automaticSelection is true, stateStart
     */
    void stateDoNotConnect();

    /**
     * @brief State handler method
     * 
     * This is one of the protected methods like stateStart.
     */
    std::function<void(EthernetWiFi &)> stateHandler = &EthernetWiFi::stateStart;

    /**
     * @brief Set during stateStart if Ethernet hardware is detected
     */
    bool ethernetPresent = false;

    /**
     * @brief Used for determining how long to wait in a state
     * 
     * All calculations using stateTime are safe for when millis rolls over to 0 every 49 days.
     */
    unsigned long stateTime = 0;

    /**
     * @brief Ethernet keep-alive value (default: 25 seconds)
     * 
     * When the network is Ethernet, a keep-alive is required to keep the UDP port forwarding active so the
     * cloud can communicate with the device. This could be as short as 30 seconds or be minutes or even
     * hours. Since the data in Ethernet is rarely metered, this defaults to 25 seconds but can be made
     * longer on a site-specific basis.
     * 
     * If the limit is too long, at the end of the period the cloud will not longer be able to communicate
     * with the device if the connection is otherwise idle.
     * 
     * If you are publishing, reading variables, or calling functions, this will reset the keep-alive timer
     * so if you are frequently sending data they keep-alive may never be reached.
     */    
    std::chrono::seconds ethernetKeepAlive = 25s;

    /**
     * @brief WiFi keep-alive value (default: 25 seconds)
     * 
     * A keep-alive is used to implement "UDP hole punching" which helps maintain the connection from the
     * cloud to the device. A temporary port-forwarded back-channel is set up by the network to allow
     * packets to be sent from the cloud to the device. As this is a finite resource, unused back-channels
     * are periodically deleted by the network.
     * 
     * Should a device become unreachable from the cloud (such as a timed out function call or variable get),
     * one possible cause of this is that the keep alives have not been sent often enough. For WiFi, the
     * keep-alive is not generally needed. However, in unusual networking situations if the network
     * router/firewall removes the port forwarded back-channels unusually aggressively, you may need to use a keep-alive.
     * 
     * If you are publishing, reading variables, or calling functions, this will reset the keep-alive timer
     * so if you are frequently sending data they keep-alive may never be reached.
     */
    std::chrono::seconds WiFiKeepAlive = 25s;

    /**
     * Period to try switching back to Ethernet from WiFi. Default: 5 minutes.
     *      * 
     * Because the Ethernet and WiFi networks cannot be on at the same time, after switching to WiFi
     * backup we need to periodically switch back to Ethernet to see if it's back up. This will interrupt
     * network connectivity for a short period of time, so you don't want to make it too short. However,
     * you also won't switch back until this period is reached, so you'll stay on backup longer than 
     * necessary if you set it too long. The default is 5 minutes.
     */ 
    std::chrono::milliseconds retryEthernetPeriod = 5min;

    /**
     * @brief The maximum time to connect to WiFi (blinking green). Default: 2 minutes.
     * 
     * 
     * The recommended value is from 0 to 2 minutes. Setting it to short values may prevent ever being able to connect
     * to WiFi.
     */ 
    std::chrono::milliseconds WiFiConnectTimeout = 2min;

    /**
     * @brief The maximum time to connect to the cloud while connected to WiFi (blinking cyan). Default: 2 minutes.
     *      * 
     * The default is 2 minutes. It's normally fast, but in areas with low signal strength it may take longer.
     */     
    std::chrono::milliseconds WiFiCloudConnectTimeout = 2min;

    /**
     * @brief The maximum time to connect to Ethernet (blinking green). Default: 10 seconds.
     * 
     * This should normally be really fast, but if for some reason your Ethernet network takes a long
     * time to establish a connection you could make the timeout longer.
     * If adaptor present but not connected then switch to backup net will happen after this time.
     */ 
    std::chrono::milliseconds ethernetConnectTimeout = 10s;

    /**
     * @brief The maximum time to connect to the cloud while connected to Ethernet (blinking cyan). Default: 60 seconds.
     *  
     * The default is 60 seconds. It should normally only take a few seconds.
     */     
    std::chrono::milliseconds ethernetCloudConnectTimeout = 60s;

    /**
     * @brief The status LED color when using WiFi backup, replacing blinking or breathing cyan 
     * 
     * When switching to WiFi backup, the cloud connection color (typically blinking cyan, followed
     * by breathing cyan) can be overridden. The default in this class is to use yellow (blinking yellow
     * followed by breathing yellow) when on WiFi backup so you can tell it's on backup, and that 
     * color is not currently used by Device OS.
     * 
     * If you don't want the status LED color to be overridden, make this call with no parameter or
     * use `RGB_COLOR_CYAN` to use the Device OS default.
     * 
     * Note that this does not override the blinking green (connecting to network) which will be blinking green
     * for both WiFi and Ethernet, however it's normally not in this state for very long and there are
     * only so many available colors.
     */    
    uint32_t WiFiBackupColor = RGB_COLOR_CYAN;

    /**
     * @brief The currently active interface
     */
    ActiveInterface activeInterface = ActiveInterface::NONE;

    /**
     * @brief Automatic interface selection (default is true), false will set interface to NONE and turm off trying to connect
     */
    bool automaticSelection = true;

    /**
     * @brief Optional callback function to call when the active interface changes
     */
    std::function<void(ActiveInterface oldInterface, ActiveInterface newInterface)> interfaceChangeCallback = NULL;

    /**
     * @brief Singleton instance of this class
     * 
     * The object pointer to this class is stored here. It's NULL at system boot.
     */
    static EthernetWiFi *_instance;
};

#endif /* __ETHERNETWIFI_H */