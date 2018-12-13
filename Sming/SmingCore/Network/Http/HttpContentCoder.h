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

typedef std::function<int(uint8_t* destination, size_t* destinationLength, const uint8_t* source, size_t sourceLength)> HttpContentCoder;
typedef HashMap<String, HttpContentCoder> ContentCoders;

int deflateDecoder(uint8_t* destination, size_t* destinationLength, const uint8_t* source, size_t sourceLength);

#endif /* SMINGCORE_NETWORK_HTTP_HTTPCONTENTCODER_H_ */
