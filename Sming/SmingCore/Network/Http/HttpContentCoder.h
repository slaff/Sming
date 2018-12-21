/*
 * HttpContentDecoder.h
 *
 *  Created on: Dec 12, 2018
 *      Author: slavey
 */

#ifndef SMINGCORE_NETWORK_HTTP_HTTPCONTENTCODER_H_
#define SMINGCORE_NETWORK_HTTP_HTTPCONTENTCODER_H_

#include "HttpCommon.h"
#include "HttpRequest.h"

#include <functional>

typedef std::function<int(uint8_t* destination, size_t* destinationLength, const uint8_t* source, int sourceLength,
						  void** context)>
	HttpContentCoder;
typedef HashMap<String, HttpContentCoder> ContentCoders;

#define CONTENT_CODER_START -1
#define CONTENT_CODER_END -2

/**
 * Decoder implementing the brotli algorithm from Google.
 * It promises very good compression ration and quite fast decompression
 */
int brotliDecoder(uint8_t* destination, size_t* destinationLength, const uint8_t* source, int sourceLength,
				  void** context);

#endif /* SMINGCORE_NETWORK_HTTP_HTTPCONTENTCODER_H_ */
