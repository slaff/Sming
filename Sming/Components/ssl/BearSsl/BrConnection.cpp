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
#include "BrConnection.h"
#include "BrError.h"
#include <Network/Ssl/Session.h>
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
	auto s = String(errorMap[error]);
	return s ?: F("Unknown_") + String(error);
}

int BrClientConnection::init()
{
	DEFINE_FSTR_ARRAY_LOCAL(
		FS_suites, uint16_t, BR_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
		BR_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256, BR_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
		BR_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256, BR_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
		BR_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384, BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM,
		BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM, BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
		BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8, BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
		BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256, BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
		BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384, BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
		BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA, BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
		BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA, BR_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
		BR_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256, BR_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,
		BR_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384, BR_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
		BR_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256, BR_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384,
		BR_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384, BR_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,
		BR_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA, BR_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA,
		BR_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA, BR_TLS_RSA_WITH_AES_128_GCM_SHA256, BR_TLS_RSA_WITH_AES_256_GCM_SHA384,
		BR_TLS_RSA_WITH_AES_128_CCM, BR_TLS_RSA_WITH_AES_256_CCM, BR_TLS_RSA_WITH_AES_128_CCM_8,
		BR_TLS_RSA_WITH_AES_256_CCM_8, BR_TLS_RSA_WITH_AES_128_CBC_SHA256, BR_TLS_RSA_WITH_AES_256_CBC_SHA256,
		BR_TLS_RSA_WITH_AES_128_CBC_SHA, BR_TLS_RSA_WITH_AES_256_CBC_SHA, BR_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA,
		BR_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA, BR_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA,
		BR_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA, BR_TLS_RSA_WITH_3DES_EDE_CBC_SHA);

	br_ssl_client_zero(&clientContext);
	auto engine = &clientContext.eng;
	br_ssl_engine_set_versions(engine, BR_TLS10, BR_TLS12);

	LOAD_FSTR_ARRAY(suites, FS_suites);
	br_ssl_engine_set_suites(engine, suites, FS_suites.length());
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
	x509Context = new X509Context(context);
	br_ssl_engine_set_x509(engine, *x509Context);

	br_ssl_engine_set_buffer(&clientContext.eng, buffer, sizeof(buffer), 1);
	br_ssl_client_reset(&clientContext, context.getExtension().hostName.c_str(), 0);

	return runUntil(BR_SSL_SENDAPP | BR_SSL_RECVAPP);
}

const Certificate* BrClientConnection::getCertificate() const
{
	if(certificate == nullptr) {
		certificate = new BrCertificate();
		x509Context->getCertificateHash(certificate->sha1Hash);
	}

	return certificate;

	/*
	if(validators == nullptr) {
		// No validation
		return true;
	}

	assert(certificate == nullptr);
	certificate = new BrCertificate();

	x509Context->getCertificateHash(certificate->sha1Hash);
	return validators->validate(certificate);

	//	Check self-signed
	//	char issuerSig[SHA256_SIZE];
	//	char subjectSig[SHA256_SIZE];
	//	return memcmp(x509Context->getSubjectHash(subjectSig), x509Context->getIssuerHash(issuerSig),
	//				  sizeof(SHA256_SIZE)) == 0;
*/
}

int BrClientConnection::decrypt(uint8_t*& buffer)
{
	int state = runUntil(BR_SSL_RECVAPP);
	if(state < 0) {
		return state;
	}

	if(state != BR_SSL_RECVAPP) {
		return 0;
	}

	size_t len = 0;
	size_t avail = 0;
	buffer = br_ssl_engine_recvapp_buf(&clientContext.eng, &avail);
	debug_i("avail = %u", avail);
	len = readTcpData(buffer, avail);
	//		debug_hex(INFO, "READ", buffer, len);
	br_ssl_engine_recvapp_ack(&clientContext.eng, len);

	return len;
}

int BrClientConnection::write(const uint8_t* data, size_t length)
{
	int state = runUntil(BR_SSL_SENDAPP);
	if(state < 0) {
		return state;
	}

	if(state != BR_SSL_SENDAPP) {
		return 0;
	}

	size_t available;
	auto buf = br_ssl_engine_sendapp_buf(&clientContext.eng, &available);
	debug_d("SSL: Expected: %d, Available: %u", length, available);
	if(available < length) {
		return ERR_MEM;
	}

	memcpy(buf, data, length);
	br_ssl_engine_sendapp_ack(&clientContext.eng, length);

	return length;
}

int BrClientConnection::runUntil(unsigned target)
{
	auto engine = &clientContext.eng;

	for(;;) {
		unsigned state = br_ssl_engine_current_state(engine);

		if(state & BR_SSL_CLOSED) {
			int err = br_ssl_engine_last_error(engine);
			debug_w("SSL CLOSED, last error = %d (%s), heap free = %u", err, getErrorString(err).c_str(),
					system_get_free_heap_size());
			return SSL_CLOSE_NOTIFY;
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
			return state & target;
		}

		/*
		 * If some application data must be read, and we did not
		 * exit, then this means that we are trying to write data,
		 * and that's not possible until the application data is
		 * read. This may happen if using a shared in/out buffer,
		 * and the underlying protocol is not strictly half-duplex.
		 * This is unrecoverable here, so we report an error.
		 */
		if(state & BR_SSL_RECVAPP) {
			return -1;
		}

		if(state & BR_SSL_RECVREC) {
			size_t avail = 0;
			auto buf = br_ssl_engine_recvrec_buf(engine, &avail);
			auto len = readTcpData(buf, avail);
			if(len == 0) {
				return 0;
			}

			//		debug_hex(INFO, "READ", buf, len);
			br_ssl_engine_recvrec_ack(engine, len);
			continue;
		}

		/*
		 * We can reach that point if the target RECVAPP, and
		 * the state contains SENDAPP only. This may happen with
		 * a shared in/out buffer. In that case, we must flush
		 * the buffered data to "make room" for a new incoming
		 * record.
		 */
		br_ssl_engine_flush(engine, 0);
	}
}

} // namespace Ssl
