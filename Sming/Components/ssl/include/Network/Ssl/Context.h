/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Context.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include "Connection.h"
#include "KeyCertPair.h"
#include "Validator.h"

struct tcp_pcb;

namespace Ssl
{
/**
 * @ingroup ssl
 * @brief Encapsulates operations related to creating a SSL context.
 * 		  The SSL context is the one that can later on be used to create client or server SSL connections
 * @{
 */

class Session;

class Context
{
public:
	Context(Session& session, tcp_pcb* tcp) : session(session), tcp(tcp)
	{
		assert(tcp != nullptr);
	}

	virtual ~Context()
	{
	}

	/**
	 * @brief Initializer method that must be called after object creation and before the creation
	 * 		  of server or client connections
	 * @retval bool true on success
	 */
	virtual bool init() = 0;

	/**
	 * @brief Creates client SSL connection.
	 *        Your SSL client use this call to make create a client connection to remote server.
	 * @retval Connection*
	 */
	virtual Connection* createClient() = 0;

	/**
	 * @brief Creates server SSL connection.
	 *        Your SSL servers use this call to allow remote clients to connect to them and use SSL.
	 * @retval Connection*
	 */
	virtual Connection* createServer() = 0;

	tcp_pcb* getTcp()
	{
		return tcp;
	}

	Session& getSession()
	{
		return session;
	}

protected:
	Session& session;
	tcp_pcb* tcp = nullptr;
};

/** @} */

} // namespace Ssl
