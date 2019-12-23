#pragma once

#include <axtls-8266/ssl/ssl.h>

#define SSL_X509_NOT_OK SSL_X509_ERROR(X509_NOT_OK)
#define SSL_X509_VFY_ERROR_NO_TRUSTED_CERT SSL_X509_ERROR(X509_VFY_ERROR_NO_TRUSTED_CERT)
#define SSL_X509_VFY_ERROR_BAD_SIGNATURE SSL_X509_ERROR(X509_VFY_ERROR_BAD_SIGNATURE)
#define SSL_X509_VFY_ERROR_NOT_YET_VALID SSL_X509_ERROR(X509_VFY_ERROR_NOT_YET_VALID)
#define SSL_X509_VFY_ERROR_EXPIRED SSL_X509_ERROR(X509_VFY_ERROR_EXPIRED)
#define SSL_X509_VFY_ERROR_SELF_SIGNED SSL_X509_ERROR(X509_VFY_ERROR_SELF_SIGNED)
#define SSL_X509_VFY_ERROR_INVALID_CHAIN SSL_X509_ERROR(X509_VFY_ERROR_INVALID_CHAIN)
#define SSL_X509_VFY_ERROR_UNSUPPORTED_DIGEST SSL_X509_ERROR(X509_VFY_ERROR_UNSUPPORTED_DIGEST)
#define SSL_X509_INVALID_PRIV_KEY SSL_X509_ERROR(X509_INVALID_PRIV_KEY)
#define SSL_X509_MAX_CERTS SSL_X509_ERROR(X509_MAX_CERTS)
#define SSL_X509_VFY_ERROR_BASIC_CONSTRAINT SSL_X509_ERROR(X509_VFY_ERROR_BASIC_CONSTRAINT)

#define AX_ERROR_MAP(XX)                                                                                               \
	XX(OK)                                                                                                             \
	XX(NOT_OK)                                                                                                         \
	XX(ERROR_DEAD)                                                                                                     \
	XX(CLOSE_NOTIFY)                                                                                                   \
	XX(ERROR_CONN_LOST)                                                                                                \
	XX(ERROR_RECORD_OVERFLOW)                                                                                          \
	XX(ERROR_SOCK_SETUP_FAILURE)                                                                                       \
	XX(ERROR_INVALID_HANDSHAKE)                                                                                        \
	XX(ERROR_INVALID_PROT_MSG)                                                                                         \
	XX(ERROR_INVALID_HMAC)                                                                                             \
	XX(ERROR_INVALID_VERSION)                                                                                          \
	XX(ERROR_UNSUPPORTED_EXTENSION)                                                                                    \
	XX(ERROR_INVALID_SESSION)                                                                                          \
	XX(ERROR_NO_CIPHER)                                                                                                \
	XX(ERROR_INVALID_CERT_HASH_ALG)                                                                                    \
	XX(ERROR_BAD_CERTIFICATE)                                                                                          \
	XX(ERROR_INVALID_KEY)                                                                                              \
	XX(ERROR_FINISHED_INVALID)                                                                                         \
	XX(ERROR_NO_CERT_DEFINED)                                                                                          \
	XX(ERROR_NO_CLIENT_RENOG)                                                                                          \
	XX(ERROR_NOT_SUPPORTED)                                                                                            \
	XX(X509_NOT_OK)                                                                                                    \
	XX(X509_VFY_ERROR_NO_TRUSTED_CERT)                                                                                 \
	XX(X509_VFY_ERROR_BAD_SIGNATURE)                                                                                   \
	XX(X509_VFY_ERROR_NOT_YET_VALID)                                                                                   \
	XX(X509_VFY_ERROR_EXPIRED)                                                                                         \
	XX(X509_VFY_ERROR_SELF_SIGNED)                                                                                     \
	XX(X509_VFY_ERROR_INVALID_CHAIN)                                                                                   \
	XX(X509_VFY_ERROR_UNSUPPORTED_DIGEST)                                                                              \
	XX(X509_INVALID_PRIV_KEY)                                                                                          \
	XX(X509_MAX_CERTS)                                                                                                 \
	XX(X509_VFY_ERROR_BASIC_CONSTRAINT)
