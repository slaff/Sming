/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * SslCertificateImpl.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include <Network/Ssl/SslCertificate.h>
#include <axtls-8266/compat/lwipr_compat.h>

class SslCertificateImpl : public SslCertificate
{
public:
	SslCertificateImpl(SSL* ssl) : ssl(ssl)
	{
	}

	~SslCertificateImpl()
	{
	}

	bool matchFingerprint(const uint8_t* hash) const override;

	bool matchPki(const uint8_t* hash) const override;

	const String getName(const SslCertificateName& name) const override;

private:
	SSL* ssl = nullptr;
};
