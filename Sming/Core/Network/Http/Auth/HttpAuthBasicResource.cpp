#include "HttpAuthBasicResource.h"

#include <Network/WebHelpers/base64.h>

#define CHECK_ALLOWED(request)                                                                                         \
	if(request.args != (void*)1) {                                                                                     \
		return 0;                                                                                                      \
	}

int HttpAuthBasicResource::checkHeaders(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
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
				request.args = (void*)1;
				if(delegate->onHeadersComplete) {
					return delegate->onHeadersComplete(connection, request, response);
				}

				return 0;
			}
		}
	}

	// specify that the resource is protected...
	response.code = HTTP_STATUS_UNAUTHORIZED;
	response.headers[HTTP_HEADER_WWW_AUTHENTICATE] = "Basic realm=\"" + realm + "\"";
	request.args = 0;
	return 0;
}

int HttpAuthBasicResource::forwardBody(HttpServerConnection& connection, HttpRequest& request, const char* at,
									   int length)
{
	CHECK_ALLOWED(request);

	if(delegate->onBody) {
		return delegate->onBody(connection, request, at, length);
	}

	return 0;
}

int HttpAuthBasicResource::forwardUpgrade(HttpServerConnection& connection, HttpRequest& request, char* at, int length)
{
	CHECK_ALLOWED(request);

	if(delegate->onUpgrade) {
		return delegate->onUpgrade(connection, request, at, length);
	}

	return 0;
}

int HttpAuthBasicResource::forwardRequestComplete(HttpServerConnection& connection, HttpRequest& request,
												  HttpResponse& response)
{
	CHECK_ALLOWED(request);

	if(delegate->onRequestComplete) {
		return delegate->onRequestComplete(connection, request, response);
	}

	return 0;
}
