#include "Client.h"

namespace Panasonic
{
namespace VieraTV
{
#define XX(action, description) #action "\0"
DEFINE_FSTR_LOCAL(fstr_commands, VIERA_COMMAND_MAP(XX))
#undef XX

String toString(enum CommandAction a)
{
	return CStringArray(fstr_commands)[(int)a];
}

void Client::onNotify(SSDP::BasicMessage& msg)
{
	tvUrl = Url(msg[HTTP_HEADER_LOCATION]);

	// TODO: get the description and extract the XML elements. We are interested in the friendlyName

	debug_d("Found Viera TV");
	if(tvUrl.Port != 0) {
		onConnected(*this);
	}
}

bool Client::connect(ConnectedCallback callback)
{
	if(!UPnP::deviceHost.begin()) {
		debug_e("UPnP initialisation failed");
		return false;
	}

	UPnP::deviceHost.registerDevice(this);
	onConnected = callback;

	return true;
}

void Client::connect(IpAddress ip, uint16_t port)
{
	tvUrl.Host = ip.toString();
	tvUrl.Port = port;
}

bool Client::sendCommand(CommandAction action)
{
	Command cmd;
	cmd.type = Command::Type::REMOTE;
	cmd.name = "X_SendKey";

	String text = "<X_KeyEvent>NRC_";
	text += toString(action);
	text += "-ONOFF</X_KeyEvent>";

	setParams(cmd, text);

	return sendRequest(cmd);
}


bool Client::switchToHdmi(size_t input)
{
	Command cmd;
	cmd.type = Command::Type::REMOTE;
	cmd.name = "X_SendKey";
	String text = "<X_KeyEvent>NRC_HDMI";
	text += (input - 1);
	text += "-ONOFF</X_KeyEvent>";

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::sendAppCommand(const String& applicationId)
{
	Command cmd;
	cmd.type = Command::Type::REMOTE;
	cmd.name = "X_LaunchApp";

	setParams(cmd,
			  "<X_AppType>vc_app<X_AppType><X_LaunchKeyword>product_id=" + applicationId + "</X_LaunchKeyword>");

	return sendRequest(cmd);
}

bool Client::getVolume(/* callback */)
{
	//		var self = this;
	//		self.volumeCallback = callback;
	//
	//		this.sendRequest('render', 'GetVolume', '<InstanceID>0</InstanceID><Channel>Master</Channel>',
	//		{
	//			callback: function(data) {
	//				var match = /<CurrentVolume>(\d*)<\/CurrentVolume>/gm.exec(data);
	//				if (match !== null) {
	//					var volume = match[1];
	//					self.volumeCallback(volume);
	//				}
	//			}
	//		});

	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "GetVolume";

	setParams(cmd, "<InstanceID>0</InstanceID><Channel>Master</Channel>");

	return sendRequest(cmd);
}

bool Client::setVolume(size_t volume)
{
	if(volume > 100) {
		debug_e("Volume must be in range from 0 to 100");
		return false;
	}

	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "SetVolume";
	String text = "<InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>";
	text += volume;
	text += "</DesiredVolume>";

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::getMute(/* callback */)
{
	//		var self = this;
	//		self.muteCallback = callback;

	//		this.sendRequest('render', 'GetMute', '<InstanceID>0</InstanceID><Channel>Master</Channel>',
	//		{
	//			callback: function(data) {
	//				var regex = /<CurrentMute>([0-1])<\/CurrentMute>/gm;
	//				var match = regex.exec(data);
	//				if (match !== null) {
	//					var mute = (match[1] === '1');
	//					self.muteCallback(mute);
	//				}
	//			}
	//		});

	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "GetMute";

	setParams(cmd, "<InstanceID>0</InstanceID><Channel>Master</Channel>");

	return sendRequest(cmd);
}

bool Client::setMute(bool enable)
{
	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "SetMute";

	String text = "<InstanceID>0</InstanceID><Channel>Master</Channel><DesiredMute>";
	text += (enable ? '1' : '0');
	text += "</DesiredMute>";

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::sendRequest(Command command)
{
	String path = F("/nrc/control_0");
	String urn = F("panasonic-com:service:p00NetworkControl:1");
	if(command.type == Command::Type::RENDER) {
		path = F("/dmr/control_0");
		urn = F("schemas-upnp-org:service:RenderingControl:1");
	}

	actionTag = nullptr;

	if(!envelope.initialise()) {
		return false;
	}

	auto body = envelope.body();
	if(body == nullptr) {
		return false;
	}

	String tag = "u:" + command.name;
	actionTag = XML::appendNode(body, tag);
	XML::appendAttribute(actionTag, "xmlns:u", urn);

	if(command.params != nullptr) {
		auto doc = body->document();
		auto commandNode = doc->first_node("s:Envelope")->first_node("s:Body")->first_node(tag.c_str());
		for(XML::Node* child = command.params->first_node(); child; child = child->next_sibling()) {
			auto node = doc->clone_node(child);
			commandNode->append_node(node);
		}
	}

	String content = XML::serialize(envelope.doc, true);

	HttpHeaders headers;
	headers[HTTP_HEADER_CONTENT_LENGTH] = content.length();
	headers[HTTP_HEADER_CONTENT_TYPE] = "text/xml; charset=\"utf-8\"";
	headers["SOAPACTION"] = "\"urn:" +urn +'#' + command.name +'"' ;

	HttpRequest* request = new HttpRequest;
	request->method = HTTP_POST;
	request->headers.setMultiple(headers);
	request->uri.Path = path;
	request->uri.Port = tvUrl.Port;
	request->uri.Host = tvUrl.Host;

	return http.send(request);
}


} // namespace Viera

} // namespace Panasonic
