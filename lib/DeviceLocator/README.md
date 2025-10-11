# DeviceLocator

*A library for Google Maps Location API*

This library is designed for GoogleMaps API it subscribes the device to the cloud service passing the BSSIDs of local WAPs and in return the service will publish the Lat, Long coordinates of the APs or none if the APs are not in the Google database. 

## Using it

#include "device-locator.h"

Make declarations:
```
unsigned long locateupdate;
float latitude, longitude, locateAcc;
GoogleMapsDeviceLocator locator;
void locationCallback(float lat, float lon, float accuracy);
```

In setup():
```
locator.withSubscribe(locationCallback);
locator.withLocateOnce();
```

In loop():
```
checkLocation();

void checkLocation()
{
 if (millis()-locateupdate >= LCHK)
 {
  locateupdate = millis();
  if (WiFi.ready())
  {
   if (Particle.connected() && opMode == D_CONNECTED) locator.loop();
  }
 }
}
```

Note that the device must be cloud connected for the API subscribe to work. 

### 0.0.3

- Added support for WITH\_ACK mode

### 0.1.0

- Added support for other storage methods