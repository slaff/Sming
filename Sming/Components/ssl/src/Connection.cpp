#include <Network/Ssl/Connection.h>
#include <Print.h>

namespace Ssl
{
String cipherSuiteName(CipherSuite id)
{
	switch(id) {
#define XX(n1, n2, tag)                                                                                                \
	case CipherSuite::tag:                                                                                             \
		return F(#tag);
		SSL_CIPHER_SUITE_MAP(XX)
#undef XX
	default: {
		char buf[32];
		auto len = m_snprintf(buf, sizeof(buf), _F("{ 0x%02X, 0x%02X }"), unsigned(id) >> 8, unsigned(id) & 0xFF);
		return String(buf, len);
	}
	}
}

size_t Connection::printTo(Print& p) const
{
	size_t n = 0;
	n += p.println(_F("SSL Connection Information:"));
	auto cert = getCertificate();
	if(cert != nullptr) {
		n += p.print(_F("  Certificate:  "));
		n += p.println(cert->getName(Certificate::Name::CERT_COMMON_NAME));
	}
	n += p.print(_F("  Cipher:       "));
	n += p.println(cipherSuiteName(getCipherSuite()));
	n += p.print(_F("  Session ID:   "));
	n += p.println(getSessionId());
	return n;
}

int Connection::read(pbuf* encrypted, pbuf*& decrypted)
{
	assert(encrypted != nullptr);
	assert(tcpData.buf == nullptr);

	if(encrypted->tot_len == 0) {
		// nothing to read
		return 0;
	}

	// Setup read buffer
	tcpData.buf = encrypted;
	tcpData.offset = 0;

	int totalBytes = 0;
	uint8_t* totalReadBuffer = nullptr;
	do {
		uint8_t* readBuffer = nullptr;
		int readBytes = decrypt(readBuffer);
		if(readBytes > 0) {
			debug_d("SSL read: %d bytes", readBytes);
		}
		if(readBytes < 0) {
			/* An error has occurred. Give it back for further processing */
			if(totalBytes == 0) {
				// Nothing is read so far -> give back the error
				totalBytes = readBytes;
			} else {
				// We already have read some data -> deliver it back and silence the error for now..
				debug_w("SSL read: Silently ignoring SSL error %d", readBytes);
			}

			break;
		}

		if(readBytes == 0) {
			continue;
		}

		if(totalReadBuffer == nullptr) {
			totalReadBuffer = (uint8_t*)malloc(readBytes);
		} else {
			debug_i("SSL read: Got more than one SSL packet inside one TCP packet");
			uint8_t* newBuffer = (uint8_t*)realloc(totalReadBuffer, totalBytes + readBytes);
			if(newBuffer == nullptr) {
				free(totalReadBuffer);
				totalReadBuffer = nullptr;
			} else {
				totalReadBuffer = newBuffer;
			}
		}

		if(totalReadBuffer == nullptr) {
			debug_e("SSL read: Unable to allocate additional %d bytes", readBytes);
			totalBytes = ERR_MEM;
			break;
		}

		memcpy(totalReadBuffer + totalBytes, readBuffer, readBytes);
		totalBytes += readBytes;

		debug_i("tcpData.offset = %u, tot_len = %u", tcpData.offset, tcpData.buf->tot_len);

	} while(tcpData.offset < tcpData.buf->tot_len);

	tcpData.buf = nullptr;
	pbuf_free(encrypted);

	if(totalBytes > 0) {
		// put the decrypted data in a brand new pbuf
		decrypted = pbuf_alloc(PBUF_TRANSPORT, totalBytes, PBUF_RAM);
		if(decrypted != nullptr) {
			memcpy(decrypted->payload, totalReadBuffer, totalBytes);
		} else {
			debug_e("Unable to allocate pbuf memory. Required %d. Check MEM_SIZE in your lwipopts.h file and "
					"increase if needed.",
					totalBytes);
			totalBytes = ERR_MEM;
		}
		free(totalReadBuffer);
	}

	return totalBytes;
}

size_t Connection::readTcpData(uint8_t* buf, size_t bufSize)
{
	if(tcpData.buf == nullptr) {
		return 0;
	}

	unsigned len = pbuf_copy_partial(tcpData.buf, buf, bufSize, tcpData.offset);
	tcpData.offset += len;

	if(len < bufSize) {
		debug_d("SSL readTcpData: Bytes needed: %d, Bytes read: %u", bufSize, len);
	}

	return len;
}

int Connection::writeTcpData(uint8_t* data, size_t length)
{
	assert(tcp != nullptr);

	if(data == nullptr || length == 0) {
		debug_w("writeTcpData: Return Zero.");
		return 0;
	}

	debug_hex(INFO, "WRITE", data, length);

	int tcp_len = 0;
	if(tcp_sndbuf(tcp) < length) {
		tcp_len = tcp_sndbuf(tcp);
		if(tcp_len == 0) {
			tcp_output(tcp);
			debug_e("writeTcpData: The send buffer is full! We have problem.");
			return 0;
		}

	} else {
		tcp_len = length;
	}

	if(tcp_len > 2 * tcp->mss) {
		tcp_len = 2 * tcp->mss;
	}

	err_t err;
	while((err = tcp_write(tcp, data, tcp_len, TCP_WRITE_FLAG_COPY)) == ERR_MEM) {
		debug_e("writeTcpData: Not enough memory to write data with length: %d (%d)", tcp_len, length);
		tcp_len /= 2;
		if(tcp_len <= 1) {
			tcp_len = 0;
			break;
		}
	}

	if(err == ERR_OK) {
		debug_d("writeTcpData: length %d (%d)", tcp_len, length);
		err = tcp_output(tcp);
		if(err != ERR_OK) {
			debug_e("writeTcpData: tcp_output got err: %d", err);
		}
	} else {
		debug_e("writeTcpData: Got error: %d", err);
	}

	return tcp_len;
}

} // namespace Ssl
