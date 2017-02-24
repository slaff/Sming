/*
 * A6.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 */

#include "A6.h"

void A6::responseToStream(AtClient &camera, Stream &source, char arrivedChar, uint16_t availableCharsCount) {
	if(!camera.currentCommand.data) {
		return;
	}

	// End of stream: 00 0d 0a  0d 0a 0d 0a 4f 4b 0d 0a
	//	              0x0\r\n\r\n\r\nOK\r\n

	Stream *outputStream = (Stream *)camera.currentCommand.data;
	for(int i=0; i<availableCharsCount; i++) {
		int ch = source.read();
		if(ch == -1) {
			break;
		}

		outputStream->write(ch);
	}
}

bool A6::takePicture(Stream* outputStream, eResolution resolution /* = eResVGA */) {
	AtCommand command;

	send("ATE0\r");
	send("AT+CAMSTOP\r", "+CME ERROR:53"); // OK or ERROR:53 when the camera is not started

	send("AT+CAMSTART="+String(resolution) +"\r", "OK", 3000); // Max 3 seconds
	send("AT+CAMCAP\r", "+CAMCAP:", 4000); // Max 4 seconds, Response: +CAMCAP:8305

	command.name = "AT+CAMRD\r";
	command.breakOnError = false;
	command.callback = AtCallback(&A6::responseToStream,this);
	command.data = (void *)outputStream;
	send(command);

	send("AT+CAMSTOP\r");
}
