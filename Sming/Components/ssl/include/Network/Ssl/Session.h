#pragma once

#include "Context.h"
#include "KeyCertPair.h"
#include "Validator.h"
#include <Platform/System.h>

class TcpConnection;

/**
 * @brief SSL Options
 *
 * These are defined by AXTLS and handled also for other SSL implementations
 */
#define SSL_SESSION_RESUME 0x0008
#define SSL_CLIENT_AUTHENTICATION 0x00010000
#define SSL_SERVER_VERIFY_LATER 0x00020000
#define SSL_NO_DEFAULT_KEY 0x00040000
#define SSL_DISPLAY_STATES 0x00080000
#define SSL_DISPLAY_BYTES 0x00100000
#define SSL_DISPLAY_CERTS 0x00200000
#define SSL_DISPLAY_RSA 0x00400000

/**
 * @brief Maximum Fragment Length Negotiation https://tools.ietf.org/html/rfc6066
 *
 * 0,1,2,3..6 corresponding to off,512,1024,2048..16384 bytes
 *
 * The allowed values for this field are: 2^9, 2^10, 2^11, and 2^12
 *
 */
enum FragmentSize {
	eSEFS_Off,
	eSEFS_512, //<< 512 bytes
	eSEFS_1K,  //<< 1024 bytes
	eSEFS_2K,
	eSEFS_4K,
	eSEFS_8K,
	eSEFS_16K,
};

namespace Ssl
{
class Session
{
public:
	String hostName;
	FragmentSize fragmentSize = eSEFS_Off;
	KeyCertPair keyCert;
	bool freeKeyCertAfterHandshake = false;
	uint32_t options = 0;
	// server
	int cacheSize = 10;
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
	int write(const uint8_t* data, size_t length);

	bool validateCertificate();
	void handshakeComplete(bool success);

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
