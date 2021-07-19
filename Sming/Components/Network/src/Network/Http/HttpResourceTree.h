/****`
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * HttpResourceTree.h
 *
 * @author: 2017 - Slavey Karadzhov <slav@attachix.com>
 *
 ****/

#pragma once

#include "HttpResource.h"
#include "Resource/HttpEventedResource.h"
#include "Resource/HttpResourcePlugin.h"

using HttpPathDelegate = Delegate<void(HttpRequest& request, HttpResponse& response)>;

/** @brief Identifies the default resource path */
#define RESOURCE_PATH_DEFAULT String('*')

/**
 * @brief Class to map URL paths to classes which handle them
 * @ingroup http
 */
class HttpResourceTree : public ObjectMap<String, HttpResource>
{
public:
	/** @brief Set the default resource handler
	 *  @param resource The default resource handler
	 */
	void setDefault(HttpResource* resource)
	{
		set(RESOURCE_PATH_DEFAULT, resource);
	}

	/** @brief Set the default resource handler, identified by "*" wildcard
	 *  @param onRequestComplete The default resource handler
	 */
	void setDefault(const HttpResourceDelegate& onRequestComplete)
	{
		set(RESOURCE_PATH_DEFAULT, onRequestComplete);
	}

	/** @brief Set the default resource handler, identified by "*" wildcard */
	void setDefault(const HttpPathDelegate& callback)
	{
		set(RESOURCE_PATH_DEFAULT, callback);
	}

	/** @brief Get the current default resource handler, if any
	 *  @retval HttpResource*
	 */
	HttpResource* getDefault()
	{
		return find(RESOURCE_PATH_DEFAULT);
	}

	using ObjectMap::set;

	/**
	 * @brief Set a callback to handle the given path
	 * @param path URL path
	 * @param onRequestComplete Delegate to handle this path
	 * @note Path should start with slash. Trailing slashes will be removed.
	 * @note Any existing handler for this path is replaced
	 */
	void set(const String& path, const HttpResourceDelegate& onRequestComplete)
	{
		HttpResource* resource = new HttpResource;
		resource->onRequestComplete = onRequestComplete;
		set(path, resource);
	}

	void set(const String& path, const HttpResourceDelegate& onRequestComplete, HttpResourcePlugin plugin)
	{
		HttpResource* resource = get(path);
		if(resource == nullptr) {
			HttpResource* resource = new HttpResource;
			resource->onRequestComplete = onRequestComplete;

			auto eventedResource = new HttpEventedResource(resource);
			plugin(*eventedResource);
			set(path, resource);

			return;
		}


		if(resource->getType() == HttpResource::EVENTED_RESOURCE) {
			plugin(*(static_cast<HttpEventedResource*>(resource)));
		}
	}


	template <class H, class... Tail>
	void set(const String& path, const HttpResourceDelegate& onRequestComplete, H plugin, Tail... plugins)
	{
		set(path, onRequestComplete, plugin);
		set(path, onRequestComplete, plugins...);
	}

	/**
	 * @brief Add a new path resource with a callback
	 * @param path URL path
	 * @param callback The callback that will handle this path
	 * @note Path should start with slash. Trailing slashes will be removed
	 * @note Any existing handler for this path is replaced
	 */
	void set(String path, const HttpPathDelegate& callback);
};
