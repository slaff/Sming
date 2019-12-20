/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * CertificateImpl.cpp
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#include "CertificateImpl.h"

namespace Ssl
{
const String CertificateImpl::getName(const Name& name) const
{
	if(ssl == nullptr) {
		return nullptr;
	}

	return String(ssl_get_cert_dn(ssl, int(name)));
}

bool CertificateImpl::matchFingerprint(const uint8_t* hash) const
{
	return (ssl_match_fingerprint(ssl, hash) == 0);
}

bool CertificateImpl::matchPki(const uint8_t* hash) const
{
	return (ssl_match_spki_sha256(ssl, hash) == 0);
}

} // namespace Ssl
