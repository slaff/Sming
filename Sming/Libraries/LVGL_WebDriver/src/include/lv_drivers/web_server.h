#pragma once

#include <Network/Http/Websocket/WebsocketConnection.h>


namespace lvgl
{
namespace driver
{

/**
 * @brief Starts a web server from where the display content can be seen
 * @param port server port
 * @param onConnect callback to be called on new connected user
 * @param onInput callback to be called on input (from mouse mainly)
 * @param onDisconnect callback to be called on disconnect
 */
void startWebServer(uint32_t port, WebsocketDelegate onConnect, WebsocketBinaryDelegate onInput,
					WebsocketDelegate onDisconnect);

/**
 * @brief Stops the web server
 */
void stopWebServer();

} // namespace driver

} // namespace lvgl
