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

namespace Ssl
{
String AxConnection::getErrorString(int error) const
{
	return F("Error_") + String(error);
}

int AxConnection::write(const uint8_t* data, size_t length)
{
	int expected = ssl_calculate_write_length(ssl, length);
	u16_t available = tcp ? tcp_sndbuf(tcp) : 0;
	debug_d("SSL: Expected: %d, Available: %u", expected, available);
	if(expected < 0 || int(available) < expected) {
		return ERR_MEM;
	}

	int written = ssl_write(ssl, data, length);
	debug_d("SSL: Write len: %d, Written: %d", length, written);
	if(written < 0) {
		debug_e("SSL: Write Error: %d", written);
		return written;
	}

	return ERR_OK;
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
