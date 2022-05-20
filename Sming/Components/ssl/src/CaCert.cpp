/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * KeyCertPair.cpp
 *
 ****/

#include <Network/Ssl/CaCert.h>

namespace Ssl
{
bool CaCert::assign(const uint8_t* newCertificate, size_t newCertificateLength)
{
	free();

	if(newCertificateLength != 0 && newCertificate != nullptr) {
		if(!certificate.setLength(newCertificateLength)) {
			return false;
		}
		memcpy(certificate.begin(), newCertificate, newCertificateLength);
	}

	return true;
}

} // namespace Ssl
