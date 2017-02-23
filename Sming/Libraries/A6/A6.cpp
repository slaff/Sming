/*
 * A6.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 */

#include "A6.h"

#ifndef debugf
#define debugf() // TODO:add the correct signature
#endif

A6::A6(HardwareSerial& stream) {
	*this->stream = stream;
	this->stream->setCallback(StreamDataReceivedDelegate(&A6::processor, this));
}

void A6::processor(Stream &source, char arrivedChar, uint16_t availableCharsCount) {
	if(!currentCommand.name.length() || arrivedChar != '\n') {
		return;
	}

	char response[availableCharsCount];
	for (int i = 0; i < availableCharsCount; i++) {
		response[i] = stream->read();
		if (response[i] == '\r' || response[i] == '\n') {
			response[i] = '\0';
		}
	}

	debugf("Got response: %s", response);

	String reply(response);
	if(reply.indexOf(currentCommand.responseOk)== -1) {
		// we did not get what we wanted. Check if we should repeat.
		if(--currentCommand.retries > 0) {
			sendDirect(currentCommand);
			return;
		}
	}

	currentCommand.name = "";
	if(queue.count() > 0) {
		send(queue.pop());
	}
}

// Low Level Functions

/**
 * @brief Sends command to the A6 device
 * @param command String
 */
void A6::send(String name, String responseOK, String responseNOK, uint32_t timeoutMs /* = 1000 */, int retries /* = 0 */) {
	A6Command a6command;
	a6command.name = name;
	a6command.responseOk = responseOK;
	a6command.responseNOK = responseNOK;
	a6command.timeout = timeoutMs;
	a6command.retries = retries;

	send(a6command);
}

void A6::send(A6Command command) {
	if(currentCommand.name.length()) {
		queue.push(command);
		return;
	}

	sendDirect(command);
}

void A6::sendDirect(A6Command command) {
	commandTimer.stop();
	currentCommand = command;
	stream->print(command.name);
	commandTimer.initializeMs(currentCommand.timeout, TimerDelegate(&A6::timeout, this)).startOnce();
}

void A6::timeout() {
	if(!currentCommand.name.length()) {
		debugf("Error: Timeout without command?!");
		return;
	}

	commandTimer.restart();
	if(--currentCommand.retries > 0) {
		sendDirect(currentCommand);
	}
}
