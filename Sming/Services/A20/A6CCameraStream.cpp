/*
 * A20VideoStream.cpp
 *
 *  Created on: Jun 5, 2018
 *      Author: slavey
 */

#include "A6CCameraStream.h"

A20VideoStream::A20VideoStream(HardwareSerial& serial)
{
	atClient = new AtClient(&serial);
}

A20VideoStream::~A20VideoStream()
{
	delete atClient;
	atClient = NULL;
}

uint16_t A20VideoStream::readMemoryBlock(char* data, int bufSize)
{
	if(!pictureSize) {
		takePicture();
		return 0;
	}

	int available = pictureSize - (page * pageSize + readOffset);
	if(available < 1) {
		readImage(page * pageSize + readOffset, pageSize);
		page++;
		readOffset = 0;

		return 0;
	}

	if(bufSize > pageSize) {
		bufSize = pageSize;
	}

	memcpy(data, buffer + readOffset, bufSize);

	return bufSize;
}

bool A20VideoStream::processSize(AtClient& atClient, String& reply)
{
	/*
	 * ================
		AT+CAMCAP capture picture
		Set Command:
		Capture and store a Picture local and returns bytes of captured picture
		AT+CAMCAP
		+CAMCAP:<bytes>
		OK
		on error:
		+CME ERROR:
	* ================
	*/
	if(reply.indexOf("+CME ERROR") == 0) {
		debug_e("Unable to capture image");
		return false;
	}

	// TODO: Get the size of the captured image
	String length = reply.substring(8);
	pictureSize = atoi(length.c_str());

	return true;
}

void A20VideoStream::takePicture(eResolution resolution /* = eResVGA */)
{
	atClient->send("ATE0\r");
	atClient->send("AT+CAMSTOP\r");

	atClient->send("AT+CAMSTART="+String(resolution) +"\r");
	atClient->send("AT+CAMCAP\r", AtCompleteCallback(&A20VideoStream::processSize, this));
}

bool A20VideoStream::processImageData(AtClient& atClient, Stream& source)
{
	// TODO: store the data in the internal stream
	// - read all the chunks, return true if the last chunk was received.
	int available = source.available();
	writeOffset += source.readBytes((char *)(buffer + writeOffset), available);
}

bool A20VideoStream::readImage(int offset, size_t length)
{
	AtCommand command;

	// AT+CAMRD=fromaddr,toaddr
	char text[30];
	memset(text, 0, 30);
	ets_sprintf(text, "AT+CAMRD=%d:%d\r", page*pageSize + offset, (page + 1 )*pageSize + offset - 1);

	writeOffset = 0; // TODO ??
	command.text = text;
	command.breakOnError = false;
	command.onReceive = AtReceiveCallback(&A20VideoStream::processImageData,this);
	atClient->send(command);

	return true;
}

void A20VideoStream::stopCamera()
{
	atClient->send("AT+CAMSTOP\r");
}
