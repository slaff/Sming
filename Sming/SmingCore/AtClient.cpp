/*
 * AtClient.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 */

#include <AtClient.h>

#ifndef debugf
#define debugf(fmt, ...)
#endif

AtClient::AtClient(HardwareSerial* stream) : stream(stream) {
	this->stream->setCallback(StreamDataReceivedDelegate(&AtClient::processor, this));
}

void AtClient::processor(Stream &source, char arrivedChar, uint16_t availableCharsCount) {
	if(!currentCommand.name.length()) {
		return;
	}

	if(state == eAtError) {
		// discard input at error state
		return;
	}

	if(currentCommand.callback) {
		currentCommand.callback(*this, source, arrivedChar, availableCharsCount);
	}

	if(arrivedChar != '\n') {
		return;
	}

	commandTimer.stop();
	debugf("Processing: %d ms, %s", millis(), currentCommand.name.substring(0, 20).c_str());

	char response[availableCharsCount];
	for (int i = 0; i < availableCharsCount; i++) {
		response[i] = stream->read();
		if (response[i] == '\r' || response[i] == '\n') {
			response[i] = '\0';
		}
	}

	debugf("Got response: %s", response);

	String reply(response);
	if(reply.indexOf(AT_REPLY_OK) + reply.indexOf(currentCommand.response2) == -2) {
		// we did not get what we wanted. Check if we should repeat.
		if(--currentCommand.retries > 0) {
			sendDirect(currentCommand);
			return;
		}

		if(currentCommand.breakOnError) {
			state = eAtError;
			return;
		}
	}

	next();
}

// Low Level  Communication Functions

void AtClient::send(String name, uint32_t timeoutMs /* = AT_TIMEOUT */, int retries /* = 0 */) {
	AtCommand atCommand;
	atCommand.name = name;
	atCommand.timeout = timeoutMs;
	atCommand.retries = retries;

	send(atCommand);
}

void AtClient::send(String name, String expectedResponse2, AtCallback onResponse, void *data /* =NULL */, uint32_t timeoutMs /* = AT_TIMEOUT */, int retries /* = 0 */) {
	AtCommand atCommand;
	atCommand.name = name;
	atCommand.response2 = expectedResponse2;
	atCommand.callback = onResponse;
	atCommand.data = data;
	atCommand.timeout = timeoutMs;
	atCommand.retries = retries;

	send(atCommand);
}

void AtClient::send(AtCommand command) {
	if(currentCommand.name.length()) {
		queue.enqueue(command);
		return;
	}

	sendDirect(command);
}

void AtClient::sendDirect(AtCommand command) {
	state = eAtRunning;
	commandTimer.stop();
	currentCommand = command;
	stream->print(command.name);
	debugf("Sent: timeout: %d, current %d ms, name: %s", currentCommand.timeout, millis(), command.name.substring(0, 20).c_str());
	commandTimer.initializeMs(currentCommand.timeout, TimerDelegate(&AtClient::ticker, this)).startOnce();
}

// Low Level Queue Functions
void AtClient::resend() {
	state = eAtOK;
	if(currentCommand.name.length()) {
		sendDirect(currentCommand);
		return;
	}

	next();
}

void AtClient::next() {
	if(state == eAtError) {
		debugf("We are at error state! No next");
		return;
	}

	state = eAtOK;
	currentCommand.name = "";
	if(queue.count() > 0) {
		send(queue.dequeue());
	}
}

void AtClient::ticker() {
	debugf("Ticker =================> ");
	if(!currentCommand.name.length()) {
		commandTimer.stop();
		debugf("Error: Timeout without command?!");
		return;
	}

	currentCommand.retries--;
	debugf("Retries: %d", currentCommand.retries);
	if(currentCommand.retries > 0) {
		commandTimer.restart();
		sendDirect(currentCommand);
		return;
	}

	state = eAtError;

	debugf("Timeout: %d ms, %s", millis(), currentCommand.name.c_str());
}
