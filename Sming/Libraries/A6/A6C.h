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


class PictureDataStream: public MemoryDataStream {
	/** @brief  Get the stream type
	 *  @retval StreamType The stream type.
	 *
	 */
	StreamType getStreamType() {
		return eSST_Memory;
	};

	/** @brief  Read a block of memory
	 *  @param  data Pointer to the data to be read
	 *  @param  bufSize Quantity of chars to read
	 *  @retval uint16_t Quantity of chars read
	 */
	uint16_t readMemoryBlock(char* data, int bufSize) {

	};

	/** @brief  Move read cursor
	 *  @param  len Position within stream to move cursor to
	 *  @retval bool True on success.
	 */
	bool seek(int len) {
		// seeking is not allowed on the stream
		return false;
	};

	/** @brief  Check if stream is finished
	 *  @retval bool True on success.
	 */
	bool isFinished() {
		return finished;
	}

	void setFinished(bool flag) {
		finished = flag;
	}

private:
	bool finished = false;

};

class A6C: public AtClient {

public:
	A6C(HardwareSerial* stream, size_t bufferSize = 4096);

	// GSM Related

	// Camera Related
	void startCamera(eResolution resolution = eResVGA);
	bool takePicture(PictureDataStream* outputStream);
	void stopCamera();

protected:
	bool onPictureSize(AtClient& atClient, String& reply);
	bool onPictureReceive(AtClient &camera, Stream& source);

private:
	PictureDataStream* picureDataStream;
	size_t bufferSize;
	size_t pictureSize = 0;
	size_t readSize = 0;
};

#endif /* LIBRARIES_A6_H_ */
