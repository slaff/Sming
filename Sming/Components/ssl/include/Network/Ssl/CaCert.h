/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * KeyCertPair.h
 *
 ****/

#pragma once

#include "WString.h"

namespace Ssl
{
/**
 * @brief Class to manage an CA certificate
 */
class CaCert
{
public:
	bool isValid() const
	{
		return certificate;
	}

	/** @brief Create certificate using provided values
	 *  @param newCertificate
	 *  @param newCertificateLength
	 *  @retval bool false on memory allocation failure
	 *  @note We take a new copy of the certificate
	 *  @{
	 */
	bool assign(const uint8_t* newCertificate, size_t newCertificateLength);

	bool assign(String newCertificate)
	{
		certificate = newCertificate;
		return certificate;
	}
	/** @} */

	/** @brief Assign another certificate to this structure
	 *  @param caCert
	 *  @retval bool false on memory allocation failure
	 *  @note We take a new copy of the certificate
	 */
	bool assign(const CaCert& caCert)
	{
		*this = caCert;
		return (certificate == caCert.certificate);
	}

	void free()
	{
		certificate = nullptr;
	}

	const uint8_t* getCertificate() const
	{
		return reinterpret_cast<const uint8_t*>(certificate.c_str());
	}

	size_t getCertificateLength() const
	{
		return certificate.length();
	}

private:
	String certificate;
};

} // namespace Ssl
