#include <SmingCore.h>
#include <memory>
#include <Network/UPnP/DeviceHost.h>
#include <Panasonic/VieraTV/Client.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
#define WIFI_PWD "PleaseEnterPass"
#endif

using namespace Panasonic;

VieraTV::Client client;

void onConnected(HttpConnection& connection, XML::Document& description)
{
	auto node = XML::getNode(description, F("device/friendlyName"));
	Serial.println(_F("New Viera TV found."));
	if(node != nullptr) {
		Serial.print(_F("Friendly name: "));
		Serial.println(node->value());
	}

	client.setMute(true);			// mute the TV
	client.getMute([](bool muted) { // check the mute state
		Serial.print("Muted state: ");
		Serial.println(muted);
	});

	client.sendCommand(VieraTV::CommandAction::ACTION_CH_UP);
	client.sendAppCommand(VieraTV::ApplicationId::APP_YOUTUBE);
}

// Will be called when WiFi station was connected to AP
void connectOk(IpAddress ip, IpAddress mask, IpAddress gateway)
{
	Serial.print(_F("I'm CONNECTED to "));
	Serial.println(ip);

	if(!UPnP::deviceHost.begin()) {
		debug_e("UPnP initialisation failed");
		return;
	}

	client.connect(onConnected);
}

// Will be called when WiFi station was disconnected
void connectFail(const String& ssid, MacAddress bssid, WifiDisconnectReason reason)
{
	// The different reason codes can be found in user_interface.h. in your SDK.
	Serial.print(_F("Disconnected from \""));
	Serial.print(ssid);
	Serial.print(_F("\", reason: "));
	Serial.println(WifiEvents.getDisconnectReasonDesc(reason));
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true); // Allow debug print to serial

	// Station - WiFi client
	WifiStation.enable(true);
	WifiStation.config(_F(WIFI_SSID), _F(WIFI_PWD));

	// Set callback that should be triggered when we have assigned IP
	WifiEvents.onStationGotIP(connectOk);

	// Set callback that should be triggered if we are disconnected or connection attempt failed
	WifiEvents.onStationDisconnect(connectFail);
}
