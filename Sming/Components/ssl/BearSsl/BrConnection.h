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
#include <Network/Ssl/Extension.h>
#include "BrCertificate.h"
#include "X509Context.h"

namespace Ssl
{
class BrConnection : public Connection
{
public:
	using Connection::Connection;

	String getErrorString(int error) const override;
};

class BrClientConnection : public BrConnection
{
public:
	using BrConnection::BrConnection;

	~BrClientConnection()
	{
		delete certificate;
	}

	int init();

	const Certificate* getCertificate() const override;

	void freeCertificate() override
	{
		delete certificate;
		certificate = nullptr;
	}

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

	int runUntil(unsigned target);

	int decrypt(uint8_t*& buffer) override;

private:
	br_ssl_client_context clientContext;
	uint8_t buffer[BR_SSL_BUFSIZE_MONO];
	X509Context* x509Context = nullptr;
	ValidatorList* validators = nullptr;
	mutable BrCertificate* certificate = nullptr;
	bool handshakeDone = false;
};

class BrServerConnection : public BrConnection
{
};

} // namespace Ssl
