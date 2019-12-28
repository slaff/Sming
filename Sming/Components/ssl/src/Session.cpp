/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Session.cpp
 *
 ****/

#include <Network/Ssl/Session.h>
#include <Network/Ssl/Factory.h>
#include <Network/TcpConnection.h>
#include <Print.h>

namespace Ssl
{
String Options::toString() const
{
	String s;

#define ADD(field)                                                                                                     \
	if(field) {                                                                                                        \
		if(s) {                                                                                                        \
			s += ", ";                                                                                                 \
			s += _F(#field);                                                                                           \
		}                                                                                                              \
	}

	ADD(sessionResume);
	ADD(clientAuthentication);
	ADD(verifyLater);
	ADD(freeKeyCertAfterHandshake);

#undef ADD

	return s;
}

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
	options.freeKeyCertAfterHandshake = true;

	return true;
}

bool Session::onAccept(TcpConnection* client)
{
	assert(context != nullptr);
	auto server = context->createServer();
	client->setSslConnection(server);
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

int Session::read(InputBuffer& input, uint8_t*& output)
{
	assert(connection != nullptr);
	int len = connection->read(input, output);
	if(len < 0) {
		debug_w("SSL: Got error: %d (%s)", len, connection->getErrorString(len).c_str());
	}

	return len;
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
		if(options.sessionResume) {
			if(sessionId == nullptr) {
				sessionId = new SessionId;
			}
			*sessionId = connection->getSessionId();
		}
	} else {
		debug_w("SSL Handshake failed");
	}

	if(options.freeKeyCertAfterHandshake) {
		connection->freeCertificate();
	}
}

size_t Session::printTo(Print& p) const
{
	size_t n = 0;

	n += p.println(_F("SSL Session:"));
	n += p.print(_F("  Options: "));
	n += p.println(options.toString());
	n += p.print(_F("  Host name: "));
	n += p.println(hostName);
	n += p.print(_F("  Cache Size: "));
	n += p.println(cacheSize);
	n += p.print(_F("  Fragment Size: "));
	n += p.println(fragmentSize);
	n += p.print(_F("  Validators: "));
	n += p.println(validators.count());
	n += p.print(_F("  Cert Length: "));
	n += p.println(keyCert.getCertificateLength());
	n += p.print(_F("  Cert PK Length: "));
	n += p.println(keyCert.getKeyLength());
	if(connection != nullptr) {
		n += connection->printTo(p);
	}

	return n;
}

}; // namespace Ssl
