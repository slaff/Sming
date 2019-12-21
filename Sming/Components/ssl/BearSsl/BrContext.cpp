/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * BrContext.cpp
 *
 * @author: 2019 - mikee47 <mike@sillyhouse.net>
 *
 ****/

#include "BrContext.h"
#include "BrConnection.h"

namespace Ssl
{
Connection* BrContext::createClient()
{
	auto connection = new BrClientConnection(*this);
	if(connection != nullptr) {
		connection->init();
	}
	return connection;
}

Connection* BrContext::createServer()
{
	return nullptr;

	/*
	context = ssl_ctx_new(SSL_CONNECT_IN_PARTS | options, sessionCacheSize);
	if(context == nullptr) {
		debug_e("SSL: Unable to allocate context");
		return false;
	}

	auto connection = new ConnectionImpl(tcp);
	auto server = ssl_server_new(context, int(connection));
	if(server == nullptr) {
		delete connection;
		return nullptr;
	}

	connection->init(server);
	return connection;
*/
}

} // namespace Ssl
