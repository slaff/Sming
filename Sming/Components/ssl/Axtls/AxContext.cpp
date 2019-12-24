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
#include <Network/Ssl/Session.h>

namespace Ssl
{
AxContext::~AxContext()
{
	// Free context typically sends out closing message
	ssl_ctx_free(context);
}

bool AxContext::init(uint32_t options, size_t sessionCacheSize)
{
	assert(context == nullptr);

	context = ssl_ctx_new(SSL_CONNECT_IN_PARTS | (options & 0xFFFF0000), sessionCacheSize);
	if(context == nullptr) {
		debug_e("SSL: Unable to allocate context");
		return false;
	}

	return true;
}

bool AxContext::setKeyCert(KeyCertPair& keyCert)
{
	if(!keyCert.isValid()) {
		debug_w("Ignoring invalid keyCert");
		return true;
	}

	auto load = [&](int objtype, const uint8_t* data, unsigned length, const char* password) -> bool {
		lastError = ssl_obj_memory_load(context, objtype, data, length, password);
		return lastError == SSL_OK;
	};

	if(!load(SSL_OBJ_RSA_KEY, keyCert.getKey(), keyCert.getKeyLength(), keyCert.getKeyPassword())) {
		debug_e("SSL: Error loading key");
		return false;
	}

	if(!load(SSL_OBJ_X509_CERT, keyCert.getCertificate(), keyCert.getCertificateLength(), nullptr)) {
		debug_e("SSL: Error loading certificate");
		return false;
	}

	return true;
}

Connection* AxContext::createClient()
{
	assert(context != nullptr);

	auto ssl_ext = ssl_ext_new();
	ssl_ext_set_host_name(ssl_ext, session.extension.hostName.c_str());
	ssl_ext_set_max_fragment_size(ssl_ext, session.extension.fragmentSize);

	auto id = session.getSessionId();
	auto connection = new AxConnection(*this);
	auto client =
		ssl_client_new(context, int(connection), id ? id->getValue() : nullptr, id ? id->getLength() : 0, ssl_ext);
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

	auto connection = new AxConnection(*this);
	auto server = ssl_server_new(context, int(connection));
	if(server == nullptr) {
		delete connection;
		return nullptr;
	}

	connection->init(server);
	return connection;
}

// Required by axtls-8266
extern "C" int ax_get_file(const char* filename, uint8_t** buf)
{
	*buf = 0;
	return 0;
}

} // namespace Ssl
