#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "../Libraries/A6/A6C.h"
#include "../../Services/WebHelpers/base64.h"

#include "../SmingCore/ProducerConsumer.h"

HardwareSerial Serial1(UART1);
A6C* camera;

HttpServer server;

MemoryDataStream buffer;
Producer producer(512);

void onFile(HttpRequest &request, HttpResponse &response)
{
	/*
	HTTP/1.0 200 OK
	Content-type: multipart/x-mixed-replace;boundary=--7b3cc56e5f51db803f790dad720ed50a
	Cache-Control: no-cache

	                                                               --7b3cc56e5f51|
	00000010  64 62 38 30 33 66 37 39  30 64 61 64 37 32 30 65  |db803f790dad720e|
	00000020  64 35 30 61 0d 0a 43 6f  6e 74 65 6e 74 2d 54 79  |d50a..Content-Ty|
	00000030  70 65 3a 20 69 6d 61 67  65 2f 6a 70 65 67 0d 0a  |pe: image/jpeg..|
	00000040  43 6f 6e 74 65 6e 74 2d  4c 65 6e 67 74 68 3a 20  |Content-Length: |
	00000050  35 36 32 35 35 0d 0a 0d  0a ff d8 ff e0 00 10 4a  |56255


	boundary
	Content-Type: image/jpeg
	Content-Length: ....

	*/
	char boundary[33];
	char bin[16];

	for (int i = 0; i < 16; ++i) {
		bin[i] = 1 + os_random() % 255;
	}

	base64_encode(16, (const unsigned char*)bin, 32, (char*)boundary);
	boundary[32] = '\0';

	bool connected = true;

	response.setContentType("multipart/x-mixed-replace;boundary=--"+String(boundary));
	response.setHeader("Cache-Control","no-cache");

	do {
		PictureDataStream pictureStream;
		camera->takePicture(&pictureStream);
		response.sendString("--"+String(boundary)+"\r\n");
		response.sendString("Content-Type: image/jpg\r\n");
		response.sendString("\r\n");
		response.sendDataStream(&pictureStream);
	} while(connected);
}

void ready() {
	camera = new A6C(&Serial);
	camera->startCamera();

	server.listen(80);
	server.setDefaultHandler(onFile);
}


void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	/* Toggle UART0 to use pins GPIO13/GPIO15 as RX and TX */
#if 0
	Serial.swap();
#endif

	/**
	 * Serial1 uses UART1, TX pin is GPIO2.
	 * UART1 can not be used to receive data because normally
	 * it's RX pin is occupied for flash chip connection.
	 *
	 * If you have a spare serial to USB converter do the following to see the
	 * messages printed on UART1:
	 * - connect converter GND to esp8266 GND
	 * - connect converter RX to esp8266 GPIO2
	 */
	Serial1.begin(SERIAL_BAUD_RATE);
	Serial1.systemDebugOutput(true); // UART1 will be used to log debug information

	// Set system ready callback method
	System.onReady(ready);

	// Soft access point
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config("SmingAP", "", AUTH_OPEN);
}
