#ifndef __DEVICE_LOCATOR_H
#define __DEVICE_LOCATOR_H

#include "Particle.h"

/**
 * This is the callback function prototype for the callback if you use the
 * withSubscribe() method to be notified of your own location.
 *
 * lat is the latitude in degrees
 * lon is the longitude in degrees
 * accuracy is the accuracy radius in meters
 */
typedef void (*GoogleMapsDeviceLocatorSubscriptionCallback)(float lat, float lon, float accuracy);

class GoogleMapsDeviceLocator {
public:
	GoogleMapsDeviceLocator();
	virtual ~GoogleMapsDeviceLocator();

	/**
	 * If you use withLocateOnce() then the location will be updated once after you've
	 * connected to the cloud.
	 */
	GoogleMapsDeviceLocator &withLocateOnce();

	/**
	 * Use this method to register a callback to be notified when your location has been found.
	 *
	 * The prototype for the function is:
	 *
	 * void locationCallback(float lat, float lon, float accuracy)
	 */
	GoogleMapsDeviceLocator &withSubscribe(GoogleMapsDeviceLocatorSubscriptionCallback callback);

	/**
	 * You should call this from loop() to give the code time to process things in the background.
	 * This is really only needed if you use withLocateOnce() or withLocatePeriodic() but it doesn't
	 * hurt to call it always from loop. It executes quickly.
	 */
	void loop();

	void publishLocation();

	/**
	 * Queries the location information (Wi-Fi or cellular) and returns a JSON block of data to
	 * put in the event data. This returns a static buffer pointer and is not reentrant.
	 */
	const char *scan();

protected:
	void subscriptionHandler(const char *event, const char *data);

	const char *wifiScan();

	static const int CONNECT_WAIT_STATE = 0;
	static const int CONNECTED_WAIT_STATE = 2;
	static const int CONNECTED_STATE = 3;
	static const int IDLE_STATE = 4;

	static const int LOCATOR_MODE_MANUAL = 0;
	static const int LOCATOR_MODE_ONCE = 1;
	static const int LOCATOR_MODE_PERIODIC = 2;

	int locatorMode;
	unsigned long periodMs;
	String eventName;
	unsigned long stateTime;
	int state;
	GoogleMapsDeviceLocatorSubscriptionCallback callback;
	unsigned long waitAfterConnect;
};

#endif
