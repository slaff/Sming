/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * ConnectionImpl.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include <Network/Ssl/Connection.h>
#include "CertificateImpl.h"
#include <axtls-8266/compat/lwipr_compat.h>

namespace Ssl
{
class ConnectionImpl : public Connection
{
public:
	ConnectionImpl(SSL* ssl) : ssl(ssl)
	{
	}

	~ConnectionImpl()
	{
		delete certificate;
		delete sessionId;
	}

	bool isHandshakeDone() override
	{
		return (ssl_handshake_status(ssl) == SSL_OK);
	}

	int read(tcp_pcb* tcp, pbuf* encrypted, pbuf*& decrypted) override;

	int write(const uint8_t* data, size_t length) override
	{
		return axl_ssl_write(ssl, data, length);
	}

	int calcWriteSize(size_t plainTextLength) override
	{
		return ssl_calculate_write_length(ssl, plainTextLength);
	}

	const String getCipher() const override;

	SessionId* getSessionId() override;

	Certificate* getCertificate() override;

protected:
	SSL* ssl = nullptr;
	CertificateImpl* certificate = nullptr; // << The certificate object is owned by this class
	SessionId* sessionId = nullptr;			// << The sslSessionId object is owned by this class
};

} // namespace Ssl
