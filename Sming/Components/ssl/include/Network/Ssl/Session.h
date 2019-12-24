#pragma once

#include "Context.h"
#include "KeyCertPair.h"
#include "Validator.h"
#include <Platform/System.h>

class TcpConnection;

namespace Ssl
{
class Session
{
public:
	Extension extension;
	KeyCertPair keyCert;
	bool freeKeyCertAfterHandshake = false;
	uint32_t options = 0;
	// server
	int cacheSize = 50;
	// client
	ValidatorList validators;

public:
	~Session()
	{
		close();
		delete sessionId;
	}

	const SessionId* getSessionId() const
	{
		return sessionId;
	}

	/**
	 * @brief Called by server to prepare for listening
	 */
	bool listen(tcp_pcb* tcp);

	bool onAccept(TcpConnection* client);

	void setConnection(Connection* connection)
	{
		delete this->connection;
		this->connection = connection;
	}

	Connection* getConnection()
	{
		return connection;
	}

	/**
	 * @brief Handle connection event
	 * @retval err_t
	 */
	bool onConnect(tcp_pcb* tcp);

	bool isConnected() const
	{
		return connected;
	}

	/**
	 * @brief End the session
	 */
	void close();

	/**
	 * @brief Process received encrypted data
	 * @param encrypted The input data, will be freed before returning
	 * @param decrypted On success, the decrypted data
	 * @retval int Number of bytes received, or error code
	 */
	int read(pbuf* encrypted, pbuf*& decrypted);

	/**
	 * @brief Write data to SSL connection
	 * @param data
	 * @param length
	 * @retval int Quantity of bytes actually written, or tcp error code
	 */
	int write(const uint8_t* data, size_t length)
	{
		if(connection == nullptr) {
			return ERR_CONN;
		}

		int res = connection->write(data, length);
		if(res < 0) {
			// @todo Add a method to obtain a more appropriate TCP error code
			return ERR_BUF;
		}

		return res;
	}

	// Called by connection to perform certificate validation
	bool handshakeComplete();

private:
	void beginHandshake();
	void endHandshake();

private:
	Context* context = nullptr;
	Connection* connection = nullptr;
	SessionId* sessionId = nullptr;
	CpuFrequency curFreq;
	bool connected = false;
};

}; // namespace Ssl
