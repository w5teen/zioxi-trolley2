#include "EthernetWiFi.h"

EthernetWiFi *EthernetWiFi::_instance;

static Logger _log("app.ethwifi");

EthernetWiFi &EthernetWiFi::instance()
{
    if (!_instance)
    {
        _instance = new EthernetWiFi();
    }
    return *_instance;
}

EthernetWiFi::EthernetWiFi()
{
}

EthernetWiFi::~EthernetWiFi()
{
}

void EthernetWiFi::setup()
{
}

void EthernetWiFi::loop()
{
    stateHandler(*this);
}

void EthernetWiFi::setActiveInterface(ActiveInterface newActiveInterface)
{
    ActiveInterface oldActiveInterface = activeInterface;
    activeInterface = newActiveInterface;

    if (oldActiveInterface != newActiveInterface)
    {
        if (interfaceChangeCallback) {
            interfaceChangeCallback(oldActiveInterface, newActiveInterface);
        }
    }
}

void EthernetWiFi::stateStart()
{
    if (!System.featureEnabled(FEATURE_ETHERNET_DETECTION))
    {
        _log.info("FEATURE_ETHERNET_DETECTION enabled (was disabled before)");
        System.enableFeature(FEATURE_ETHERNET_DETECTION);
    }

    uint8_t mac[6] = {0};
    ethernetPresent = false;
    if (Ethernet.macAddress(mac) != 0)
    {
        _log.info("Ethernet adapter present");

        ethernetPresent = true;
    }

    if (automaticSelection)
    {
        if (ethernetPresent)
        {
            stateHandler = &EthernetWiFi::stateTryEthernet;
        }
        else
        {        
            stateHandler = &EthernetWiFi::stateTryWiFi;
        }
    }
    else
    {
        _log.info("automaticSelection false - operate with no connection");
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        setActiveInterface(ActiveInterface::OFF);
    }
}

void EthernetWiFi::stateTryEthernet()
{
    _log.info("Trying to connect by Ethernet");
    LEDSystemTheme theme;
    theme.setColor(LED_SIGNAL_CLOUD_CONNECTING, RGB_COLOR_YELLOW);
    theme.setColor(LED_SIGNAL_CLOUD_HANDSHAKE, RGB_COLOR_YELLOW);
    theme.setColor(LED_SIGNAL_CLOUD_CONNECTED, RGB_COLOR_YELLOW);
    theme.apply(); 
    //LEDSystemTheme::restoreDefault();
    setActiveInterface(ActiveInterface::NONE);

    stateTime = millis();
    WiFi.disconnect();
    Ethernet.connect();
    stateHandler = &EthernetWiFi::stateWaitEthernetReady;
}

void EthernetWiFi::stateWaitEthernetReady()
{
    if (automaticSelection)
    {
        if (Ethernet.ready())
        {
            // Have Ethernet link, try connecting to the Particle cloud
            setActiveInterface(ActiveInterface::ETHERNET);
            Particle.connect();
            stateHandler = &EthernetWiFi::stateWaitEthernetCloud;
            return;
        }
        if (millis() - stateTime >= ethernetConnectTimeout.count())
        {
            // Timed out connecting to Ethernet (no DHCP, for example)
            setActiveInterface(ActiveInterface::NONE);
            _log.info("Timed out connecting to Ethernet, reverting to WiFi");
            stateHandler = &EthernetWiFi::stateTryWiFi;
            return;
        }
        // Wait some more
    }
    else
    {
        _log.info("automation Selection now false");
        Ethernet.disconnect();
        setActiveInterface(ActiveInterface::OFF);
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        return;
    }
}

void EthernetWiFi::stateWaitEthernetCloud()
{
    if (automaticSelection)
    {
        if (Particle.connected())
        {
            _log.info("Cloud connected over Ethernet keepAlive=%d", (int) ethernetKeepAlive.count());
            setActiveInterface(ActiveInterface::ETHERNET);
            Particle.keepAlive(ethernetKeepAlive.count());
            stateTime = millis();
            stateHandler = &EthernetWiFi::stateEthernetCloudConnected;
            return;  
        }

        if (millis() - stateTime >= ethernetCloudConnectTimeout.count())
        {
            _log.info("Took too long to connect to the cloud by Ethernet, switching to WiFi");
            Particle.disconnect();
            Ethernet.disconnect();
            stateHandler = &EthernetWiFi::stateTryWiFi;
            return;
        }
        // Wait some more
    }
    else
    {
        _log.info("automation Selection now false");
        Ethernet.disconnect();
        setActiveInterface(ActiveInterface::OFF);
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        return;
    }
}

void EthernetWiFi::stateEthernetCloudConnected()
{
    if (automaticSelection)
    {
        if (!Particle.connected())
        {
            _log.info("Disconnected from the cloud while on Ethernet, waiting for reconnect");
            setActiveInterface(ActiveInterface::NONE);
            stateHandler = &EthernetWiFi::stateWaitEthernetCloud;
            stateTime = millis();
            return;
        }
    }
    else
    {
        _log.info("automation Selection now false");
        Particle.disconnect();
        Ethernet.disconnect();
        setActiveInterface(ActiveInterface::OFF);
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        return;
    }
}

