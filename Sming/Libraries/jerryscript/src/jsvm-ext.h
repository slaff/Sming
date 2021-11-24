/*
 * jsvm-ext.h
 *
 *  Created on: Feb 11, 2016
 *      Author: slavey
 */

#pragma once

#include <jerry-core/include/jerryscript.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * External functions that are available to JerryScript
 */

/**
 * brief Function that prints data in the debug console
 */
jerry_value_t alertFunction(const jerry_value_t function_obj, const jerry_value_t this_val,
							const jerry_value_t args_p[], const jerry_length_t args_cnt);

/**
 * brief Function that prints data in the standard console
 */
jerry_value_t printFunction(const jerry_value_t function_obj, const jerry_value_t this_val,
							const jerry_value_t args_p[], const jerry_length_t args_cnt);

#ifdef __cplusplus
}
#endif
