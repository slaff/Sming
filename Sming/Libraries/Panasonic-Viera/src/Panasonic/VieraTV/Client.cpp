#include "Client.h"
#include <Network/SSDP/Server.h>

namespace Panasonic
{
namespace VieraTV
{
#define XX(action, description) #action "\0"
DEFINE_FSTR_LOCAL(vieraCommands, VIERA_COMMAND_MAP(XX))
#undef XX

#define XX(id, name, code) #code "\0"
DEFINE_FSTR_LOCAL(vieraApps, VIERA_APP_MAP(XX))
#undef XX

DEFINE_FSTR(panasonicRemoteUrn, "urn:panasonic-com:device:p00RemoteController:1");

String toString(enum CommandAction a)
{
	return CStringArray(vieraCommands)[(int)a];
}

String toString(enum ApplicationId a)
{
	return CStringArray(vieraApps)[(int)a];
}

bool Client::connect(ConnectedCallback callback, const String& id)
{
	if(!initialized) {
		UPnP::panasonic_com::registerClasses();
		UPnP::schemas_upnp_org::registerClasses();
		initialized = true;
	}

	cancelSearch();
	beginSearch(Delegate<bool(MediaRenderer1&)>([this, id](auto& device) {
		debug_d("Found Media Renderer: %s", device.caption().c_str());

		String uniqueDeviceName = device.getField(UPnP::Device::Field::UDN);
		if(id != nullptr) {
			if(uniqueDeviceName != id) {
				// we are not interested in this device and don't keep it
				return false;
			}

			// Stop search - we were looking for a specific device and we have found it
			cancelSearch();
		}

		devices[uniqueDeviceName].render = device.getRenderingControl();

		return true;
	}));

	// do a search for renderers too
	auto timer = new AutoDeleteTimer;
	timer
		->initializeMs<5000>([this, id, callback]() {
			beginSearch(Delegate<bool(p00RemoteController1&)>([this, id, callback](auto& device) {
				debug_d("Found Panasonic Controller: %s", device.caption().c_str());

				String uniqueDeviceName = device.getField(UPnP::Device::Field::UDN);
				if(id != nullptr) {
					if(uniqueDeviceName != id) {
						// we are not interested in this device and don't keep it
						return false;
					}

					// Stop search - we were looking for a specific device and we have found it
					cancelSearch();
				}

				devices[uniqueDeviceName].control = device.getp00NetworkControl();

				callback(*this);

				return true;
			}));
		})
		.startOnce();

	return true;
}

bool Client::sendCommand(CommandAction action, p00NetworkControl1::SendKey::Callback callback)
{
	return sendCommand(toString(action), callback);
}

bool Client::sendCommand(const String& action, p00NetworkControl1::SendKey::Callback callback)
{
	for(size_t i = 0; i < devices.count(); i++) {
		auto device = devices.valueAt(i);
		if(device.control == nullptr) {
			continue;
		}

		device.control->sendKey(action, callback);
	}

	return true;
}

bool Client::switchToHdmi(size_t input)
{
	String action = F("NRC_HDMI");
	action += (input - 1);

	return sendCommand(action, nullptr);
}

bool Client::launchApp(const String& applicationId, p00NetworkControl1::LaunchApp::Callback callback)
{
	for(size_t i = 0; i < devices.count(); i++) {
		auto device = devices.valueAt(i);
		if(device.control == nullptr) {
			continue;
		}

		device.control->launchApp("vc_app", F("product_id=") + applicationId, callback);
	}

	return true;
}

bool Client::getVolume(GetVolumeCallback onVolume, uint32_t instanceId, const String& channel)
{
	for(size_t i = 0; i < devices.count(); i++) {
		auto device = devices.valueAt(i);
		if(device.render == nullptr) {
			continue;
		}

		device.render->getVolume(instanceId, channel, [this, onVolume](auto response) {
			if(checkResponse(response)) {
				onVolume(response.getCurrentVolume());
			}
		});
	}

	return true;
}

bool Client::setVolume(size_t volume, uint32_t instanceId, const String& channel)
{
	if(volume > 100) {
		debug_e("Volume must be in range from 0 to 100");
		return false;
	}

	for(size_t i = 0; i < devices.count(); i++) {
		auto device = devices.valueAt(i);
		if(device.render == nullptr) {
			continue;
		}

		device.render->setVolume(instanceId, channel, volume, [instanceId, volume, channel](auto response) {
			debug_d("render->setVolume(%d, %s): %d", instanceId, channel.c_str(), volume);
		});
	}

	return true;
}

bool Client::getMute(GetMuteCallback onMute, uint32_t instanceId, const String& channel)
{
	for(size_t i = 0; i < devices.count(); i++) {
		auto device = devices.valueAt(i);
		if(device.render == nullptr) {
			continue;
		}

		device.render->getMute(instanceId, channel, [this, onMute, instanceId, channel](auto response) {
			if(checkResponse(response)) {
				debug_d("render->getVolume(%d, %s): %d", instanceId, channel.c_str(), response.getCurrentMute());
				onMute(response.getCurrentMute());
			}
		});
	}

	return true;
}

bool Client::setMute(bool enable, uint32_t instanceId, const String& channel)
{
	for(size_t i = 0; i < devices.count(); i++) {
		auto device = devices.valueAt(i);
		if(device.render == nullptr) {
			continue;
		}

		device.render->setMute(instanceId, channel, enable, [this, instanceId, channel](auto response) {});
	}

	return true;
}

bool Client::checkResponse(UPnP::ActionResponse& response)
{
	if(auto fault = response.fault()) {
		fault.printTo(Serial);
		return false;
	}

	return true;
}

} // namespace VieraTV

} // namespace Panasonic
