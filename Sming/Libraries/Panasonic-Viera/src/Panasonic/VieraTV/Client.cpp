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

DEFINE_FSTR(domain, "panasonic-com")
DEFINE_FSTR(device, "p00RemoteController")
DEFINE_FSTR(service_NetworkControl, "p00NetworkControl")
DEFINE_FSTR(service_RenderingControl, "RenderingControl")

constexpr uint8_t version{1};

String toString(CommandAction a)
{
	return CStringArray(vieraCommands)[unsigned(a)];
}

String toString(ApplicationId a)
{
	return CStringArray(vieraApps)[unsigned(a)];
}

bool Client::connect(Connected callback)
{
	UPnP::DeviceUrn urn(domain, device, version);
	return beginSearch(urn, [this, callback](HttpConnection& connection, XML::Document& description) {
		tvUrl = connection.getRequest()->uri;

		debug_d("Found Viera TV");

		if(callback) {
			callback(connection, description);
		}
	});
}

bool Client::sendCommand(CommandAction action)
{
	Command cmd;
	cmd.type = Command::Type::REMOTE;
	cmd.name = F("X_SendKey");

	String text = F("<X_KeyEvent>NRC_");
	text += toString(action);
	text += F("-ONOFF</X_KeyEvent>");

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::switchToHdmi(size_t input)
{
	Command cmd;
	cmd.type = Command::Type::REMOTE;
	cmd.name = F("X_SendKey");
	String text = F("<X_KeyEvent>NRC_HDMI");
	text += (input - 1);
	text += F("-ONOFF</X_KeyEvent>");

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::sendAppCommand(const String& applicationId)
{
	Command cmd;
	cmd.type = Command::Type::REMOTE;
	cmd.name = F("X_LaunchApp");

	String text = F("<X_AppType>vc_app</X_AppType><X_LaunchKeyword>product_id=");
	text += applicationId;
	text += F("</X_LaunchKeyword>");

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::getVolume(GetVolume onVolume)
{
	RequestCompletedDelegate requestCallback = [this, onVolume](HttpConnection& connection, bool successful) -> int {
		/* @see: docs/RequestResponse.txt for sample communication */
		auto node = this->getNode(connection, F("s:Body/u:GetVolumeResponse/CurrentVolume"));
		if(node != nullptr) {
			onVolume(atoi(node->value()));

			return true;
		}

		return false;
	};

	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "GetVolume";

	String text = F("<InstanceID>0</InstanceID><Channel>Master</Channel>");

	setParams(cmd, text);

	return sendRequest(cmd, requestCallback);
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
	String text = F("<InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>");
	text += volume;
	text += "</DesiredVolume>";

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::getMute(GetMute onMute)
{
	RequestCompletedDelegate requestCallback = [this, onMute](HttpConnection& connection, bool successful) -> int {
		/* @see: docs/RequestResponse.txt for sample communication */
		auto node = this->getNode(connection, F("s:Body/u:GetMuteResponse/CurrentMute"));
		if(node != nullptr) {
			onMute(atoi(node->value()) != 0);

			return true;
		}

		return false;
	};

	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "GetMute";

	String text = F("<InstanceID>0</InstanceID><Channel>Master</Channel>");

	setParams(cmd, text);

	return sendRequest(cmd, requestCallback);
}

bool Client::setMute(bool enable)
{
	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "SetMute";

	String text = F("<InstanceID>0</InstanceID><Channel>Master</Channel><DesiredMute>");
	text += enable ? '1' : '0';
	text += F("</DesiredMute>");

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::sendRequest(Command command, RequestCompletedDelegate requestCallback)
{
	String path;
	UPnP::ServiceUrn urn(domain, nullptr, version);
	if(command.type == Command::Type::RENDER) {
		path = F("/dmr/control_0");
		urn.type = service_RenderingControl;
	} else {
		path = F("/nrc/control_0");
		urn.type = service_RenderingControl;
	}

	if(!envelope.initialise()) {
		return false;
	}

	auto body = envelope.body();
	if(body == nullptr) {
		return false;
	}

	String tag = "u:" + command.name;
	auto actionTag = XML::appendNode(body, tag);
	XML::appendAttribute(actionTag, "xmlns:u", urn);

	if(command.params != nullptr) {
		auto doc = body->document();
		// TODO: Can use XML::getNode()
		auto commandNode = XML::getNode(doc, F("s:Envelope/s:Body/") + tag);
		assert(commandNode != nullptr);
		for(XML::Node* child = command.params->first_node(); child; child = child->next_sibling()) {
			auto node = doc->clone_node(child);
			commandNode->append_node(node);
		}
	}

	String content = XML::serialize(envelope.doc, false);

	debug_d("Content XML: %s\n", content.c_str());

	HttpHeaders headers;
	headers[HTTP_HEADER_CONTENT_TYPE] = F("text/xml; charset=\"utf-8\"");
	headers["SOAPACTION"] = "\"urn:" + urn + '#' + command.name + '"';

	auto request = new HttpRequest;
	request->method = HTTP_POST;
	request->headers.setMultiple(headers);
	request->uri.Path = path;
	request->uri.Port = tvUrl.Port;
	request->uri.Host = tvUrl.Host;
	request->setBody(content);

	if(requestCallback) {
		request->onRequestComplete(requestCallback);
	}

	return http.send(request);
}

XML::Node* Client::getNode(HttpConnection& connection, const String& path)
{
	HttpResponse* response = connection.getResponse();
	if(response->stream == nullptr) {
		debug_e("No body");
		return nullptr;
	}

	String content;
	response->stream->moveString(content);
	XML::Document doc;
	XML::deserialize(doc, content);

	return XML::getNode(doc, path);
}

} // namespace VieraTV

} // namespace Panasonic
