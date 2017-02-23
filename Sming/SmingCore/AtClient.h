/*
 * AtClient.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 */

#ifndef _SMING_CORE_ATCLIENT_H_
#define _SMING_CORE_ATCLIENT_H_

#include "../SmingCore/HardwareSerial.h"
#include "../Wiring/FILO.h"
#include "../SmingCore/Delegate.h"
#include "../SmingCore/Timer.h"

class AtClient;

typedef Delegate<void(AtClient& atClient, Stream& source, char arrivedChar, uint16_t availableCharsCount)> AtCallback;

typedef struct {
	String name; // << the actual AT command
	String response1; // << successful response text
	String response2; // << alternative successful response
	int timeout; // << timeout in milliseconds
	int retries; // << number of retries before giving up
	bool breakOnError = true; // << stop excuting next command if that one has failed
	AtCallback callback=0; // << if that is set you can process manually all incoming data in a callback
	void *data=0; // << additional information to pass. Useful to pass information to the callback
} AtCommand;

typedef enum {
	eAtOK = 0,
	eAtRunning,
	eAtError
} AtState;

template<typename T, int rawSize>
class SimpleQueue: public FIFO<T, rawSize> {
	virtual const T& operator[](unsigned int) const { }
	virtual T& operator[](unsigned int) { }
};

/**
 * @brief Class that facilitates the communication with an AT device.
 */
class AtClient {

public:
	AtClient(HardwareSerial* stream);
	virtual ~AtClient() {}

	/**
	 * @brief Sends AT command
	 * @param name String The actual AT command text. For example AT+CAMSTOP
	 * @param expectedResponse1 String Expected response on success
	 * @param timeoutMs uint32_t Time in milliseconds to wait for response
	 * @param retries int Retries on error
	 */
	void send(String name, String expectedResponse1 = "OK", uint32_t timeoutMs = 1000, int retries = 0) {
		send(name, expectedResponse1, "YxZa", timeoutMs, retries);
	}

	/**
	 * @brief Sends AT command
	 * @param name String The actual AT command text. For example AT+CAMSTOP
	 * @param expectedResponse1 String Expected response on success
	 * @param expectedResponse2 String Alternative expected response on success
	 * @param timeoutMs uint32_t Time in milliseconds to wait for response
	 * @param retries int Retries on error
	 */
	void send(String name, String expectedResponse1, String expectedResponse2 = "OK", uint32_t timeoutMs = 1000, int retries = 0);

	/**
	 * @brief Sends AT command
	 * @param name String The actual AT command text. For example AT+CAMSTOP
	 * @param expectedResponse1 String Expected response on success
	 * @param expectedResponse2 String Alternative expected response on success
	 * @param timeoutMs uint32_t Time in milliseconds to wait for response
	 * @param retries int Retries on error
	 */
	void send(String name, AtCallback onResponse, void *data = NULL, uint32_t timeoutMs = 1000, int retries = 0);

	// Low Level Functions

	/**
	 * @brief Adds a command to the queue.
	 * 		  If you need all the flexibility then use that command
	 * 		  and mannually set your AtCommand arguments.
	 * @param command AtCommand
	 */
	void send(AtCommand command);

	/**
	 * @brief Executes directly (does not queue it) a command
	 * @param command AtCommand
	 */
	void sendDirect(AtCommand command);

	/**
	 * @brief Returns the current state
	 * @return AtState
	 */
	AtState getState() {
		return state;
	}

	/*
	 * @brief Repeats the execution of the current command
	 * 		  Useful if the current State is not eAtOK
	 */
	void resend();

	/*
	 * @brief Replaces the current command with the next on in the queue
	 */
	void next();

	AtCommand currentCommand; // << The current command

protected:
	/**
	 * @brief Processes response data.
	*/
	virtual void processor(Stream &source, char arrivedChar, uint16_t availableCharsCount);

private:
	SimpleQueue<AtCommand, 10> queue; // << Queue for the commands to be executed
	HardwareSerial* stream; // << The main communication stream
	Timer commandTimer; // timer used for commands with timeout
	AtState state = eAtOK;

	/**
	 * @brief Timeout checker method
	 */
	void ticker();
};

#endif /* _SMING_CORE_ATCLIENT_H_ */
