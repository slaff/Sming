/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * AxConnection.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include <Network/Ssl/Connection.h>
#include "AxCertificate.h"

namespace Ssl
{
class AxConnection : public Connection
{
public:
	AxConnection(tcp_pcb* tcp) : tcp(tcp)
	{
	}

	~AxConnection()
	{
		delete certificate;
	}

	void init(SSL* ssl)
	{
		this->ssl = ssl;
	}

	bool isHandshakeDone() const override
	{
		return (ssl_handshake_status(ssl) == SSL_OK);
	}

	int read(pbuf* encrypted, pbuf*& decrypted) override;

	int write(const uint8_t* data, size_t length) override;

	CipherSuite getCipherSuite() const override
	{
		return CipherSuite(ssl_get_cipher_id(ssl));
	}

	SessionId getSessionId() const override
	{
		SessionId id;
		if(isHandshakeDone()) {
			id.assign(ssl->session_id, ssl->sess_id_size);
		}

		return id;
	}

	const Certificate* getCertificate() const override
	{
		if(certificate == nullptr && ssl->x509_ctx != nullptr) {
			certificate = new AxCertificate(ssl);
		}

		return certificate;
	}

	void freeCertificate() override
	{
		delete certificate;
		certificate = nullptr;
	}

public:
	// Called from axTLS
	int port_write(uint8_t* buf, uint16_t bytes_needed);
	int port_read(uint8_t* buf, int bytes_needed);

private:
	SSL* ssl;
	mutable AxCertificate* certificate = nullptr;
	struct tcp_pcb* tcp = nullptr;
	struct pbuf* tcp_pbuf = nullptr;
	int pbuf_offset = 0;
};

} // namespace Ssl
