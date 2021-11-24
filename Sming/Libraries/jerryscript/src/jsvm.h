/*
 * jsvm.h
 *
 *  Created on: Feb 11, 2016
 *      Author: slavey
 */

#pragma once

#include <WString.h>
#include <jerry-core/include/jerryscript.h>

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
	 */
	bool eval(const String& jsCode);

	/**
	 * @brief Loads a snapshot from file and executes it.
	 * @return 0 on success
	 * 		   negative on error
	 */
	int exec(const String& fileName);

	/**
	 * @brief Executes a snapshot file
	 */
	bool exec(const uint32_t* snapshot, size_t snapshot_size);

	/**
	 * @brief Runs a specified JavaScript function
	 *
	 * @return true if the function exists and was called successfully
	 */
	bool runFunction(const String& functionName);

	/**
	 * @brief Runs the loop JavaScript function
	 */
	bool runLoop();

	int registerFunction(const char* name_p, jerry_external_handler_t handler);

	int registerFunction(const String& name, jerry_external_handler_t handler)
	{
		return registerFunction(name.c_str(), handler);
	}
};
