/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/anakod/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * JsonObjectStream.h
 *
 ****/

#pragma once

#include "MemoryDataStream.h"
#include "Libraries/ArduinoJson/ArduinoJson.h"

/** @brief JsonObject stream class
 * 	@ingroup    stream data
 *  @{
 *
 */

class JsonObjectStream : public MemoryDataStream
{
public:
	/** @brief  Create a JSON object stream
    */
	JsonObjectStream() : doc(1024)
	{
	}

	//Use base class documentation
	StreamType getStreamType() const override
	{
		return eSST_JsonObject;
	}

	/** @brief  Get the JSON root node
     *  @retval JsonObject Reference to the root node
     */
	JsonObject getRoot()
	{
		return doc.as<JsonObject>();
	}

	//Use base class documentation
	uint16_t readMemoryBlock(char* data, int bufSize) override;

	/**
	 * @brief Return the total length of the stream
	 * @retval int -1 is returned when the size cannot be determined
	 */
	int available() override
	{
		return (doc.isNull() ? 0 : measureJson(doc));
	}

private:
	DynamicJsonDocument doc;
	bool send = true;
};

/** @} */
