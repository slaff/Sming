/*
 * A20VideoStream.h
 *
 *  Created on: Jun 5, 2018
 *      Author: slavey
 */

#ifndef SERVICES_A20_A6CCAMERASTREAM_H_
#define SERVICES_A20_A6CCAMERASTREAM_H_

#include "Data/Stream/DataSourceStream.h"
#include "SmingCore/AtClient.h"

typedef enum {
	eResQVGA = 0,
	eResVGA,
	eResQQVGA
} eResolution;

class A20VideoStream: public ReadWriteStream {
public:
	A20VideoStream(HardwareSerial& serial);
	virtual ~A20VideoStream();

	virtual size_t write(uint8_t charToWrite)
	{
		// writing is not allowed ...
		return 0;
	};

    /** @brief  Write chars to stream
     *  @param  buffer Pointer to buffer to write to the stream
     *  @param  size Quantity of chars to write
     *  @retval size_t Quantity of chars written to stream
     */
	virtual size_t write(const uint8_t *buffer, size_t size)
	{
		// writing is not allowed ...
		return 0;
	}

    //Use base class documentation
	virtual uint16_t readMemoryBlock(char* data, int bufSize);

	// Camera Related
	void takePicture(eResolution resolution = eResVGA);
	void stopCamera();

private:
	bool readImage(int offset, size_t length);
	bool processImageData(AtClient& atClient, Stream& source)
	bool processSize(AtClient& atClient, String& reply);

private:
	AtClient* atClient = nullptr;
	size_t pictureSize = 0;
	size_t readOffset = 0;
	size_t writeOffset = 0;
	size_t page = 0;
	size_t pageSize = 1024;
	uint8_t buffer[1024];
};

#endif /* SERVICES_A20_A6CCAMERASTREAM_H_ */
