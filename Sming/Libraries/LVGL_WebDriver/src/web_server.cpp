#include "include/lv_drivers/web_server.h"
#include <Network/HttpServer.h>
#include <Network/Http/Websocket/WebsocketResource.h>
#include <Data/Stream/FlashMemoryStream.h>
#include <SimpleTimer.h>

namespace lvgl
{
namespace driver
{
static HttpServer server;

IMPORT_FSTR(index_html, COMPONENT_PATH "/resources/index.html")
IMPORT_FSTR(favicon_ico, COMPONENT_PATH "/resources/favicon.ico")
IMPORT_FSTR(heatshrink_js, COMPONENT_PATH "/resources/heatshrink.js")

void onIndex(HttpRequest& request, HttpResponse& response)
{
	auto source = new FlashMemoryStream(index_html);
	response.setCache(86400, true);
	response.sendNamedStream(source);
}

void onFavicon(HttpRequest& request, HttpResponse& response)
{
	auto source = new FlashMemoryStream(favicon_ico);
	response.setCache(86400, true);
	response.sendNamedStream(source);
}

void onHeatshrink(HttpRequest& request, HttpResponse& response)
{
	auto source = new FlashMemoryStream(heatshrink_js);
	response.setCache(86400, true);
	response.headers[HTTP_HEADER_CONTENT_TYPE] = "application/javascript";
	response.sendNamedStream(source);
}

void startWebServer(uint32_t port, WebsocketDelegate onConnect, WebsocketBinaryDelegate onInput,
					WebsocketDelegate onDisconnect)
{
	server.listen(port);
	server.paths.set("/", onIndex);
	server.paths.set("/index.html", onIndex);
	server.paths.set("/favicon.ico", onFavicon);
	server.paths.set("/heatshrink.js", onHeatshrink);
	server.paths.setDefault(onIndex);

	// Web Sockets configuration
	auto wsResource = new WebsocketResource();
	wsResource->setConnectionHandler(onConnect);
	wsResource->setMessageHandler([](WebsocketConnection& socket, const String& message) {
		debug_d("WebSocket message received:\r\n%s\r\n", message.c_str());
	});
	wsResource->setBinaryHandler(onInput);
	wsResource->setDisconnectionHandler(onDisconnect);

	server.paths.set("/ws", wsResource);

	debug_i("=== Web Server started on port: %d ===", port);
}

void stopWebServer()
{
	debug_i("Server is shutting down ...");

	// Stop the web server: Don't shutdown immediately, wait a bit to allow messages to propagate
	auto timer = new SimpleTimer;
	timer->initializeMs<1000>(
		[](void* timer) {
			delete static_cast<SimpleTimer*>(timer);
			server.shutdown();
		},
		timer);
	timer->startOnce();
}

} // namespace driver

} // namespace lvgl
