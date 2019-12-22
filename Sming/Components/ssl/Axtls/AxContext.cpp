/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * AxContext.cpp
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#include "AxContext.h"
#include "AxConnection.h"

namespace Ssl
{
AxContext::~AxContext()
{
	// Free context typically sends out closing message
	ssl_ctx_free(context);
}

bool AxContext::init(tcp_pcb* tcp, uint32_t options, size_t sessionCacheSize)
{
	assert(context == nullptr);

	context = ssl_ctx_new(SSL_CONNECT_IN_PARTS | options, sessionCacheSize);
	if(context == nullptr) {
		debug_e("SSL: Unable to allocate context");
		return false;
	}

	this->tcp = tcp;
	return true;
}

Connection* AxContext::createClient(SessionId* sessionId, const Extension& extension)
{
	assert(context != nullptr);

	auto ssl_ext = ssl_ext_new();
	ssl_ext_set_host_name(ssl_ext, extension.hostName.c_str());
	ssl_ext_set_max_fragment_size(ssl_ext, extension.fragmentSize);

	auto connection = new AxConnection(tcp);
	auto client = ssl_client_new(context, int(connection), sessionId ? sessionId->getValue() : nullptr,
								 sessionId ? sessionId->getLength() : 0, ssl_ext);
	if(client == nullptr) {
		ssl_ext_free(ssl_ext);
		delete connection;
		return nullptr;
	}

	connection->init(client);
	return connection;
}

Connection* AxContext::createServer()
{
	assert(context != nullptr);

	auto connection = new AxConnection(tcp);
	auto server = ssl_server_new(context, int(connection));
	if(server == nullptr) {
		delete connection;
		return nullptr;
	}

	connection->init(server);
	return connection;
}

bool AxContext::loadMemory(ObjectType memType, const uint8_t* data, size_t length, const char* password)
{
	return (ssl_obj_memory_load(context, int(memType), data, length, password) == SSL_OK);
}

// Required by axtls-8266
extern "C" int ax_get_file(const char* filename, uint8_t** buf)
{
	*buf = 0;
	return 0;
}

} // namespace Ssl
