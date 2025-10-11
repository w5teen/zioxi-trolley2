#include "Particle.h"
#include "WiFiChannelRK.h"

// System thread defaults to on in 6.2.0 and later and this line is not required
#ifndef SYSTEM_VERSION_v620
SYSTEM_THREAD(ENABLED);
#endif

SYSTEM_MODE(SEMI_AUTOMATIC);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

void updateCallback();

void setup() {
    // Uncomment the following to wait for USB serial to be connected for debugging
    // waitFor(Serial.isConnected, 8000); delay(2000);

    // From global application setup you must call:    
    WiFiChannelRK::instance()
        .withUpdateCallback(updateCallback)
        .setup();

    Particle.connect();
}


void loop() {

}

void updateCallback() {
    Log.info("updateCallback channel=%d, 2.4GHz=%d, 5GHz=%d",
        WiFiChannelRK::instance().getWiFiChannel(),
        (int) WiFiChannelRK::instance().isWiFi_2_4_GHz(),
        (int) WiFiChannelRK::instance().isWiFi_5_GHz());
}