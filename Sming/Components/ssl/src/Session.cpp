
#include <Network/Ssl/Session.h>
#include <Network/Ssl/Factory.h>
#include <Platform/WDT.h>

namespace Ssl
{
bool Session::listen(tcp_pcb* tcp)
{
#ifdef SSL_DEBUG
	options |= SSL_DISPLAY_STATES | SSL_DISPLAY_BYTES | SSL_DISPLAY_CERTS;
#endif

	delete context;
	assert(factory != nullptr);
	context = factory->createContext();
	if(context == nullptr) {
		return false;
	}

	if(!context->init(tcp, options, cacheSize)) {
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

err_t Session::onConnected(tcp_pcb* tcp)
{
	debug_d("SSL: Starting connection...");

	// Client Session
	delete context;
	assert(factory != nullptr);
	context = factory->createContext();
	if(context == nullptr) {
		return ERR_ABRT;
	}

	uint32_t localOptions = options;
#ifdef SSL_DEBUG
	localOptions |= SSL_DISPLAY_STATES | SSL_DISPLAY_BYTES | SSL_DISPLAY_CERTS;
	debug_d("SSL: Show debug data ...");
#endif

	if(!context->init(tcp, localOptions, 1)) {
		return ERR_MEM;
	}

	if(!context->setKeyCert(keyCert)) {
		debug_e("SSL: Error loading keyCert");
		return ERR_ABRT;
	}

//	if(freeKeyCertAfterHandshake) {
//		keyCert.free();
//	}

	if(sessionId != nullptr && sessionId->isValid()) {
		debug_d("-----BEGIN SSL SESSION PARAMETERS-----");
		debug_d("SessionId: %s", sessionId->toString().c_str());
		debug_d("------END SSL SESSION PARAMETERS------");
	}

	beginHandshake();

	connection = context->createClient(sessionId, extension);
	if(connection == nullptr) {
		endHandshake();
		return ERR_ABRT;
	}

	if(!connection->isHandshakeDone()) {
		debug_d("SSL: handshake is in progress...");
		return ERR_INPROGRESS;
	}

	endHandshake();

	if(sessionId != nullptr) {
		*sessionId = connection->getSessionId();
	}

	return ERR_OK;
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
		if(read_bytes == SSL_CLOSE_NOTIFY) {
			read_bytes = 0;
		}
	} else if(read_bytes == 0) {
		if(!connected && connection->isHandshakeDone()) {
			connected = true;

			endHandshake();

			if(!validators.validate(connection->getCertificate())) {
				debug_w("SSL Validation failed");

				if(decrypted != nullptr) {
					pbuf_free(decrypted);
					decrypted = nullptr;
				}

				return ERR_ABRT;
			}

			if(sessionId != nullptr) {
				*sessionId = connection->getSessionId();
			}

			if(freeKeyCertAfterHandshake) {
				connection->freeCertificate();
			}

			return ERR_OK;
		}
	} else {
		// we got some decrypted bytes...
		debug_d("SSL: Decrypted data len %d", read_bytes);
	}

	return read_bytes;
}

}; // namespace Ssl
