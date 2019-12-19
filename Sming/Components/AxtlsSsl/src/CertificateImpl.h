/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * CertificateImpl.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include <Network/Ssl/Certificate.h>
#include <axtls-8266/compat/lwipr_compat.h>

namespace Ssl
{
class CertificateImpl : public Certificate
{
public:
	CertificateImpl(SSL* ssl) : ssl(ssl)
	{
	}

	~CertificateImpl()
	{
	}

	bool matchFingerprint(const uint8_t* hash) const override;

	bool matchPki(const uint8_t* hash) const override;

	const String getName(const Name& name) const override;

private:
	SSL* ssl = nullptr;
};

} // namespace Ssl
