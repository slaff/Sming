#pragma once

#include "../Resource/HttpConditionalResource.h"
#include <Data/WebHelpers/base64.h>

class HttpAuthBasic
{
public:
	HttpAuthBasic(const String& realm, const String& username, const String& password): realm(realm), username(username), password(password)
    {
	}

	int operator()(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
	{
		auto& headers = request.headers;
		auto authorization = headers[HTTP_HEADER_AUTHORIZATION];
		if(authorization) {
			// check the authorization
			authorization.trim();
			auto pos = authorization.indexOf(' ');
			if(pos < 0) {
				// Invalid authorization header
				return -1;
			}

			auto type = authorization.substring(0, pos);
			auto token = authorization.substring(pos + 1, authorization.length());
			if(!type.equalsIgnoreCase(F("Basic"))) {
				return -1;
			}

			String text = base64_decode(token.c_str(), token.length());
			pos = text.indexOf(':');
			if(pos > 0) {
				auto providedUsername = text.substring(0, pos);
				auto providedPassword = text.substring(pos + 1, text.length());
				if(providedUsername == username && providedPassword == password) {
					return 0;
				}
			}
		}

		// specify that the resource is protected...
		response.code = HTTP_STATUS_UNAUTHORIZED;
		response.headers[HTTP_HEADER_WWW_AUTHENTICATE] = F("Basic realm=\"") + realm + "\"";
		return HttpConditionalResource::SKIP_REQUEST;
	}

private:
	String realm;
	String username;
	String password;
};
