/*
 * A6.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 *
 *
 *  Influenced from: https://github.com/skorokithakis/A6lib/blob/master/A6lib.cpp
 */

#ifndef LIBRARIES_A6_H_
#define LIBRARIES_A6_H_

#include "../SmingCore/AtClient.h"

typedef enum {
	eResQVGA = 0,
	eResVGA,
	eResQQVGA
} eResolution;

class A6: public AtClient {

public:
	using AtClient::AtClient;

	// GSM Related

	// Camera Related
	bool takePicture(Stream* outputStream, eResolution resolution = eResVGA);

private:
	void responseToStream(AtClient &camera, Stream &source, char arrivedChar, uint16_t availableCharsCount);
};

#endif /* LIBRARIES_A6_H_ */
