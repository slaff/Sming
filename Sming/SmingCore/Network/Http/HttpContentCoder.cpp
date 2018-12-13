#include "HttpContentCoder.h"
#include "../third-party/miniz/miniz.h"

int deflateDecoder(uint8_t* destination, size_t* destinationLength, const uint8_t* source, size_t sourceLength)
{
	int status = uncompress((unsigned char *)destination, destinationLength, source, (mz_ulong)sourceLength);

	return (status == Z_OK);
}
