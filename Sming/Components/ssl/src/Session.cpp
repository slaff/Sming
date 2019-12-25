
#include <Network/Ssl/Session.h>
#include <Network/Ssl/Factory.h>
#include <Network/TcpConnection.h>

namespace Ssl
{
bool Session::listen(tcp_pcb* tcp)
{
	if(!keyCert.isValid()) {
		debug_e("SSL: server certificate and key are not provided!");
		return false;
	}

	delete context;
	assert(factory != nullptr);
	context = factory->createContext(*this, tcp);
	if(context == nullptr) {
		return false;
	}

	if(!context->init()) {
		return false;
	}

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

	cacheSize = 1;

	if(!context->init()) {
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

	delete connection;
	connection = nullptr;

	delete context;
	context = nullptr;

	hostName = nullptr;
	fragmentSize = eSEFS_Off;

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

int Session::write(const uint8_t* data, size_t length)
{
	if(connection == nullptr) {
		return ERR_CONN;
	}

	int res = connection->write(data, length);
	if(res < 0) {
		// @todo Add a method to obtain a more appropriate TCP error code
		return ERR_BUF;
	}

	return res;
}

bool Session::validateCertificate()
{
	if(validators.validate(connection->getCertificate())) {
		debug_i("SSL validation passed, heap free = %u", system_get_free_heap_size());
		return true;
	}

	debug_w("SSL Validation failed");
	return false;
}

void Session::handshakeComplete(bool success)
{
	assert(!connected);

	endHandshake();

	if(success) {
		connected = true;

		// If requested, take a copy of the session ID for later re-use
		if(options & eSO_SESSION_RESUME) {
			if(sessionId == nullptr) {
				sessionId = new SessionId;
			}
			*sessionId = connection->getSessionId();
		}
	} else {
		debug_w("SSL Handshake failed");
	}

	if(freeKeyCertAfterHandshake) {
		connection->freeCertificate();
	}
}

}; // namespace Ssl
