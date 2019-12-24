
#include <Network/Ssl/Session.h>
#include <Network/Ssl/Factory.h>
#include <Platform/WDT.h>
#include <Network/TcpConnection.h>

namespace Ssl
{
bool Session::listen(tcp_pcb* tcp)
{
#ifdef SSL_DEBUG
	options |= SSL_DISPLAY_STATES | SSL_DISPLAY_BYTES | SSL_DISPLAY_CERTS;
#endif

	delete context;
	assert(factory != nullptr);
	context = factory->createContext(*this, tcp);
	if(context == nullptr) {
		return false;
	}

	if(!context->init(options, cacheSize)) {
		return false;
	}

	if(!keyCert.isValid()) {
		debug_e("SSL: server certificate and key are not provided!");
		return false;
	}

	context->setKeyCert(keyCert);

	// TODO: test: free the certificate data on server destroy...
	freeKeyCertAfterHandshake = true;

	return true;
}

bool Session::onAccept(TcpConnection* client)
{
	assert(context != nullptr);
	auto server = context->createServer();
	client->setSsl(server);
	return true;
}

bool Session::onConnect(tcp_pcb* tcp)
{
	debug_d("SSL: Starting connection...");

	// Client Session
	delete context;
	assert(factory != nullptr);
	context = factory->createContext(*this, tcp);
	if(context == nullptr) {
		return false;
	}

	uint32_t localOptions = options;
#ifdef SSL_DEBUG
	localOptions |= SSL_DISPLAY_STATES | SSL_DISPLAY_BYTES | SSL_DISPLAY_CERTS;
	debug_d("SSL: Show debug data ...");
#endif

	if(!context->init(localOptions, 1)) {
		return false;
	}

	if(!context->setKeyCert(keyCert)) {
		debug_e("SSL: Error loading keyCert");
		return false;
	}

	if(sessionId != nullptr && sessionId->isValid()) {
		debug_d("-----BEGIN SSL SESSION PARAMETERS-----");
		debug_d("SessionId: %s", sessionId->toString().c_str());
		debug_d("------END SSL SESSION PARAMETERS------");
	}

	beginHandshake();

	connection = context->createClient();
	if(connection == nullptr) {
		endHandshake();
		return false;
	}

	return true;
}

void Session::beginHandshake()
{
	debug_d("SSL: handshake start");
#ifndef SSL_SLOW_CONNECT
	curFreq = System.getCpuFrequency();
	if(curFreq != eCF_160MHz) {
		debug_d("SSL: Switching to 160 MHz");
		System.setCpuFrequency(eCF_160MHz); // For shorter waiting time, more power consumption.
	}
#endif
}

void Session::endHandshake()
{
#ifndef SSL_SLOW_CONNECT
	if(curFreq != System.getCpuFrequency()) {
		debug_d("SSL: Switching back to %u MHz", curFreq);
		System.setCpuFrequency(curFreq);
	}
#endif
	debug_d("SSL: Handshake done");
}

void Session::close()
{
	debug_d("SSL: closing ...");

	delete context;
	context = nullptr;

	delete connection;
	connection = nullptr;

	extension.clear();

	connected = false;
}

int Session::read(pbuf* encrypted, pbuf*& decrypted)
{
	assert(encrypted != nullptr);

	decrypted = nullptr;

	if(connection == nullptr) {
		pbuf_free(encrypted);
		return ERR_CONN;
	}

	/* SSL handshake needs time. In theory we have max 8 seconds before the hardware watchdog resets the device */
	WDT.alive();

	int read_bytes = connection->read(encrypted, decrypted);

	if(read_bytes < 0) {
		debug_d("SSL: Got error: %d", read_bytes);
		// @todo Perhaps change this to returning read_bytes == 0, then call method
		// on connection to determine alert code
		// Implementation error codes should be opaque.
		if(read_bytes == SSL_CLOSE_NOTIFY) {
			read_bytes = 0;
		}
	} else if(read_bytes != 0) {
		// we got some decrypted bytes...
		debug_d("SSL: Decrypted data len %d", read_bytes);
	}

	return read_bytes;
}

bool Session::handshakeComplete()
{
	assert(!connected);

	endHandshake();

	bool validated = validators.validate(connection->getCertificate());

	if(validated) {
		connected = true;

		// If requested, take a copy of the session ID for later re-use
		if(options & SSL_SESSION_RESUME) {
			if(sessionId == nullptr) {
				sessionId = new SessionId;
			}
			*sessionId = connection->getSessionId();
		}
	} else {
		debug_w("SSL Validation failed");
	}

	if(freeKeyCertAfterHandshake) {
		connection->freeCertificate();
	}

	return validated;
}

}; // namespace Ssl
