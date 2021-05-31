/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 *
 * @author: 2021 - Slavey Karadzhov <slaff@attachix.com>
 *
 ****/

#include "HttpConditionalResource.h"
#include "../HttpServerConnection.h"

#define CHECK_ALLOWED(request)                                                                                         \
	if(request.args != (void*)1) {                                                                                     \
		return 0;                                                                                                      \
	}

int HttpConditionalResource::checkHeaders(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
{
	request.args = check(connection, request, response) == SKIP_REQUEST ? (void*)0 : (void*)1;

	return 0;
}

int HttpConditionalResource::forwardBody(HttpServerConnection& connection, HttpRequest& request, const char* at,
									   int length)
{
	CHECK_ALLOWED(request);

	if(delegate->onBody) {
		return delegate->onBody(connection, request, at, length);
	}

	return 0;
}

int HttpConditionalResource::forwardUpgrade(HttpServerConnection& connection, HttpRequest& request, char* at, int length)
{
	CHECK_ALLOWED(request);

	if(delegate->onUpgrade) {
		return delegate->onUpgrade(connection, request, at, length);
	}

	return 0;
}

int HttpConditionalResource::forwardRequestComplete(HttpServerConnection& connection, HttpRequest& request,
												  HttpResponse& response)
{
	CHECK_ALLOWED(request);

	if(delegate->onRequestComplete) {
		return delegate->onRequestComplete(connection, request, response);
	}

	return 0;
}
