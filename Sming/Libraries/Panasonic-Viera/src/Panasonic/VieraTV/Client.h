#pragma once

#include <SmingCore.h>
#include <Network/UPnP/DeviceHost.h>
#include <Network/UPnP/Soap.h>
#include <Data/CStringArray.h>

using namespace rapidxml;

#define VIERA_COMMAND_MAP(XX)                                                                                          \
	/* action, description */                                                                                          \
	XX(CH_DOWN, "channel down")                                                                                        \
	XX(CH_UP, "channel up")                                                                                            \
	XX(VOLUP, "volume up")                                                                                             \
	XX(VOLDOWN, "volume down")                                                                                         \
	XX(POWER, "power off")                                                                                             \
	XX(MUTE, "mute")                                                                                                   \
	XX(TV, "TV")                                                                                                       \
	XX(CHG_INPUT, "AV")                                                                                                \
	XX(HOME, "home screen")                                                                                            \
	XX(APPS, "apps")                                                                                                   \
	XX(GUIDE, "guide")                                                                                                 \
	XX(RED, "red button")                                                                                              \
	XX(GREEN, "green")                                                                                                 \
	XX(YELLOW, "yellow")                                                                                               \
	XX(BLUE, "blue")                                                                                                   \
	XX(VTOOLS, "VIERA tools")                                                                                          \
	XX(CANCEL, "Cancel / Exit")                                                                                        \
	XX(SUBMENU, "Option")                                                                                              \
	XX(RETURN, "Return")                                                                                               \
	XX(ENTER, "Control Center click / enter")                                                                          \
	XX(RIGHT, "Control RIGHT")                                                                                         \
	XX(LEFT, "Control LEFT")                                                                                           \
	XX(UP, "Control UP")                                                                                               \
	XX(DOWN, "Control DOWN")                                                                                           \
	XX(3D, "3D button")                                                                                                \
	XX(SD_CARD, "SD-card")                                                                                             \
	XX(DISP_MODE, "Display mode / Aspect ratio")                                                                       \
	XX(MENU, "Menu")                                                                                                   \
	XX(INTERNET, "VIERA connect")                                                                                      \
	XX(VIERA_LINK, "VIERA link")                                                                                       \
	XX(EPG, "Guide / EPG")                                                                                             \
	XX(TEXT, "Text / TTV")                                                                                             \
	XX(STTL, "STTL / Subtitles")                                                                                       \
	XX(INFO, "info")                                                                                                   \
	XX(INDEX, "TTV index")                                                                                             \
	XX(HOLD, "TTV hold / image freeze")                                                                                \
	XX(R_TUNE, "Last view")                                                                                            \
	XX(REW, "rewind")                                                                                                  \
	XX(PLAY, "play")                                                                                                   \
	XX(FF, "fast forward")                                                                                             \
	XX(SKIP_PREV, "skip previous")                                                                                     \
	XX(PAUSE, "pause")                                                                                                 \
	XX(SKIP_NEXT, "skip next")                                                                                         \
	XX(STOP, "stop")                                                                                                   \
	XX(REC, "record")

namespace Panasonic
{
namespace VieraTV
{
enum class CommandAction {
#define XX(name, description) ACTION_##name,
	VIERA_COMMAND_MAP(XX)
#undef XX
};

String toString(enum CommandAction a);

class Client: public UPnP::RootDevice
{
public:
	using ConnectedCallback = Delegate<void(Client&)>;

	struct Command {
		enum class Type {
			REMOTE,
			RENDER,
		};

		Type type;
		String name; // How device identifies itself
		XML::Document* params;
	};

	/**
	 * @brief Searches for Viera TVs and connects to the first that is found.
	 */
	bool connect(ConnectedCallback callback);

	/**
	 * @brief Connect to a desired ip and port where Viera TV should be up and running.
	 * @param ip
	 * @param port
	 */
	void connect(IpAddress ip, uint16_t port);

	/**
	 * Send a command to the TV
	 *
	 * @param action command Command from codes.txt
	 */
	bool sendCommand(CommandAction action);

	/**
	 * Send a change HDMI input to the TV
	 *
	 * @param input
	 */
	bool switchToHdmi(size_t input);

	/**
	 * Send command to open app on the TV
	 *
	 * @param {String} applicationId appId from codes.txt
	 */
	bool sendAppCommand(const String& applicationId);

	/**
	 * Get volume from TV
	 *
	 * @param {Function} callback
	 */
	bool getVolume(/* callback */);

	/**
	 * Set volume
	 *
	 * @param {Int} volume Desired volume in range from 0 to 100
	 */
	bool setVolume(size_t volume);

	/**
	 * Get the current mute setting
	 *
	 * @param {Function} callback
	 */
	bool getMute(/* callback */);

	/**
	 * Set mute to on/off
	 *
	 * @param {Boolean} enable The value to set mute to
	 */
	bool setMute(bool enable);

	/* UPnP stuff helping us find the location of the TV */
	bool formatMessage(SSDP::Message& msg, SSDP::MessageSpec& ms) override
	{
		msg["ST"] = F("urn:panasonic-com:device:p00RemoteController:1");
		return true;
	}

	void onNotify(SSDP::BasicMessage& msg) override;

private:
	SOAP::Envelope envelope;
	XML::Node* actionTag = nullptr;
	XML::Document paramsDoc;

	ConnectedCallback onConnected;

	HttpClient http;
	Url tvUrl;

	bool sendRequest(Command command);

	bool setParams(Command& cmd, const String& text)
	{
		cmd.params = &paramsDoc;
		cmd.params->parse<0>((char*)text.c_str());
		return true;
	}
};

} // namespace Viera

} // namespace Panasonic
