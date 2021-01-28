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

bool Client::connect(ConnectedCallback callback)
{
	UPnP::schemas_upnp_org::registerClasses();
	UPnP::panasonic_com::registerClasses();

	onConnected = callback;

	cancelSearch();
	beginSearch(Delegate<bool(MediaRenderer1&)>([this, callback](auto& device) {
		// Stop at the first response
		cancelSearch();

		debug_d("Found Media Renderer: %s", device.caption().c_str());
		renderService = device.getRenderingControl();

		beginSearch(Delegate<bool(p00RemoteController1&)>([this, callback](auto& device) {
			// Stop at the first response
			cancelSearch();

			debug_d("Found Panasonic Remote Control: %s", device.caption().c_str());
			remoteControlService = device.getp00NetworkControl();

			callback(*this);

			return true;
		}));

		return true;
	}));

	return true;
}

bool Client::sendCommand(CommandAction action, p00NetworkControl1::SendKey::Callback callback)
{
	return sendCommand(toString(action), callback);
}

bool Client::sendCommand(const String& action, p00NetworkControl1::SendKey::Callback callback)
{
	if(remoteControlService == nullptr) {
		return false;
	}

	return remoteControlService->sendKey(action, callback);
}

bool Client::switchToHdmi(size_t input)
{
	String action = F("NRC_HDMI");
	action += (input - 1);

	return sendCommand(action, nullptr);
}

//bool

bool Client::launchApp(const String& applicationId, p00NetworkControl1::LaunchApp::Callback callback)
{
	if(remoteControlService == nullptr) {
		return false;
	}

	return remoteControlService->launchApp("vc_app", F("product_id=") + applicationId, callback);
}

bool Client::getVolume(GetVolumeCallback onVolume, uint32_t instanceId, const String& channel)
{
	if(renderService == nullptr) {
		return false;
	}

	return renderService->getVolume(instanceId, channel, [this, onVolume](auto response) {
		if(checkResponse(response)) {
			onVolume(response.getCurrentVolume());
		}
	});

	return false;
}

bool Client::setVolume(size_t volume, uint32_t instanceId, const String& channel)
{
	if(renderService == nullptr) {
		return false;
	}

	if(volume > 100) {
		debug_e("Volume must be in range from 0 to 100");
		return false;
	}

	return renderService->setVolume(instanceId, channel, volume, [instanceId, volume, channel](auto response) {
		debug_d("render->setVolume(%d, %s): %d", instanceId, channel.c_str(), volume);
	});
}

bool Client::getMute(GetMuteCallback onMute, uint32_t instanceId, const String& channel)
{
	if(renderService == nullptr) {
		return false;
	}

	return renderService->getMute(instanceId, channel, [this, onMute, instanceId, channel](auto response) {
		if(checkResponse(response)) {
			debug_d("render->getVolume(%d, %s): %d", instanceId, channel.c_str(), response.getCurrentMute());
			onMute(response.getCurrentMute());
		}
	});
}

bool Client::setMute(bool enable, uint32_t instanceId, const String& channel)
{
	if(renderService == nullptr) {
		return false;
	}

	return renderService->setMute(instanceId, channel, enable, [this, instanceId, channel](auto response) {});
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
