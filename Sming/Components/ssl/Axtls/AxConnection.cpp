/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * AxConnection.cpp
 *
 * @author: 2019 - mikee47 <mike@sillyhouse.net>
 *
 * Contains code from compatibility layer https://github.com/attachix/lwirax.git
 * so we can work directly with axTLS.
 *
 * Compatibility for AxTLS with LWIP raw tcp mode (http://lwip.wikia.com/wiki/Raw/TCP)
 *
 *  Created on: Jan 15, 2016
 *      Author: Slavey Karadzhov
 *
 ****/

/*
 */
#include "AxConnection.h"
#include "AxError.h"
#include "AxContext.h"
#include <FlashString/Map.hpp>

namespace Ssl
{
#define XX(tag) DEFINE_FSTR_LOCAL(errStr_##tag, #tag)
AX_ERROR_MAP(XX)
#undef XX

#define XX(tag) {SSL_##tag, &errStr_##tag},
DEFINE_FSTR_MAP_LOCAL(errorMap, int, FSTR::String, AX_ERROR_MAP(XX));
#undef XX

String AxConnection::getErrorString(int error) const
{
	auto s = String(errorMap[error]);
	return s ?: F("Unknown_") + String(error);
}

int AxConnection::write(const uint8_t* data, size_t length)
{
	int expected = ssl_calculate_write_length(ssl, length);
	if(expected < 0) {
		return expected;
	}

	auto tcp = context.getTcp();
	u16_t available = tcp ? tcp_sndbuf(tcp) : 0;
	if(int(available) < expected) {
		debug_d("SSL: Expected: %d, Available: %u", expected, available);
		return SSL_NOT_OK;
	}

	int written = ssl_write(ssl, data, length);
	debug_d("SSL: Write len: %d, Written: %d", length, written);
	if(written < 0) {
		debug_e("SSL: Write Error: %d", written);
		return written;
	}

	return SSL_OK;
}

int AxConnection::decrypt(uint8_t*& buffer)
{
	bool connected = isHandshakeDone();
	int readBytes = ssl_read(ssl, &buffer);
	if(!connected && isHandshakeDone()) {
		if(!context.validateCertificate()) {
			context.handshakeComplete(false);
			return SSL_ERROR_BAD_CERTIFICATE;
		}

		context.handshakeComplete(true);
	}

	return readBytes;
}

/*
 * Lower Level LWIP RAW functions
 */

/*
 * The LWIP tcp raw version of the SOCKET_WRITE(A, B, C)
 */
extern "C" int ax_port_write(int clientfd, uint8_t* buf, uint16_t bytes_needed)
{
	assert(clientfd != 0);

	auto connection = reinterpret_cast<AxConnection*>(clientfd);
	return connection->writeTcpData(buf, bytes_needed);
}

/*
 * The LWIP tcp raw version of the SOCKET_READ(A, B, C)
 */
extern "C" int ax_port_read(int clientfd, uint8_t* buf, int bytes_needed)
{
	assert(clientfd != 0);

	auto connection = reinterpret_cast<AxConnection*>(clientfd);
	return connection->readTcpData(buf, bytes_needed);
}

} // namespace Ssl
