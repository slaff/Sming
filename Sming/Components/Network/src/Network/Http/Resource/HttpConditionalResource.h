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

#pragma once

#include "../HttpResource.h"
#include "../HttpResourceTree.h"

class HttpConditionalResource : public HttpResource
{
public:

	static constexpr int SKIP_REQUEST=-9999;

	/**
	 * @brief
	 * @param resource once set the resource will be owned by this class and destroyed when this object is destroyed
	 * @param realm
	 * @param username
	 * @param password
	 */
	HttpConditionalResource(HttpResource* resource, const HttpResourceDelegate& onHeaders): delegate(resource), check(onHeaders)
	{
		delegate = resource;

		onHeadersComplete = HttpResourceDelegate(&HttpConditionalResource::checkHeaders, this);
		onBody = HttpServerConnectionBodyDelegate(&HttpConditionalResource::forwardBody, this);
		onUpgrade = HttpServerConnectionUpgradeDelegate(&HttpConditionalResource::forwardUpgrade, this);
		onRequestComplete = HttpResourceDelegate(&HttpConditionalResource::forwardRequestComplete, this);
	}

	~HttpConditionalResource()
	{
		delete delegate;
	}

private:
	const HttpResource* delegate = nullptr; // << once set the delegate is owned by this class
	HttpResourceDelegate check; //

	int checkHeaders(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response);
	int forwardBody(HttpServerConnection& connection, HttpRequest& request, const char* at, int length);
	int forwardUpgrade(HttpServerConnection& connection, HttpRequest&, char* at, int length);
	int forwardRequestComplete(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response);
};
