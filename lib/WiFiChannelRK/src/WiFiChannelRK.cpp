#include "WiFiChannelRK.h"

WiFiChannelRK *WiFiChannelRK::_instance;

static Logger _logger("app.wifiChan");

// [static]
WiFiChannelRK &WiFiChannelRK::instance() {
    if (!_instance) {
        _instance = new WiFiChannelRK();
    }
    return *_instance;
}

WiFiChannelRK::WiFiChannelRK() {
}

WiFiChannelRK::~WiFiChannelRK() {
}

void WiFiChannelRK::setup() {
    os_mutex_create(&mutex);

    thread = new Thread("WiFiChannelRK", [this]() { return threadFunction(); }, OS_THREAD_PRIORITY_DEFAULT, 3072);
}

bool WiFiChannelRK::isWiFi_2_4_GHz() const {
    return channel >= 1 && channel <= 14;
}

bool WiFiChannelRK::isWiFi_5_GHz() const {
    return channel >= 36 && channel <= 177;
}

void WiFiChannelRK::stateIdle() {
}

void WiFiChannelRK::stateScanWait() {
    if (millis() - stateTime < scanDelay.count()) {
        return;
    }

    WiFi.BSSID(bssid);
    _logger.trace("Current BSSID %02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

    channel = 0;

    int resultCount = WiFi.scan(wifiScanCallbackStatic);
    
    _logger.trace("resultCount=%d channel=%d", resultCount, (int)channel);
    
    if (updateCallback) {
        updateCallback();
    }

    stateHandler = &WiFiChannelRK::stateIdle;

}


os_thread_return_t WiFiChannelRK::threadFunction(void) {
    while(true) {
        if (WiFi.ready()) {
            if (!wifiReady) {
                _logger.trace("Wi-Fi ready");
                wifiReady = true;
                stateTime = millis();
                stateHandler = &WiFiChannelRK::stateScanWait;
            }
        }
        else {
            if (wifiReady) {
                _logger.trace("Wi-Fi no longer ready");
                wifiReady = false;
                stateHandler = &WiFiChannelRK::stateIdle;
            }
        }
        if (stateHandler) {
            stateHandler(*this);
        }
        delay(1);
    }
}


void WiFiChannelRK::wifiScanCallback(WiFiAccessPoint* wap) {
    if (memcmp(wap->bssid, bssid, sizeof(bssid)) == 0) {
        channel = (int) wap->channel;
    }
}

// [static]
void WiFiChannelRK::wifiScanCallbackStatic(WiFiAccessPoint* wap, void* data) {
    if (_instance) {
        _instance->wifiScanCallback(wap);
    }
}