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
#include "Resource/HttpConditionalResource.h"

/* HttpResourceTree */

void HttpResourceTree::set(String path, const HttpPathDelegate& callback, const HttpResourceDelegate& onHeadersComplete)
{
	if(path.length() > 1 && path.endsWith("/")) {
		path.remove(path.length() - 1);
	}
	debug_i("'%s' registered", path.c_str());

	HttpResource* resource= new HttpCompatResource(callback);
	if(onHeadersComplete && resource != nullptr) {
		resource = new HttpConditionalResource(resource, onHeadersComplete);
	}
	set(path, resource);
}

void HttpResourceTree::set(const String& path, const HttpResourceDelegate& onRequestComplete, const HttpResourceDelegate& onHeadersComplete)
{
	HttpResource* resource = new HttpResource;
	resource->onRequestComplete = onRequestComplete;
	if(onHeadersComplete) {
		resource = new HttpConditionalResource(resource, onHeadersComplete);
	}
	set(path, resource);
}
