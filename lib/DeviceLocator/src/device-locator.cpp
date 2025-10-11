#include "Particle.h"
#include "device-locator.h"

static char requestBuf[256];
static char *requestCur;
static int numAdded = 0;

GoogleMapsDeviceLocator::GoogleMapsDeviceLocator() : locatorMode(LOCATOR_MODE_MANUAL), periodMs(10000), eventName("deviceLocator"), stateTime(0), state(CONNECT_WAIT_STATE), callback(NULL), waitAfterConnect(8000)
{

}

GoogleMapsDeviceLocator::~GoogleMapsDeviceLocator()
{

}

GoogleMapsDeviceLocator &GoogleMapsDeviceLocator::withLocateOnce()
{
	locatorMode = LOCATOR_MODE_ONCE;
	return *this;
}

GoogleMapsDeviceLocator &GoogleMapsDeviceLocator::withSubscribe(GoogleMapsDeviceLocatorSubscriptionCallback callback)
{
	this->callback = callback;

	snprintf(requestBuf, sizeof(requestBuf), "hook-response/%s/%s", eventName.c_str(), System.deviceID().c_str());

	Particle.subscribe(requestBuf, &GoogleMapsDeviceLocator::subscriptionHandler, this);	//MY_DEVICES deprecated

	return *this;
}

void GoogleMapsDeviceLocator::loop()
{
	switch(state)
	{
	case CONNECT_WAIT_STATE:
	    if (WiFi.ready())
	    {
    		if (Particle.connected())
    		{
    			state = CONNECTED_WAIT_STATE;
    			stateTime = millis();
    		}
	    }
		break;
	case CONNECTED_WAIT_STATE:
		if (millis() - stateTime >= waitAfterConnect)
		{
			// Wait several seconds after connecting before doing the location
			if (locatorMode == LOCATOR_MODE_ONCE)
			{
    		    if (Particle.connected())
    		    {
    				publishLocation();
    				state = IDLE_STATE;
    		    }
			}
			else if (locatorMode == LOCATOR_MODE_MANUAL)
			{
				state = IDLE_STATE;
			}
			else
			{
				state = CONNECTED_STATE;
				stateTime = millis() - periodMs;
			}
		}
		break;
	case CONNECTED_STATE:
	    if (WiFi.ready())
	    {
		    if (Particle.connected())
		    {
			    if (millis() - stateTime >= periodMs)
			    {
			    	stateTime = millis();
				    publishLocation();
			    }
		    }
		    else
		    {
			    state = CONNECT_WAIT_STATE;    // We have disconnected, rec
		    }
	    }
	    else
	    {
			    state = CONNECT_WAIT_STATE;
	    }
		break;
	case IDLE_STATE:
		// Just hang out here forever (entered only on LOCATOR_MODE_ONCE)
		break;
	}
}

const char *GoogleMapsDeviceLocator::scan()
{
	return wifiScan();
}

void GoogleMapsDeviceLocator::publishLocation()
{

	//Serial.println("publishLocation");

	const char *scanData = scan();

	//Serial.printlnf("scanData=%s", scanData);

	if (scanData[0])
	{
	    if (WiFi.ready())
	    {
    		if (Particle.connected())
    		{
    			Particle.publish(eventName, scanData, PRIVATE);                         //V397
    		}
	    }
	}
}

void GoogleMapsDeviceLocator::subscriptionHandler(const char *event, const char *data)
{
	if (callback)
	{
		char *mutableCopy = strdup(data);
		char *part, *end;
		float lat, lon, accuracy;

		part = strtok_r(mutableCopy, ",", &end);
		if (part) {
			lat = atof(part);
			part = strtok_r(NULL, ",", &end);
			if (part) {
				lon = atof(part);
				part = strtok_r(NULL, ",", &end);
				if (part) {
					accuracy = atof(part);

					(*callback)(lat, lon, accuracy);
				}
			}
		}
		free(mutableCopy);
	}
}

static void wifiScanCallback(WiFiAccessPoint* wap, void* data)
{
	// The - 3 factor here to leave room for the closing JSON array ] object }} and the trailing null
	size_t spaceLeft = &requestBuf[sizeof(requestBuf) - 3] - requestCur;
	if (spaceLeft < 32) {
		return;
	}

	int sizeNeeded = snprintf(requestCur, spaceLeft,
			"%s{\"m\":\"%02x:%02x:%02x:%02x:%02x:%02x\",\"s\":%d,\"c\":%d}",
			(requestCur[-1] == '[' ? "" : ","),
			wap->bssid[0], wap->bssid[1], wap->bssid[2], wap->bssid[3], wap->bssid[4], wap->bssid[5],
			wap->rssi, wap->channel);
	if (sizeNeeded > 0 && sizeNeeded < (int)spaceLeft) {
		// There is enough space to store the whole entry, so save it
		requestCur += sizeNeeded;
		numAdded++;
	}
}

const char *GoogleMapsDeviceLocator::wifiScan()
{

	requestCur = requestBuf;
	numAdded = 0;

	requestCur += sprintf(requestCur, "{\"w\":{\"a\":");
	*requestCur++ = '[';

	WiFi.scan(wifiScanCallback);

	*requestCur++ = ']';
	*requestCur++ = '}';
	*requestCur++ = '}';
	*requestCur++ = 0;

	if (numAdded == 0) {
		requestBuf[0] = 0;
	}

	return requestBuf;
}