void EthernetWiFi::stateTryWiFi()
{
    _log.info("Trying to connect by WiFi");
    setActiveInterface(ActiveInterface::NONE);

    /*
    //When in WiFi backup mode, show breathing yellow instead of breathing cyan when cloud connected
    if (WiFiBackupColor != RGB_COLOR_CYAN)
    {
        LEDSystemTheme theme;
        theme.setColor(LED_SIGNAL_CLOUD_CONNECTING, WiFiBackupColor);
        theme.setColor(LED_SIGNAL_CLOUD_HANDSHAKE, WiFiBackupColor);
        theme.setColor(LED_SIGNAL_CLOUD_CONNECTED, WiFiBackupColor);
        theme.apply(); 
    }
    else
    {
        LEDSystemTheme::restoreDefault();
    } */
    LEDSystemTheme::restoreDefault();

    stateTime = millis();
    Ethernet.disconnect();
    WiFi.connect();
    stateHandler = &EthernetWiFi::stateWaitWiFiReady;
}

void EthernetWiFi::stateWaitWiFiReady()
{
    if (automaticSelection)
    {
        if (WiFi.ready())
        {
            // Have WiFi link, try connecting to the Particle cloud
            setActiveInterface(ActiveInterface::WIFI);
            Particle.connect();
            stateHandler = &EthernetWiFi::stateWaitWiFiCloud;
            return;
        }
        if (millis() - stateTime >= WiFiConnectTimeout.count())
        {
            // Timed out connecting to WiFi (no WAP, for example)
            if (ethernetPresent)
            {
                // Try going back to Ethernet
                setActiveInterface(ActiveInterface::NONE);
                stateHandler = &EthernetWiFi::stateTryEthernet;
                return;
            }
            // No Ethernet to go back to, just keep waiting
        }
        // Wait some more    
    }
    else
    {
        _log.info("automation Selection now false");
        setActiveInterface(ActiveInterface::OFF);
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        return;
    }
}

void EthernetWiFi::stateWaitWiFiCloud()
{
    if (automaticSelection)
    {
        if (Particle.connected())
        {
            _log.info("Cloud connected over WiFi keepAlive=%d", (int) WiFiKeepAlive.count());
            Particle.keepAlive(WiFiKeepAlive.count());
            setActiveInterface(ActiveInterface::WIFI);

            stateTime = millis();
            stateHandler = &EthernetWiFi::stateWiFiCloudConnected;
            return;  
        }

        if (millis() - stateTime >= WiFiCloudConnectTimeout.count())
        {
            if (ethernetPresent)
            {
                _log.info("Took too long to connect to the cloud by WiFi, trying Ethernet again");
                Particle.disconnect();
                setActiveInterface(ActiveInterface::NONE);
                stateHandler = &EthernetWiFi::stateTryEthernet;
                return;
            }
        }
        // Wait some more
    }
    else
    {
        _log.info("automation Selection now false");
        Particle.disconnect();
        WiFi.disconnect();
        setActiveInterface(ActiveInterface::OFF);
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        return;
    }
}

void EthernetWiFi::stateWiFiCloudConnected()
{
    if (automaticSelection)
    {
        if (!Particle.connected())
        {
            _log.info("Disconnected from the cloud while on WiFi");
            setActiveInterface(ActiveInterface::NONE);
            stateHandler = &EthernetWiFi::stateWaitWiFiCloud;
            stateTime = millis();
            return;
        }

        if (millis() - stateTime >= retryEthernetPeriod.count())
        {
            if (ethernetPresent)
            {
                _log.info("Trying Ethernet again");
                Particle.disconnect();
                setActiveInterface(ActiveInterface::NONE);

                // We were really cloud connected before, so disconnecting will take
                // a non-zero amount of time. This does not happen when going
                // from Ethernet to WiFi for a failed connection, as the
                // connection hasn't been made yet so there's nothing to tear down.
                stateHandler = &EthernetWiFi::stateWiFiWaitDisconnectedThenTryEthernet;
                return;
            }
        }
    }
    else
    {
        _log.info("automation Selection now false");
        Particle.disconnect();
        WiFi.disconnect();
        setActiveInterface(ActiveInterface::OFF);
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        return;
    }
}

void EthernetWiFi::stateWiFiWaitDisconnectedThenTryEthernet()
{
    if (automaticSelection)
    {
        if (Particle.disconnected())
        {
            _log.info("WiFi off Try Ethernet instead");
            stateHandler = &EthernetWiFi::stateTryEthernet;
            return;
        }
    }
    else
    {
        _log.info("automation Selection now false");
        Particle.disconnect();
        WiFi.disconnect();
        setActiveInterface(ActiveInterface::OFF);
        stateHandler = &EthernetWiFi::stateDoNotConnect;
        return;
    }
}

void EthernetWiFi::stateDoNotConnect()
{
    if (automaticSelection)
    {
        _log.info("automation Selection now true");
        setActiveInterface(ActiveInterface::NONE);
        stateHandler = &EthernetWiFi::stateStart;
        return;
    }    // else do nothing
}
