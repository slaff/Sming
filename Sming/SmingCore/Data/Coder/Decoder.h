/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * @author Slavey Karadzhov <slaff@attachix.com>
 *
 ****/

#ifndef _SMING_CORE_DATA_CODER_DECODER_H_
#define _SMING_CORE_DATA_CODER_DECODER_H_

#include <user_config.h>
#include "WHashMap.h"
#include "WString.h"

#include <functional>

typedef std::function<int(uint8_t* destination, size_t* destinationLength, const uint8_t* source, int sourceLength,
						  void** context)>
	ContentCoder;
typedef HashMap<String, ContentCoder> ContentCoders;

#define CONTENT_CODER_START -1
#define CONTENT_CODER_END -2

/**
 * Decoder implementing the brotli algorithm from Google.
 * It promises very good compression ration and quite fast decompression
 */
int brotliDecoder(uint8_t* destination, size_t* destinationLength, const uint8_t* source, int sourceLength,
				  void** context);

#endif /* _SMING_CORE_DATA_CODER_DECODER_H_ */
