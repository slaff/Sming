/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * AxContext.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include <Network/Ssl/Context.h>
#include <axtls-8266/ssl/ssl.h>

namespace Ssl
{
class AxContext : public Context
{
public:
	using Context::Context;
	~AxContext();

	bool init(uint32_t options, size_t sessionCacheSize) override;
	bool setKeyCert(KeyCertPair& keyCert) override;
	Connection* createClient() override;
	Connection* createServer() override;

private:
	SSL_CTX* context = nullptr;
	int lastError = SSL_OK;
};

} // namespace Ssl
