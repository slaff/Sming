/****`
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * HttpResourceTree.cpp
 *
 * @author: 2017 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#include "HttpResourceTree.h"

/* HttpResourceTree */

void HttpResourceTree::set(String path, const HttpPathDelegate& callback)
{
	if(path.length() > 1 && path.endsWith("/")) {
		path.remove(path.length() - 1);
	}
	debug_i("'%s' registered", path.c_str());

	set(path, new HttpCompatResource(callback));
}
