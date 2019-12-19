/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * SslContextImpl.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include <Network/Ssl/SslContext.h>
#include <axtls-8266/compat/lwipr_compat.h>

class SslContextImpl : public SslContext
{
public:
	~SslContextImpl();

	bool init(tcp_pcb* tcp, uint32_t options, size_t sessionCacheSize) override;
	SslConnection* createServer() override;
	bool loadMemory(const SslContextObject& memType, const uint8_t* data, size_t length, const char* password) override;

protected:
	SslConnection* internalCreateClient(const uint8_t* sessionData, size_t length,
										SslExtension* sslExtensions) override;

private:
	uint8_t capacity = 10;
	SSLCTX* context = nullptr;
};
