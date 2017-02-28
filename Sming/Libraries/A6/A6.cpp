/*
 * A6.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 */

#include "A6.h"

bool A6::onReceive(AtClient &camera, Stream &source) {
	// End of stream: 00 0d 0a  0d 0a 0d 0a 4f 4b 0d 0a
	//	              0x0\r\n\r\n\r\nOK\r\n

	// TODO:...

}

bool A6::takePicture(Stream* outputStream, eResolution resolution /* = eResVGA */) {
	AtCommand command;

	send("ATE0\r"); // A.
	send("AT+CAMSTOP\r", "+CME ERROR:53"); // OK or ERROR:53 when the camera is not started // A.

	send("AT+CAMSTART="+String(resolution) +"\r", "OK", 3000); // Max 3 seconds
	send("AT+CAMCAP\r", "+CAMCAP:", 4000); // Max 4 seconds, Response: +CAMCAP:8305

	command.text = "AT+CAMRD\r";
	command.breakOnError = false;
//	command.onReceive = AtReceiveCallback(&A6::onReceive, this);
	command.onReceive = (AtReceiveCallback)([](AtClient& client, Stream& source) -> bool {

		// End of stream: 00 0d 0a  0d 0a 0d 0a 4f 4b 0d 0a
		//	              0x0\r\n\r\n\r\nOK\r\n

		// TODO:...


		return true;
	});

	send(command);

	send("AT+CAMSTOP\r");
}
