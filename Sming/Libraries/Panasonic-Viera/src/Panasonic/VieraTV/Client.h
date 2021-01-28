#pragma once

#include <SmingCore.h>
#include <Data/CStringArray.h>
#include <Network/UPnP/ControlPoint.h>
#include <Network/UPnP/schemas-upnp-org/ClassGroup.h>
#include <Network/UPnP/panasonic-com/ClassGroup.h>

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

#define VIERA_APP_MAP(XX)                                                                                              \
	/** id, name, code */                                                                                              \
	XX(NETFLIX, "Netflix", 0010000200000001)                                                                           \
	XX(YOUTUBE, "YouTube", 0070000200170001)                                                                           \
	XX(AMAZON, "Amazon Prime Video", 0010000100170001)                                                                 \
	XX(PLEX, "Plex", 0076010507000001)                                                                                 \
	XX(BBC_IPLAYER, "BBC iPlayer", 0020000A00170010)                                                                   \
	XX(BBC_NEWS, "BBC News", 0020000A00170006)                                                                         \
	XX(BBC_SPORT, "BBC Sport", 0020000A00170007)                                                                       \
	XX(ITV_HUB, "ITV Hub", 0387878700000124)                                                                           \
	XX(TUNE_INE, "TuneIn", 0010001800000001)                                                                           \
	XX(ACCU_WATHER, "AccuWeather", 0070000C00000001)                                                                   \
	XX(ALL_4, "All 4", 0387878700000125)                                                                               \
	XX(DEMAND_5, "Demand 5", 0020009300000002)                                                                         \
	XX(RAKUTEN, "Rakuten TV", 0020002A00000001)                                                                        \
	XX(CHILI, "CHILI", 0020004700000001)                                                                               \
	XX(STV_PLAYER, "STV Player", 0387878700000132)                                                                     \
	XX(DIGITAL_CONCERT_HALL, "Digital Concert Hall", 0076002307170001)                                                 \
	XX(APPS_MARKET, "Apps Market", 0387878700000102)                                                                   \
	XX(BROWSER, "Browser", 0077777700160002)                                                                           \
	XX(CALENDAR, "Calendar", 0387878700150020)                                                                         \
	XX(VIERA_LINK, "VIERA Link", 0387878700000016)                                                                     \
	XX(RECORDED_TV, "Recorded TV", 0387878700000013)                                                                   \
	XX(FREEVIEW_CATCH_UP, "Freeview Catch Up", 0387878700000109)

namespace Panasonic
{
namespace VieraTV
{
enum class CommandAction {
#define XX(name, description) ACTION_##name,
	VIERA_COMMAND_MAP(XX)
#undef XX
};

enum class ApplicationId {
#define XX(id, name, code) APP_##id,
	VIERA_APP_MAP(XX)
#undef XX
};

String toString(enum CommandAction a);
String toString(enum ApplicationId a);

using namespace UPnP::schemas_upnp_org::device;
using namespace UPnP::schemas_upnp_org::service;
using namespace UPnP::panasonic_com::device;
using namespace UPnP::panasonic_com::service;

class Client : public UPnP::ControlPoint
{
public:
	using ConnectedCallback = Delegate<void(Client&)>;
	using GetMuteCallback = Delegate<void(bool muted)>;
	using GetVolumeCallback = Delegate<void(int volume)>;

	Client(size_t maxDescriptionSize = 4096) : ControlPoint(maxDescriptionSize)
	{
	}

	/**
	 * @brief Searches for Viera TVs and connects to the first that is found.
	 * @param callback will be called once a TV is auto-discovered
	 * @retval true when the connect request can be started
	 */
	bool connect(ConnectedCallback callback);

	/**
	 * Send a command to the TV
	 *
	 * @param action command Command from codes.txt
	 */
	bool sendCommand(CommandAction action, p00NetworkControl1::SendKey::Callback callback = nullptr);

	/**
	 * Send a command to the TV
	 *
	 * @param action in free form
	 *
	 */
	bool sendCommand(const String& action, p00NetworkControl1::SendKey::Callback callback = nullptr);

	/**
	 * Send a change HDMI input to the TV
	 *
	 * @param input
	 */
	bool switchToHdmi(size_t input);

	/**
	 * Send command to open app on the TV
	 *
	 * @param id
	 */
	bool launchApp(enum ApplicationId id, p00NetworkControl1::LaunchApp::Callback callback = nullptr)
	{
		return launchApp(toString(id), callback);
	}

	/**
	 * Send command to open app on the TV
	 *
	 * @param {String} applicationId appId from codes.txt
	 */
	bool launchApp(const String& applicationId, p00NetworkControl1::LaunchApp::Callback callback = nullptr);

	/**
	 * Get volume from TV
	 *
	 * @param callback
	 * @return bool - true on success false otherwise
	 */
	bool getVolume(GetVolumeCallback onVolume, uint32_t instanceId = 0,
				   const String& channel = RenderingControl1::Channel::fs_Master);

	/**
	 * Set volume
	 *
	 * @param {Int} volume Desired volume in range from 0 to 100
	 */
	bool setVolume(size_t volume, uint32_t instanceId = 0,
				   const String& channel = RenderingControl1::Channel::fs_Master);

	/**
	 * Get the current mute setting
	 *
	 * @param callback
	 * @return bool - true on success false otherwise
	 */
	bool getMute(GetMuteCallback onMute, uint32_t instanceId = 0,
				 const String& channel = RenderingControl1::Channel::fs_Master);

	/**
	 * Set mute to on/off
	 *
	 * @param {Boolean} enable The value to set mute to
	 */
	bool setMute(bool enable, uint32_t instanceId = 0, const String& channel = RenderingControl1::Channel::fs_Master);

private:
	ConnectedCallback onConnected;

	// TODO: Move this as protected method in the ControlPoint class
	bool checkResponse(UPnP::ActionResponse& response);

	UPnP::schemas_upnp_org::service::RenderingControl1* renderService = nullptr;
	UPnP::panasonic_com::service::p00NetworkControl1* remoteControlService = nullptr;
};

} // namespace VieraTV

} // namespace Panasonic
