/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * SslConnection.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/
#pragma once

#include <user_config.h>
#include "SslSessionId.h"
#include "SslCertificate.h"

/**
 * @ingroup ssl
 * @brief Provides the functionality needed to create SSL communicate over TCP
 * @{
 */

/* errors that can be generated - these are specific to AXTLS */
#define SSL_OK 0
#define SSL_NOT_OK -1
#define SSL_ERROR_DEAD -2
#define SSL_CLOSE_NOTIFY -3
#define SSL_ERROR_CONN_LOST -256
#define SSL_ERROR_RECORD_OVERFLOW -257
#define SSL_ERROR_SOCK_SETUP_FAILURE -258
#define SSL_ERROR_INVALID_HANDSHAKE -260
#define SSL_ERROR_INVALID_PROT_MSG -261
#define SSL_ERROR_INVALID_HMAC -262
#define SSL_ERROR_INVALID_VERSION -263
#define SSL_ERROR_UNSUPPORTED_EXTENSION -264
#define SSL_ERROR_INVALID_SESSION -265
#define SSL_ERROR_NO_CIPHER -266
#define SSL_ERROR_INVALID_CERT_HASH_ALG -267
#define SSL_ERROR_BAD_CERTIFICATE -268
#define SSL_ERROR_INVALID_KEY -269
#define SSL_ERROR_FINISHED_INVALID -271
#define SSL_ERROR_NO_CERT_DEFINED -272
#define SSL_ERROR_NO_CLIENT_RENOG -273
#define SSL_ERROR_NOT_SUPPORTED -274
#define SSL_X509_OFFSET -512

#define SSL_ALERT_CODE_MAP(XX)                                                                                         \
	XX(CLOSE_NOTIFY, 0)                                                                                                \
	XX(UNEXPECTED_MESSAGE, 10)                                                                                         \
	XX(BAD_RECORD_MAC, 20)                                                                                             \
	XX(RECORD_OVERFLOW, 22)                                                                                            \
	XX(HANDSHAKE_FAILURE, 40)                                                                                          \
	XX(BAD_CERTIFICATE, 42)                                                                                            \
	XX(UNSUPPORTED_CERTIFICATE, 43)                                                                                    \
	XX(CERTIFICATE_EXPIRED, 45)                                                                                        \
	XX(CERTIFICATE_UNKNOWN, 46)                                                                                        \
	XX(ILLEGAL_PARAMETER, 47)                                                                                          \
	XX(UNKNOWN_CA, 48)                                                                                                 \
	XX(DECODE_ERROR, 50)                                                                                               \
	XX(DECRYPT_ERROR, 51)                                                                                              \
	XX(INVALID_VERSION, 70)                                                                                            \
	XX(NO_RENEGOTIATION, 100)                                                                                          \
	XX(UNSUPPORTED_EXTENSION, 110)

/**
 * @brief Alert codes defined by the standard
 */
enum SslAlertCode {
#define XX(tag, code) SSL_ALERT##tag = code,
	SSL_ALERT_CODE_MAP(XX)
#undef XX
};

class SslConnection
{
public:
	virtual ~SslConnection()
	{
	}

	/**
	 * @brief Checks if the handshake has finished
	 * @retval bool true on success
	 */
	virtual bool isHandshakeDone() = 0;

	/**
	 * @brief Reads encrypted information and decrypts it
	 * @param tcp active tcp connection
	 * @param encrypted Source encrypted data
	 * @param decrypted Decrypted plaintext
	 *
	 * @retval
	 * 		 0 - when the handshake is still in progress
	 * 		 > 0 - when the is decrypted data
	 * 		 < 0 - in case of an error
	 */
	virtual int read(tcp_pcb* tcp, pbuf* encrypted, pbuf*& decrypted) = 0;

	/**
	 * @brief Converts and sends plaintext data
	 * @param data
	 * @param length
	 * @retval int length of the data that was actually written
	 */
	virtual int write(const uint8_t* data, size_t length) = 0;

	/**
	 * @brief Calculates the size of encrypted plain text data
	 * @param plainTextLength
	 * @retval int
	 */
	virtual int calcWriteSize(size_t plainTextLength) = 0;

	/**
	 * @brief Gets the name of the cipher that was used
	 * @retval String
	 */
	virtual const String getCipher() const = 0;

	/**
	 * @brief Gets the current session id object.
	 *        Should be called after handshake.
	 * @retval SslSessionId*
	 */
	virtual SslSessionId* getSessionId() = 0;

	/**
	 * @brief Gets the certificate object.
	 *        That object MUST be owned by the SslConnection implementation
	 *        and should not be freed outside of it
	 *
	 * @retval SslSessionId*
	 */
	virtual SslCertificate* getCertificate() = 0;
};

/** @} */
