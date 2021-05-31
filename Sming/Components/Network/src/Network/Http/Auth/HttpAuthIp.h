#pragma once

#include "../Resource/HttpConditionalResource.h"
#include <IpAddress.h>

class HttpAuthIp
{
public:
	HttpAuthIp(IpAddress ip, IpAddress netmask): ip(ip), netmask(netmask)
	{
	}

	int operator()(HttpServerConnection& connection, HttpRequest& request, HttpResponse& response)
	{
		auto remoteIp = connection.getRemoteIp();
		if(remoteIp.compare(ip, netmask)) {
			// This IP is allowed to proceed
			return 0;
		}

		// specify that the resource is protected...
		response.code = HTTP_STATUS_UNAUTHORIZED;
		return HttpConditionalResource::SKIP_REQUEST;
	}

private:
	IpAddress ip;
	IpAddress netmask;
};
