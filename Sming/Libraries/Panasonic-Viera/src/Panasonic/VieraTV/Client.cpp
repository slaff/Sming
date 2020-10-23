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

bool Client::formatMessage(SSDP::Message& message, SSDP::MessageSpec& ms)
{
	// Override the search target
	message["ST"] = panasonicRemoteUrn;
	return true;
}

void Client::onNotify(SSDP::BasicMessage& message)
{
	if(panasonicRemoteUrn != message["NT"] && panasonicRemoteUrn != message["ST"]) {
		return;
	}

	auto location = message[HTTP_HEADER_LOCATION];
	if(location == nullptr) {
		debug_d("No valid Location header found.");
		return;
	}

	if(locations.contains(location)) {
		return; // Already found
	}
	locations += location;

	debug_d("TV message received:");
	for(unsigned i = 0; i < message.count(); ++i) {
		debug_d("%s", message[i]);
	}

	debug_d("Fetching '%s'", location);
	Url url(location);
	auto request = new HttpRequest(url);
	request->setResponseStream(new LimitedMemoryStream(maxDescriptionSize));
	request->onRequestComplete(RequestCompletedDelegate(&Client::onDescription, this));
	http.send(request);
}

int Client::onDescription(HttpConnection& conn, bool success)
{
	if(!success) {
		debug_e("Fetch failed");
		return 0;
	}

	debug_i("Received description");
	auto response = conn.getResponse();
	if(response->stream == nullptr) {
		debug_e("No body");
		return 0;
	}

	if(response->headers.contains(_F("Application-URL"))) {
		// TODO: the application URL can be used using the DIAL protocol to start applications and send stuff to them
		//     see: https://gist.github.com/jcarbaugh/e08dcfe61ece0e7eea12
		debug_d("Application URL: %s", response->headers[_F("Application-URL")].c_str());
	}

	auto stream = reinterpret_cast<LimitedMemoryStream*>(response->stream);
	stream->print('\0');
	XML::Document doc;
#ifndef ARCH_HOST // On my Linux box RapidXML is NOT working correctly with properly formatted XML?!
	XML::deserialize(doc, stream->getStreamPointer());
#endif

	tvUrl = Url(conn.getRequest()->uri);

	debug_d("Found Viera TV");
	if(onConnected) {
		onConnected(*this, doc, response->headers);
	}

	return 0;
}

bool Client::connect(ConnectedCallback callback)
{
	if(!UPnP::deviceHost.begin()) {
		debug_e("UPnP initialisation failed");
		return false;
	}

	UPnP::deviceHost.registerControlPoint(this);

	auto message = new SSDP::MessageSpec(SSDP::MESSAGE_MSEARCH);
	message->object = this;
	message->repeat = 2;
	message->target = SSDP::TARGET_ROOT;
	SSDP::server.messageQueue.add(message, 0);

	onConnected = callback;

	return true;
}

bool Client::connect(const Url& descriptionUrl, ConnectedCallback callback)
{
	tvUrl = descriptionUrl;
	onConnected = callback;

	debug_d("Fetching '%s'", descriptionUrl.toString().c_str());
	auto request = new HttpRequest(descriptionUrl);
	request->setResponseStream(new LimitedMemoryStream(maxDescriptionSize));
	request->onRequestComplete(RequestCompletedDelegate(&Client::onDescription, this));
	http.send(request);

	return true;
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

	String text =
		F("<X_AppType>vc_app</X_AppType><X_LaunchKeyword>product_id=") + applicationId + F("</X_LaunchKeyword>");

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::getVolume(GetVolumeCallback onVolume)
{
	RequestCompletedDelegate requestCallback = [this, onVolume](HttpConnection& connection, bool successful) -> int {
		/* @see: docs/RequestResponse.txt for sample communication */
		CStringArray path("s:Body");
		path.add("u:GetVolumeResponse");
		path.add("CurrentVolume");

		auto node = this->getNode(connection, path);
		if(node != nullptr) {
			onVolume((int)node->value());

			return true;
		}

		return false;
	};

	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "GetVolume";

	String text = "<InstanceID>0</InstanceID><Channel>Master</Channel>";

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
	String text = "<InstanceID>0</InstanceID><Channel>Master</Channel><DesiredVolume>";
	text += volume;
	text += "</DesiredVolume>";

	setParams(cmd, text);

	return sendRequest(cmd);
}

bool Client::getMute(GetMuteCallback onMute)
{
	RequestCompletedDelegate requestCallback = [this, onMute](HttpConnection& connection, bool successful) -> int {
		/* @see: docs/RequestResponse.txt for sample communication */
		CStringArray path("s:Body");
		path.add("u:GetMuteResponse");
		path.add("CurrentMute");
		auto node = this->getNode(connection, path);
		if(node != nullptr) {
			onMute((bool)node->value());

			return true;
		}

		return false;
	};

	Command cmd;
	cmd.type = Command::Type::RENDER;
	cmd.name = "GetMute";

	String text = "<InstanceID>0</InstanceID><Channel>Master</Channel>";

	setParams(cmd, text);

	return sendRequest(cmd, requestCallback);
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

bool Client::sendRequest(Command command, RequestCompletedDelegate requestCallack)
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

	const String content = XML::serialize(envelope.doc, false);

	debug_d("Content XML: %s\n", content.c_str());

	HttpHeaders headers;
	headers[HTTP_HEADER_CONTENT_TYPE] = F("text/xml; charset=\"utf-8\"");
	headers["SOAPACTION"] = "\"urn:" + urn + '#' + command.name + '"';

	HttpRequest* request = new HttpRequest;
	request->method = HTTP_POST;
	request->headers.setMultiple(headers);
	request->uri.Path = path;
	request->uri.Port = tvUrl.Port;
	request->uri.Host = tvUrl.Host;
	request->setBody(content);

	if(requestCallack != nullptr) {
		request->onRequestComplete(requestCallack);
	}

	return http.send(request);
}

XML::Node* Client::getNode(HttpConnection& connection, const CStringArray& path)
{
	HttpResponse* response = connection.getResponse();
	if(response->stream == nullptr) {
		debug_e("No body");
		return nullptr;
	}

	auto stream = reinterpret_cast<LimitedMemoryStream*>(response->stream);
	stream->print('\0');
	XML::Document doc;
#ifndef ARCH_HOST // On my Linux box RapidXML is NOT working correctly with properly formatted XML?!
	XML::deserialize(doc, stream->getStreamPointer());
#endif

	return getNode(doc, path);
}

XML::Node* Client::getNode(const XML::Document& doc, const CStringArray& path)
{
	auto node = doc.first_node();
	if(node != nullptr) {
		for(size_t i = 0; i < path.count(); i++) {
			node = node->first_node(path[i]);
			if(node == nullptr) {
				break;
			}
		}
	}

	return node;
}

} // namespace VieraTV

} // namespace Panasonic
