/*
 * jsvm.h
 *
 *  Created on: Feb 11, 2016
 *      Author: slavey
 */

#pragma once

#include <WString.h>
#include <jerry-core/include/jerryscript.h>
#include <FileSystem.h>

/**
 * @brief JavaScriptVM for Sming
 */
class Jsvm
{
public:
	/**
	* @brief Initializes the JavaScript VM
	*/
	Jsvm(jerry_init_flag_t flags = JERRY_INIT_EMPTY);

	~Jsvm();

	/*
	 * @brief Parses the JavaScript code and prepares it for execution
	 * @retval bool true on success
	 */
	bool eval(const String& jsCode);

	/**
	 * @brief Loads a snapshot from file and executes it.
	 * @param fileName Path to snapshot file
	 * @retval bool true on success
	 */
	bool loadFromFile(const String& fileName)
	{
		return load(fileGetContent(fileName));
	}

	/**
	 * @brief Executes a snapshot
	 * @param snapshot Points to snapshot content
	 * @param snapshotSize Number of bytes in snapshot
	 * @retval bool true on success
	 */
	bool load(const uint32_t* snapshot, size_t snapshotSize);

	bool load(const String& snapshot)
	{
		return load(reinterpret_cast<const uint32_t*>(snapshot.c_str()), snapshot.length());
	}

	/**
	 * @brief Runs a specified JavaScript function
	 * @param functionName Name of function to run
	 * @retval bool true if the function exists and was called successfully
	 */
	bool runFunction(const String& functionName);

	/**
	 * @brief Runs the loop JavaScript function
	 * @retval bool true on success
	 */
	bool runLoop();

	int registerFunction(const char* name_p, jerry_external_handler_t handler);

	int registerFunction(const String& name, jerry_external_handler_t handler)
	{
		return registerFunction(name.c_str(), handler);
	}
};
