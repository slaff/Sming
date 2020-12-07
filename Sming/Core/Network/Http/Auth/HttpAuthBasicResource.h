/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 *
 * @author: 2020 - Slavey Karadzhov <slaff@attachix.com>
 *
 ****/

#pragma once

#include "../HttpResource.h"
#include "../HttpResourceTree.h"

class HttpAuthBasicResource : public HttpResource
{
public:
	/**
	 * @brief
	 * @param resource once set the resource will be owned by this class and destroyed when this object is destroyed
	 * @param realm
	 * @param username
	 * @param password
	 */
	HttpAuthBasicResource(HttpResource* resource, const String& realm, const String& username, const String& password)
	{
		delegate = resource;
		this->realm = realm;
		this->username = username;
		this->password = password;

		onHeadersComplete = HttpResourceDelegate(&HttpAuthBasicResource::checkHeaders, this);
		onBody = HttpServerConnectionBodyDelegate(&HttpAuthBasicResource::forwardBody, this);
		onUpgrade = HttpServerConnectionUpgradeDelegate(&HttpAuthBasicResource::forwardUpgrade, this);
		onRequestComplete = HttpResourceDelegate(&HttpAuthBasicResource::forwardRequestComplete, this);
	}

	HttpAuthBasicResource(const HttpResourceDelegate& onRequestComplete, const String& realm, const String& username,
						  const String& password)
	{
		HttpResource* resource = new HttpResource;
		resource->onRequestComplete = onRequestComplete;
		HttpAuthBasicResource(resource, realm, username, password);
	}

	HttpAuthBasicResource(const HttpPathDelegate& callback, const String& realm, const String& username,
							  const String& password): HttpAuthBasicResource(new HttpCompatResource(callback), realm, username, password)
	{

	}

	~HttpAuthBasicResource()
	{
		delete delegate;
	}

private:
	HttpResource* delegate{nullptr}; // << once set the delegate is owned by this class
	String realm;
	String username;
	String password;

	int checkHeaders(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response);
	int forwardBody(HttpServerConnection& connection, HttpRequest& request, const char* at, int length);
	int forwardUpgrade(HttpServerConnection& connection, HttpRequest&, char* at, int length);
	int forwardRequestComplete(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response);
};
