/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * BrConnection.h
 *
 * @author: 2019 - mikee47 <mike@sillyhouse.net>
 *
 ****/

#pragma once

#include <Network/Ssl/Connection.h>
#include "BrError.h"
#include "BrCertificate.h"
#include "X509Context.h"

namespace Ssl
{
class BrConnection : public Connection
{
public:
	using Connection::Connection;

	String getErrorString(int error) const override
	{
		return Ssl::getErrorString(error);
	}
};

class BrClientConnection : public BrConnection
{
public:
	using BrConnection::BrConnection;

	~BrClientConnection()
	{
		delete certificate;
		delete x509Context;
		delete[] buffer;
	}

	int init();

	const Certificate* getCertificate() const override;

	void freeCertificate() override
	{
		delete certificate;
		certificate = nullptr;
	}

	int read(InputBuffer& input, uint8_t*& output) override;

	int write(const uint8_t* data, size_t length) override;

	CipherSuite getCipherSuite() const override
	{
		if(handshakeDone) {
			return CipherSuite(clientContext.eng.session.cipher_suite);
		} else {
			return CipherSuite::NULL_WITH_NULL_NULL;
		}
	}

	SessionId getSessionId() const override
	{
		SessionId id;
		if(handshakeDone) {
			auto& param = clientContext.eng.session;
			id.assign(param.session_id, param.session_id_len);
		}

		return id;
	}

	bool isHandshakeDone() const override
	{
		return handshakeDone;
	}

	int runUntil(InputBuffer& input, unsigned target);

private:
	br_ssl_client_context clientContext;
	uint8_t* buffer = nullptr;
	X509Context* x509Context = nullptr;
	mutable BrCertificate* certificate = nullptr;
	bool handshakeDone = false;
};

class BrServerConnection : public BrConnection
{
};

} // namespace Ssl
