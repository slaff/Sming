/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * ArduinoJson.h
 *
 ****/

#pragma once

#include <Libraries/ArduinoJson/ArduinoJson.h>
#include <Data/FlashStringRefAdapter.hpp>
#include "Data/Stream/FileStream.h"

namespace Json
{
/**
 * @brief Helper function to copy a Json data value, provided it's valid
 * @param data Value returned from JsonObject[key], JsonDocument[key] or JsonVariant[key]
 * @param value Where to write value, unchanged if data is null
 * @retval bool true if value was read, false if not
 * @note Example:
 *
 *		JsonDocument doc;
 *		...
 *		String value1;
 *		if(!Json::getValue(doc["key1"], value1)) {
 *			// Perform required action when value1 doesn't exist, such as initialising defaults
 *		}
 *
 */
template <typename TValue> bool getValue(const JsonVariantConst& data, TValue& value)
{
	if(data.isNull()) {
		return false;
	} else {
		value = data;
		return true;
	}
}

/**
 * @brief Save a JsonDocument to a file in JSON format
 * @param source JsonDocument (or JsonArray, JsonObject, JsonVariant)
 * @param filename Name of file to create, will always be overwritten
 * @param pretty If true, output will contain spaces and line breaks
 * @retval bool true on success, false if the write failed
 */
template <typename TSource> bool saveToFile(const TSource& source, const String& filename, bool pretty = false)
{
	FileStream stream(filename, eFO_WriteOnly | eFO_CreateNewAlways);
	if(!stream.isValid()) {
		return false;
	}

	if(pretty) {
		serializeJson(source, stream);
	} else {
		serializeJsonPretty(source, stream);
	}

	return stream.getLastError() == 0;
}

/**
 * @brief Parses the contents of a JSON file into a JsonDocument object
 * @param doc Document to store the decoded file
 * @param filename Name of file to create, will always be overwritten
 * @retval bool true on success, false if the file couldn't be read or there was a parsing error
 */
inline bool loadFromFile(JsonDocument& doc, const String& filename)
{
	FileStream stream(filename);
	if(stream.isValid()) {
		return deserializeJson(doc, stream) == DeserializationError::Ok;
	} else {
		return false;
	}
}

}; // namespace Json

namespace MsgPack
{
/**
 * @brief Save a JsonDocument to a file in MessagePack format
 * @param source JsonDocument (or JsonArray, JsonObject, JsonVariant)
 * @param filename Name of file to create, will always be overwritten
 * @retval bool true on success, false if the write failed
 */
template <typename TSource> bool saveToFile(const TSource& source, const String& filename)
{
	FileStream stream(filename, eFO_WriteOnly | eFO_CreateNewAlways);
	if(!stream.isValid()) {
		return false;
	}

	serializeMsgPack(source, stream);
	return stream.getLastError() == 0;
}

/**
 * @brief Parses the contents of a MessagePack file into a JsonDocument object
 * @param doc Document to store the decoded file
 * @param filename Name of file to create, will always be overwritten
 * @retval bool true on success, false if the file couldn't be read or there was a parsing error
 */
inline bool loadFromFile(JsonDocument& doc, const String& filename)
{
	FileStream stream(filename);
	if(stream.isValid()) {
		return deserializeMsgPack(doc, stream) == DeserializationError::Ok;
	} else {
		return false;
	}
}

}; // namespace MsgPack
