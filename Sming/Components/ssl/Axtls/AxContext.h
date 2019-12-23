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
	~AxContext();

	bool init(tcp_pcb* tcp, uint32_t options, size_t sessionCacheSize) override;
	bool setKeyCert(KeyCertPair& keyCert) override;
	Connection* createClient(SessionId* sessionId, const Extension& extension) override;
	Connection* createServer() override;

private:
	SSLCTX* context = nullptr;
	tcp_pcb* tcp = nullptr;
	int lastError = SSL_OK;
};

} // namespace Ssl
