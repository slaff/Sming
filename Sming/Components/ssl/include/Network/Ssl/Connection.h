/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Connection.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/
#pragma once

#include <user_config.h>
#include "SessionId.h"
#include "Certificate.h"

namespace Ssl
{
/**
 * @ingroup ssl
 * @brief Provides the functionality needed to create SSL communicate over TCP
 * @{
 */

#define SSL_ALERT_CODE_MAP(XX)                                                                                         \
	XX(CLOSE_NOTIFY, 0)                                                                                                \
	XX(UNEXPECTED_MESSAGE, 10)                                                                                         \
	XX(BAD_RECORD_MAC, 20)                                                                                             \
	XX(RECORD_OVERFLOW, 22)                                                                                            \
	XX(DECOMPRESSION_FAILURE, 30)                                                                                      \
	XX(HANDSHAKE_FAILURE, 40)                                                                                          \
	XX(BAD_CERTIFICATE, 42)                                                                                            \
	XX(UNSUPPORTED_CERTIFICATE, 43)                                                                                    \
	XX(CERTIFICATE_REVOKED, 44)                                                                                        \
	XX(CERTIFICATE_EXPIRED, 45)                                                                                        \
	XX(CERTIFICATE_UNKNOWN, 46)                                                                                        \
	XX(ILLEGAL_PARAMETER, 47)                                                                                          \
	XX(UNKNOWN_CA, 48)                                                                                                 \
	XX(ACCESS_DENIED, 49)                                                                                              \
	XX(DECODE_ERROR, 50)                                                                                               \
	XX(DECRYPT_ERROR, 51)                                                                                              \
	XX(INVALID_VERSION, 70)                                                                                            \
	XX(INSUFFICIENT_SECURITY, 71)                                                                                      \
	XX(INTERNAL_ERROR, 80)                                                                                             \
	XX(USER_CANCELLED, 90)                                                                                             \
	XX(NO_RENEGOTIATION, 100)                                                                                          \
	XX(UNSUPPORTED_EXTENSION, 110)                                                                                     \
	XX(NO_APPLICATION_PROTOCOL, 120)

/**
 * @brief Alert codes defined by the standard
 */
enum class Alert {
#define XX(tag, code) tag = code,
	SSL_ALERT_CODE_MAP(XX)
#undef XX
};

String getAlertString(Alert alert);

/*
 * Cipher suites
 *
 *	TLS v1.2	https://tools.ietf.org/html/rfc5246#appendix-A.5
 * 	TLS v1.3	https://tools.ietf.org/html/rfc8446#appendix-B.4
 *
 * Courtesy of Bear SSL. Defined here so they're not tied into any specific implementation.
 *
 */
#define SSL_CIPHER_SUITE_MAP(XX)                                                                                       \
	/* From RFC 5246 */                                                                                                \
	XX(NULL_WITH_NULL_NULL, 0x0000)                                                                                    \
	XX(RSA_WITH_NULL_MD5, 0x0001)                                                                                      \
	XX(RSA_WITH_NULL_SHA, 0x0002)                                                                                      \
	XX(RSA_WITH_NULL_SHA256, 0x003B)                                                                                   \
	XX(RSA_WITH_RC4_128_MD5, 0x0004)                                                                                   \
	XX(RSA_WITH_RC4_128_SHA, 0x0005)                                                                                   \
	XX(RSA_WITH_3DES_EDE_CBC_SHA, 0x000A)                                                                              \
	XX(RSA_WITH_AES_128_CBC_SHA, 0x002F)                                                                               \
	XX(RSA_WITH_AES_256_CBC_SHA, 0x0035)                                                                               \
	XX(RSA_WITH_AES_128_CBC_SHA256, 0x003C)                                                                            \
	XX(RSA_WITH_AES_256_CBC_SHA256, 0x003D)                                                                            \
	XX(DH_DSS_WITH_3DES_EDE_CBC_SHA, 0x000D)                                                                           \
	XX(DH_RSA_WITH_3DES_EDE_CBC_SHA, 0x0010)                                                                           \
	XX(DHE_DSS_WITH_3DES_EDE_CBC_SHA, 0x0013)                                                                          \
	XX(DHE_RSA_WITH_3DES_EDE_CBC_SHA, 0x0016)                                                                          \
	XX(DH_DSS_WITH_AES_128_CBC_SHA, 0x0030)                                                                            \
	XX(DH_RSA_WITH_AES_128_CBC_SHA, 0x0031)                                                                            \
	XX(DHE_DSS_WITH_AES_128_CBC_SHA, 0x0032)                                                                           \
	XX(DHE_RSA_WITH_AES_128_CBC_SHA, 0x0033)                                                                           \
	XX(DH_DSS_WITH_AES_256_CBC_SHA, 0x0036)                                                                            \
	XX(DH_RSA_WITH_AES_256_CBC_SHA, 0x0037)                                                                            \
	XX(DHE_DSS_WITH_AES_256_CBC_SHA, 0x0038)                                                                           \
	XX(DHE_RSA_WITH_AES_256_CBC_SHA, 0x0039)                                                                           \
	XX(DH_DSS_WITH_AES_128_CBC_SHA256, 0x003E)                                                                         \
	XX(DH_RSA_WITH_AES_128_CBC_SHA256, 0x003F)                                                                         \
	XX(DHE_DSS_WITH_AES_128_CBC_SHA256, 0x0040)                                                                        \
	XX(DHE_RSA_WITH_AES_128_CBC_SHA256, 0x0067)                                                                        \
	XX(DH_DSS_WITH_AES_256_CBC_SHA256, 0x0068)                                                                         \
	XX(DH_RSA_WITH_AES_256_CBC_SHA256, 0x0069)                                                                         \
	XX(DHE_DSS_WITH_AES_256_CBC_SHA256, 0x006A)                                                                        \
	XX(DHE_RSA_WITH_AES_256_CBC_SHA256, 0x006B)                                                                        \
	XX(DH_anon_WITH_RC4_128_MD5, 0x0018)                                                                               \
	XX(DH_anon_WITH_3DES_EDE_CBC_SHA, 0x001B)                                                                          \
	XX(DH_anon_WITH_AES_128_CBC_SHA, 0x0034)                                                                           \
	XX(DH_anon_WITH_AES_256_CBC_SHA, 0x003A)                                                                           \
	XX(DH_anon_WITH_AES_128_CBC_SHA256, 0x006C)                                                                        \
	XX(DH_anon_WITH_AES_256_CBC_SHA256, 0x006D)                                                                        \
	/* From, RFC, 4492, */                                                                                             \
	XX(ECDH_ECDSA_WITH_NULL_SHA, 0xC001)                                                                               \
	XX(ECDH_ECDSA_WITH_RC4_128_SHA, 0xC002)                                                                            \
	XX(ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA, 0xC003)                                                                       \
	XX(ECDH_ECDSA_WITH_AES_128_CBC_SHA, 0xC004)                                                                        \
	XX(ECDH_ECDSA_WITH_AES_256_CBC_SHA, 0xC005)                                                                        \
	XX(ECDHE_ECDSA_WITH_NULL_SHA, 0xC006)                                                                              \
	XX(ECDHE_ECDSA_WITH_RC4_128_SHA, 0xC007)                                                                           \
	XX(ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA, 0xC008)                                                                      \
	XX(ECDHE_ECDSA_WITH_AES_128_CBC_SHA, 0xC009)                                                                       \
	XX(ECDHE_ECDSA_WITH_AES_256_CBC_SHA, 0xC00A)                                                                       \
	XX(ECDH_RSA_WITH_NULL_SHA, 0xC00B)                                                                                 \
	XX(ECDH_RSA_WITH_RC4_128_SHA, 0xC00C)                                                                              \
	XX(ECDH_RSA_WITH_3DES_EDE_CBC_SHA, 0xC00D)                                                                         \
	XX(ECDH_RSA_WITH_AES_128_CBC_SHA, 0xC00E)                                                                          \
	XX(ECDH_RSA_WITH_AES_256_CBC_SHA, 0xC00F)                                                                          \
	XX(ECDHE_RSA_WITH_NULL_SHA, 0xC010)                                                                                \
	XX(ECDHE_RSA_WITH_RC4_128_SHA, 0xC011)                                                                             \
	XX(ECDHE_RSA_WITH_3DES_EDE_CBC_SHA, 0xC012)                                                                        \
	XX(ECDHE_RSA_WITH_AES_128_CBC_SHA, 0xC013)                                                                         \
	XX(ECDHE_RSA_WITH_AES_256_CBC_SHA, 0xC014)                                                                         \
	XX(ECDH_anon_WITH_NULL_SHA, 0xC015)                                                                                \
	XX(ECDH_anon_WITH_RC4_128_SHA, 0xC016)                                                                             \
	XX(ECDH_anon_WITH_3DES_EDE_CBC_SHA, 0xC017)                                                                        \
	XX(ECDH_anon_WITH_AES_128_CBC_SHA, 0xC018)                                                                         \
	XX(ECDH_anon_WITH_AES_256_CBC_SHA, 0xC019)                                                                         \
	/* From, RFC, 5288, */                                                                                             \
	XX(RSA_WITH_AES_128_GCM_SHA256, 0x009C)                                                                            \
	XX(RSA_WITH_AES_256_GCM_SHA384, 0x009D)                                                                            \
	XX(DHE_RSA_WITH_AES_128_GCM_SHA256, 0x009E)                                                                        \
	XX(DHE_RSA_WITH_AES_256_GCM_SHA384, 0x009F)                                                                        \
	XX(DH_RSA_WITH_AES_128_GCM_SHA256, 0x00A0)                                                                         \
	XX(DH_RSA_WITH_AES_256_GCM_SHA384, 0x00A1)                                                                         \
	XX(DHE_DSS_WITH_AES_128_GCM_SHA256, 0x00A2)                                                                        \
	XX(DHE_DSS_WITH_AES_256_GCM_SHA384, 0x00A3)                                                                        \
	XX(DH_DSS_WITH_AES_128_GCM_SHA256, 0x00A4)                                                                         \
	XX(DH_DSS_WITH_AES_256_GCM_SHA384, 0x00A5)                                                                         \
	XX(DH_anon_WITH_AES_128_GCM_SHA256, 0x00A6)                                                                        \
	XX(DH_anon_WITH_AES_256_GCM_SHA384, 0x00A7)                                                                        \
	/* From, RFC, 5289, */                                                                                             \
	XX(ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, 0xC023)                                                                    \
	XX(ECDHE_ECDSA_WITH_AES_256_CBC_SHA384, 0xC024)                                                                    \
	XX(ECDH_ECDSA_WITH_AES_128_CBC_SHA256, 0xC025)                                                                     \
	XX(ECDH_ECDSA_WITH_AES_256_CBC_SHA384, 0xC026)                                                                     \
	XX(ECDHE_RSA_WITH_AES_128_CBC_SHA256, 0xC027)                                                                      \
	XX(ECDHE_RSA_WITH_AES_256_CBC_SHA384, 0xC028)                                                                      \
	XX(ECDH_RSA_WITH_AES_128_CBC_SHA256, 0xC029)                                                                       \
	XX(ECDH_RSA_WITH_AES_256_CBC_SHA384, 0xC02A)                                                                       \
	XX(ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, 0xC02B)                                                                    \
	XX(ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, 0xC02C)                                                                    \
	XX(ECDH_ECDSA_WITH_AES_128_GCM_SHA256, 0xC02D)                                                                     \
	XX(ECDH_ECDSA_WITH_AES_256_GCM_SHA384, 0xC02E)                                                                     \
	XX(ECDHE_RSA_WITH_AES_128_GCM_SHA256, 0xC02F)                                                                      \
	XX(ECDHE_RSA_WITH_AES_256_GCM_SHA384, 0xC030)                                                                      \
	XX(ECDH_RSA_WITH_AES_128_GCM_SHA256, 0xC031)                                                                       \
	XX(ECDH_RSA_WITH_AES_256_GCM_SHA384, 0xC032)                                                                       \
	/* From, RFC, 6655, and, 7251, */                                                                                  \
	XX(RSA_WITH_AES_128_CCM, 0xC09C)                                                                                   \
	XX(RSA_WITH_AES_256_CCM, 0xC09D)                                                                                   \
	XX(RSA_WITH_AES_128_CCM_8, 0xC0A0)                                                                                 \
	XX(RSA_WITH_AES_256_CCM_8, 0xC0A1)                                                                                 \
	XX(ECDHE_ECDSA_WITH_AES_128_CCM, 0xC0AC)                                                                           \
	XX(ECDHE_ECDSA_WITH_AES_256_CCM, 0xC0AD)                                                                           \
	XX(ECDHE_ECDSA_WITH_AES_128_CCM_8, 0xC0AE)                                                                         \
	XX(ECDHE_ECDSA_WITH_AES_256_CCM_8, 0xC0AF)                                                                         \
	/* From, RFC, 7905, */                                                                                             \
	XX(ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256, 0xCCA8)                                                                \
	XX(ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256, 0xCCA9)                                                              \
	XX(DHE_RSA_WITH_CHACHA20_POLY1305_SHA256, 0xCCAA)                                                                  \
	XX(PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAB)                                                                      \
	XX(ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAC)                                                                \
	XX(DHE_PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAD)                                                                  \
	XX(RSA_PSK_WITH_CHACHA20_POLY1305_SHA256, 0xCCAE)                                                                  \
	/* From, RFC, 7507, */                                                                                             \
	XX(FALLBACK_SCSV, 0x5600)

/**
 * @brief Cipher suite identifier
 *
 * The TLS standard specifies codes using two 8-bit values.
 * We combine these into a single 16-bit value in MSB-LSB order.
 *
 * For example:
 *
 * TLS_RSA_WITH_AES_128_CBC_SHA = { 0x00, 0x2F } = 0x002F
 */
enum class CipherSuite : uint16_t {
#define XX(tag, code) tag = code,
	SSL_CIPHER_SUITE_MAP(XX)
#undef XX
};

/**
 * @brief Gets the name of the cipher suite
 * @param Cipher Suite identifier
 * @retval String
 */
String getCipherSuiteName(CipherSuite id);

class Context;

class Connection : public Printable
{
public:
	Connection(Context& context) : context(context)
	{
	}

