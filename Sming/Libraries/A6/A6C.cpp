/*
 * A6C.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 */

#include "A6C.h"

A6C::A6C(HardwareSerial *stream, size_t bufferSize) : AtClient(stream) {
	stream->setRxBufferSize(bufferSize);
	this->bufferSize = bufferSize;
}

void A6C::startCamera(eResolution resolution /* = eResVGA */){
	send("ATE0\r"); // A.
	send("AT+CAMSTOP\r", "+CME ERROR:53"); // OK or ERROR:53 when the camera is not started
	send("AT+CAMSTART="+String(resolution) +"\r", "OK", 3000); // Max 3 seconds
}

void A6C::stopCamera() {
	send("AT+CAMSTOP\r", "+CME ERROR:53"); // OK or ERROR:53 when the camera is not started
}



bool A6C::onPictureSize(AtClient& atClient, String& reply) {
	if(reply.indexOf("+CAMCAP:") != 0) { // +CAMCAP:8305
		return false;
	}

	pictureSize = atoi(reply.substring(7).c_str());

	int chunks = (pictureSize + 11) % this->bufferSize;
	int offset = 0;
	AtCommand command;
	command.onReceive = AtReceiveCallback(&A6C::onPictureReceive, this);
	char commandBuffer[32];
	for(int i=0; i<chunks; i++) {
		int  length = offset + this->bufferSize;
		ets_sprintf(commandBuffer, "AT+CAMRD=%d:%d\r", offset, length);
		command.text = String(commandBuffer);
		send(command);
	}

	return true;
}


bool A6C::onPictureReceive(AtClient &camera, Stream &source) {
	// End of stream: 00 0d 0a  0d 0a 0d 0a 4f 4b 0d 0a
	//	              0x0\r\n\r\n\r\nOK\r\n  // <!-- 11 bytes as end marker

	int max = source.available();
	if(max < 1) {
		return false;
	}

	char ch;
	while(source.available()) {
		ch = source.read();
		if(ch == -1) {
			break;
		}

		readSize++;
		picureDataStream->write(ch);
	}

	if(readSize+11 == pictureSize) {
		return true;
	}

	return false;
}

bool A6C::takePicture(PictureDataStream* outputStream) {
	picureDataStream = outputStream;
	send("AT+CAMCAP\r", AtCompleteCallback(&A6C::onPictureSize, this), 4000); // Max 4 seconds, Response: +CAMCAP:8305
	return true;
}
