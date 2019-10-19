/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Camera/ArduCamera.h
 *
 * @author: 2019 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include "CameraInterface.h"
#include <Core/HardwareSerial.h>
#include <Core/AtClient.h>

typedef enum {
	eResQVGA = 0,
	eResVGA,
	eResQQVGA
} eResolution;

class A20Camera: public CameraInterface
{
public:

	/**
	 * Sets the list of all images that should be used to roll over.
	 */
	A20Camera(HardwareSerial& serial, eResolution resolution = eResQVGA)
	{
		atClient = new AtClient(&serial);
		this->resolution = resolution;
	}

	~A20Camera()
	{
		delete atClient;
		atClient = nullptr;
	}

	const String getMimeType() const override
	{
		return "image/jpeg";
	}

	/**
	 * @brief Initializes the camera
	 *
	 * @retval true on success. The camera state should also change to eWCS_READY
	 */
	bool init() override
	{
		state = eWCS_READY;

		return true;
	}

	/**
	 * @brief Instructs the camera to capture new picture.
	 */
	bool capture() override
	{
		if(state == eWCS_NOT_READY && !init()) {
			return false;
		}

		atClient->send("ATE0\r");
		atClient->send("AT+CAMSTOP\r");

		atClient->send("AT+CAMSTART="+String(resolution) +"\r");
		atClient->send("AT+CAMCAP\r", AtCompleteCallback(&A20Camera::processSize, this));

		state = eWCS_WORKING;

		return true;
	}

	/**
	 * Gets the size of the current picture
	 */
	size_t getSize() override
	{
		return pictureSize;
	}

	void next() override
	{
		atClient->send("AT+CAMSTOP\r");
		CameraInterface::next();
	}

	/**
	 * @brief Read picture data from the camera.
	 * @param buffer the allocated data buffer to store the data
	 * @param size the size of the allocated buffer
	 * @param offset
	 *
	 * @retval bytes successfully read and stored in the buffer
	 */
	size_t read(char* buffer, size_t size, size_t offset = 0)
	{
		if(state == eWCS_WORKING && pictureSize) {
			// The picture size is determined. We are ready to go
			state = eWCS_HAS_PICTURE;
		}

		if(state != eWCS_HAS_PICTURE) {
			return 0;
		}

		// TODO: read back the data....
		readImage(offset, size, [buffer,size](AtClient& atClient, Stream& source) -> bool {
			source.readBytes(buffer, size);
			return true;
		});

		return size;
	}

private:
	AtClient* atClient = nullptr;
	size_t pictureSize = 0;
	eResolution resolution = eResVGA;

private:
	bool processSize(AtClient& atClient, String& reply)
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

	bool readImage(int offset, size_t length, AtReceiveCallback onReceive)
	{
		AtCommand command;

		// AT+CAMRD=fromaddr,toaddr
		char text[30] = {0};
		sprintf(text, "AT+CAMRD=%d:%d\r", offset, offset + length);

		command.text = text;
		command.breakOnError = false;
		command.onReceive = onReceive;
		atClient->send(command);

		return true;
	}
};