	virtual ~Connection()
	{
	}

	/**
	 * @brief Checks if the handshake has finished
	 * @retval bool true on success
	 */
	virtual bool isHandshakeDone() const = 0;

	/**
	 * @brief Reads encrypted information and decrypts it
	 * @param encrypted Source encrypted data
	 * @param decrypted Decrypted plaintext
	 *
	 * @retval
	 * 		 0 - when the handshake is still in progress
	 * 		 > 0 - when the is decrypted data
	 * 		 < 0 - in case of an error
	 */
	virtual int read(pbuf* encrypted, pbuf*& decrypted);

	/**
	 * @brief Converts and sends plaintext data
	 * @param data
	 * @param length
	 * @retval int length of the data that was actually written
	 * 	< 0 on error
	 */
	virtual int write(const uint8_t* data, size_t length) = 0;

	/**
	 * @brief Gets the cipher suite that was used
	 * @retval CipherSuite IDs as defined by SSL/TLS standard
	 */
	virtual CipherSuite getCipherSuite() const = 0;

	/**
	 * @brief Gets the current session id object.
	 *        Should be called after handshake.
	 * @retval SessionId
	 */
	virtual SessionId getSessionId() const = 0;

	/**
	 * @brief Gets the certificate object.
	 *        That object MUST be owned by the Connection implementation
	 *        and should not be freed outside of it
	 *
	 * @retval Certificate* Returns NULL if there is no certificate available
	 */
	virtual const Certificate* getCertificate() const = 0;

	virtual void freeCertificate() = 0;

	/**
	 * @brief For debugging
	 */
	size_t printTo(Print& p) const override;

	size_t readTcpData(uint8_t* buf, size_t bufSize);

	int writeTcpData(uint8_t* data, size_t length);

	/*
	 * Called through from read() method.
	 * Read incoming data from `decryptSource`, updating `offset` field,
	 * and pass into SSL layer, then return a pointer to a block of
	 * decrypted data with length. Zero-length block is fine.
	 *
	 * This method only returns application data so during initial handshake will return 0.
	 */
	virtual int decrypt(uint8_t*& buffer) = 0;

	/**
	 * @brief Get string for error code
	 */
	virtual String getErrorString(int error) const = 0;

protected:
	Context& context;
	struct {
		struct pbuf* buf = nullptr;
		uint16_t offset = 0;
	} tcpData;
};

/** @} */

} // namespace Ssl
