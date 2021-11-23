/*
 * jsvm.h
 *
 *  Created on: Feb 11, 2016
 *      Author: slavey
 */

#include "jsvm.h"
#include "jsvm-ext.h"

#include <FileSystem.h>

Jsvm::Jsvm(jerry_init_flag_t flags /* =JERRY_INIT_EMPTY */)
{
	jerry_init(flags);

	registerFunction("print", printFunction);
#ifdef DEBUG
	registerFunction("alert", alertFunction);
#endif
}

bool Jsvm::eval(String jsCode)
{
	jerry_value_t ret_code = jerry_eval((const jerry_char_t*)jsCode.c_str(), jsCode.length(), false);
	if(jerry_value_is_error(ret_code)) {
		jerry_release_value(ret_code);
		return false;
	}

	jerry_release_value(ret_code);

	return true;
}

int Jsvm::exec(const char* fileName)
{
	file_t file = fileOpen(fileName, File::ReadOnly);
	if(file < 0) {
		return file;
	}

	// Get size
	fileSeek(file, 0, SeekOrigin::End);
	int size = fileTell(file);
	if(size <= 0) {
		fileClose(file);
		debug_e("File has zero size");
		return -1;
	}

	fileSeek(file, 0, SeekOrigin::Start);

	uint8_t* snapshot = new uint8_t[size];
	fileRead(file, snapshot, size);
	fileClose(file);

	bool success = exec(snapshot, size);
	delete[] snapshot;
	if(success) {
		return 0;
	}

	debug_e("Unable to execute the snapshot");
	return -2;
}

bool Jsvm::exec(uint8_t* snapshot, size_t size)
{
	jerry_value_t ret_code = jerry_exec_snapshot((const uint32_t*)snapshot, size, 0, JERRY_SNAPSHOT_EXEC_COPY_DATA);
	if(jerry_value_is_error(ret_code)) {
		jerry_release_value(ret_code);
		return false;
	}

	jerry_release_value(ret_code);

	return true;
}

bool Jsvm::runFunction(String functionName)
{
	bool ret_code = true;

	jerry_value_t global_obj_val = jerry_get_global_object();
	jerry_value_t prop_name_val = jerry_create_string((const jerry_char_t*)functionName.c_str());
	jerry_value_t jsFunction = jerry_get_property(global_obj_val, prop_name_val);
	jerry_release_value(prop_name_val);

	if(jerry_value_is_error(jsFunction)) {
		debug_e("Error: '%s' not defined!", functionName.c_str());
		jerry_release_value(jsFunction);
		jerry_release_value(global_obj_val);
		return false;
	}

	if(!jerry_value_is_function(jsFunction)) {
		debug_e("Error: '%s' is not a function!", functionName.c_str());
		jerry_release_value(jsFunction);
		jerry_release_value(global_obj_val);
		return false;
	}

	jerry_value_t res = jerry_call_function(jsFunction, global_obj_val, NULL, 0);
	if(jerry_value_is_error(res)) {
		ret_code = false;
	}

	jerry_release_value(res);
	jerry_release_value(jsFunction);
	jerry_release_value(global_obj_val);

	return ret_code;
}

bool Jsvm::runLoop()
{
	return runFunction("loop");
}

int Jsvm::registerFunction(const char* name_p, jerry_external_handler_t handler)
{
	jerry_value_t global_obj_val = jerry_get_global_object();
	jerry_value_t function_name_val = jerry_create_string((const jerry_char_t*)name_p);
	jerry_value_t function_val = jerry_create_external_function(handler);

	jerry_value_t result_val = jerry_set_property(global_obj_val, function_name_val, function_val);

	jerry_release_value(function_val);
	jerry_release_value(function_name_val);
	jerry_release_value(global_obj_val);

	return result_val;
}

Jsvm::~Jsvm()
{
	jerry_cleanup();
}
