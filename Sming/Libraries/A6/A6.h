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
	bool onReceive(AtClient &camera, Stream &source);
};

#endif /* LIBRARIES_A6_H_ */
