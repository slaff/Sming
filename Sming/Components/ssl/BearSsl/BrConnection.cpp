/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * BrConnection.cpp
 *
 * @author: 2019 - mikee47 <mike@sillyhouse.net>
 *
 ****/

/*
 */
#include "debug.h"
#include "BrConnection.h"
#include "BrError.h"
#include <Network/Ssl/Session.h>
#include <Network/Ssl/Alert.h>
#include <FlashString/Map.hpp>
#include <FlashString/Array.hpp>

namespace Ssl
{
#define XX(tag, text) DEFINE_FSTR_LOCAL(errStr_##tag, #tag)
BR_ERROR_MAP(XX)
#undef XX

#define XX(tag, text) {BR_ERR_##tag, &errStr_##tag},
DEFINE_FSTR_MAP_LOCAL(errorMap, int, FSTR::String, BR_ERROR_MAP(XX));
#undef XX

String BrConnection::getErrorString(int error) const
{
	if(error < 0) {
		error = -error;
	}
	if(error >= BR_ERR_SEND_FATAL_ALERT) {
		auto alert = Alert(error - BR_ERR_SEND_FATAL_ALERT);
		return F("SEND_") + getAlertString(alert);
	} else if(error >= BR_ERR_RECV_FATAL_ALERT) {
		auto alert = Alert(error - BR_ERR_RECV_FATAL_ALERT);
		return F("RECV_") + getAlertString(alert);
	} else {
		auto s = String(errorMap[error]);
		return s ?: F("Unknown_") + String(error);
	}
}

int BrClientConnection::init()
{
	DEFINE_FSTR_ARRAY_LOCAL(FS_suitesBasic, CipherSuite, CipherSuite::RSA_WITH_AES_128_CBC_SHA256,
							CipherSuite::RSA_WITH_AES_256_CBC_SHA256, CipherSuite::RSA_WITH_AES_128_CBC_SHA,
							CipherSuite::RSA_WITH_AES_256_CBC_SHA);

	DEFINE_FSTR_ARRAY_LOCAL(
		FS_suitesFull, CipherSuite, CipherSuite::ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
		CipherSuite::ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256, CipherSuite::ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
		CipherSuite::ECDHE_RSA_WITH_AES_128_GCM_SHA256, CipherSuite::ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
		CipherSuite::ECDHE_RSA_WITH_AES_256_GCM_SHA384, CipherSuite::ECDHE_ECDSA_WITH_AES_128_CCM,
		CipherSuite::ECDHE_ECDSA_WITH_AES_256_CCM, CipherSuite::ECDHE_ECDSA_WITH_AES_128_CCM_8,
		CipherSuite::ECDHE_ECDSA_WITH_AES_256_CCM_8, CipherSuite::ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
		CipherSuite::ECDHE_RSA_WITH_AES_128_CBC_SHA256, CipherSuite::ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
		CipherSuite::ECDHE_RSA_WITH_AES_256_CBC_SHA384, CipherSuite::ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
		CipherSuite::ECDHE_RSA_WITH_AES_128_CBC_SHA, CipherSuite::ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
		CipherSuite::ECDHE_RSA_WITH_AES_256_CBC_SHA, CipherSuite::ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
		CipherSuite::ECDH_RSA_WITH_AES_128_GCM_SHA256, CipherSuite::ECDH_ECDSA_WITH_AES_256_GCM_SHA384,
		CipherSuite::ECDH_RSA_WITH_AES_256_GCM_SHA384, CipherSuite::ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
		CipherSuite::ECDH_RSA_WITH_AES_128_CBC_SHA256, CipherSuite::ECDH_ECDSA_WITH_AES_256_CBC_SHA384,
		CipherSuite::ECDH_RSA_WITH_AES_256_CBC_SHA384, CipherSuite::ECDH_ECDSA_WITH_AES_128_CBC_SHA,
		CipherSuite::ECDH_RSA_WITH_AES_128_CBC_SHA, CipherSuite::ECDH_ECDSA_WITH_AES_256_CBC_SHA,
		CipherSuite::ECDH_RSA_WITH_AES_256_CBC_SHA, CipherSuite::RSA_WITH_AES_128_GCM_SHA256,
		CipherSuite::RSA_WITH_AES_256_GCM_SHA384, CipherSuite::RSA_WITH_AES_128_CCM, CipherSuite::RSA_WITH_AES_256_CCM,
		CipherSuite::RSA_WITH_AES_128_CCM_8, CipherSuite::RSA_WITH_AES_256_CCM_8,
		CipherSuite::RSA_WITH_AES_128_CBC_SHA256, CipherSuite::RSA_WITH_AES_256_CBC_SHA256,
		CipherSuite::RSA_WITH_AES_128_CBC_SHA, CipherSuite::RSA_WITH_AES_256_CBC_SHA,
		CipherSuite::ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA, CipherSuite::ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,
		CipherSuite::ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA, CipherSuite::ECDH_RSA_WITH_3DES_EDE_CBC_SHA,
		CipherSuite::RSA_WITH_3DES_EDE_CBC_SHA);

	auto& FS_suites = FS_suitesFull;

	br_ssl_client_zero(&clientContext);
	auto engine = &clientContext.eng;
	br_ssl_engine_set_versions(engine, BR_TLS10, BR_TLS12);

	LOAD_FSTR_ARRAY(suites, FS_suites);
	br_ssl_engine_set_suites(engine, (uint16_t*)suites, FS_suites.length());
	br_ssl_client_set_default_rsapub(&clientContext);
	br_ssl_engine_set_default_rsavrfy(engine);
	br_ssl_engine_set_default_ecdsa(engine);

	// Set supported hash functions for the SSL engine
#define INSTALL_HASH(hash) br_ssl_engine_set_hash(engine, br_##hash##_ID, &br_##hash##_vtable);
	INSTALL_HASH(md5)
	INSTALL_HASH(sha1)
	INSTALL_HASH(sha224)
	INSTALL_HASH(sha256)
	INSTALL_HASH(sha384)
	INSTALL_HASH(sha512)
#undef INSTALL_HASH

	// Set the PRF implementations
	br_ssl_engine_set_prf10(engine, &br_tls10_prf);
	br_ssl_engine_set_prf_sha256(engine, &br_tls12_sha256_prf);
	br_ssl_engine_set_prf_sha384(engine, &br_tls12_sha384_prf);

	// Symmetric encryption
	br_ssl_engine_set_default_aes_cbc(engine);
	br_ssl_engine_set_default_aes_ccm(engine);
	br_ssl_engine_set_default_aes_gcm(engine);
	br_ssl_engine_set_default_des_cbc(engine);
	br_ssl_engine_set_default_chapol(engine);

	// X509 verification
	x509Context = new X509Context([this]() { return context.getSession().validateCertificate(); });
	br_ssl_engine_set_x509(engine, *x509Context);

	// Set Mono-directional buffer size according to requested max. fragment size
	auto fragSize = context.getSession().fragmentSize ?: eSEFS_4K;
	size_t bufSize = (256U << fragSize) + (BR_SSL_BUFSIZE_MONO - 16384U);
	debug_i("Using buffer size of %u bytes", bufSize);
	delete buffer;
	buffer = new uint8_t[bufSize];
	if(buffer == nullptr) {
		debug_e("Buffer allocation failed");
		return BR_ERR_BAD_PARAM;
	}
	br_ssl_engine_set_buffer(&clientContext.eng, buffer, bufSize, 0);

	br_ssl_client_reset(&clientContext, context.getSession().hostName.c_str(), 0);

	InputBuffer input(nullptr);
	return runUntil(input, BR_SSL_SENDAPP | BR_SSL_RECVAPP);
}

const Certificate* BrClientConnection::getCertificate() const
{
	if(certificate == nullptr) {
		certificate = new BrCertificate();
		x509Context->getCertificateHash(certificate->sha1Hash);
	}

	return certificate;
}

int BrClientConnection::read(InputBuffer& input, uint8_t*& output)
{
	int state = runUntil(input, BR_SSL_RECVAPP);
	if(state < 0) {
		return state;
	}

	if((state & BR_SSL_RECVAPP) == 0) {
		return 0;
	}

	size_t len = 0;
	output = br_ssl_engine_recvapp_buf(&clientContext.eng, &len);
	debug_hex(DBG, "READ", output, len);
	br_ssl_engine_recvapp_ack(&clientContext.eng, len);
	return len;
}

int BrClientConnection::write(const uint8_t* data, size_t length)
{
	InputBuffer input(nullptr);
	int state = runUntil(input, BR_SSL_SENDAPP);
	if(state < 0) {
		return state;
	}

	if((state & BR_SSL_SENDAPP) == 0) {
		return -1;
	}

	size_t available;
	auto buf = br_ssl_engine_sendapp_buf(&clientContext.eng, &available);
	debug_d("SSL: Expected: %d, Available: %u", length, available);
	if(available < length) {
		return -1;
	}

	memcpy(buf, data, length);
	br_ssl_engine_sendapp_ack(&clientContext.eng, length);
	br_ssl_engine_flush(&clientContext.eng, 0);
	runUntil(input, BR_SSL_SENDAPP | BR_SSL_RECVAPP);

	return ERR_OK;
}

int BrClientConnection::runUntil(InputBuffer& input, unsigned target)
{
	auto engine = &clientContext.eng;

	for(;;) {
		unsigned state = br_ssl_engine_current_state(engine);

		if(state & BR_SSL_CLOSED) {
			int err = br_ssl_engine_last_error(engine);
			debug_w("SSL CLOSED, last error = %d (%s), heap free = %u", err, getErrorString(err).c_str(),
					system_get_free_heap_size());
			return -1;
		}

		if(!handshakeDone && (state & BR_SSL_SENDAPP)) {
			handshakeDone = true;
			context.getSession().handshakeComplete(true);
			debug_i("Negotiated MFLN: %u", br_ssl_engine_get_mfln_negotiated(engine));
			continue;
		}

		/*
		 * If there is some record data to send, do it. This takes
		 * precedence over everything else.
		 */
		if(state & BR_SSL_SENDREC) {
			size_t len;
			auto buf = br_ssl_engine_sendrec_buf(engine, &len);
			int wlen = writeTcpData(buf, len);
			if(wlen < 0) {
				debug_w("SSL SHUTDOWN");
				/*
				 * If we received a close_notify and we
				 * still send something, then we have our
				 * own response close_notify to send, and
				 * the peer is allowed by RFC 5246 not to
				 * wait for it.
				 */
				if(!engine->shutdown_recv) {
					//				br_ssl_engine_fail(engine, BR_ERR_IO);
				}
				return -1;
			}
			if(wlen > 0) {
				br_ssl_engine_sendrec_ack(engine, wlen);
			}

			continue;
		}

		/*
		 * If we reached our target, then we are finished.
		 */
		if(state & target) {
			return state;
		}

		// Conflict: Application data hasn't been read
		if(state & BR_SSL_RECVAPP) {
			debug_e("SSL: Protocol Error");
			return -1;
		}

		if(state & BR_SSL_RECVREC) {
			size_t avail = 0;
			auto buf = br_ssl_engine_recvrec_buf(engine, &avail);
			auto len = input.read(buf, avail);
			if(len == 0) {
				return state;
			}

			debug_hex(DBG, "READ", buf, len);
			br_ssl_engine_recvrec_ack(engine, len);

			continue;
		}

		// Make room for new incoming records
		br_ssl_engine_flush(engine, 0);
	}
}

} // namespace Ssl
