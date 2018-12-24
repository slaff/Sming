#include "Decoder.h"

#include "../third-party/brotli/include/brotli/decode.h"
#include "../third-party/brotli/dec/state.h"

int brotliDecoder(uint8_t* destination, size_t* destinationLength, const uint8_t* source, int sourceLength,
				  void** context)
{
	BrotliDecoderState* state = (BrotliDecoderState*)*context;
	if(sourceLength == CONTENT_CODER_START) {
		state = new BrotliDecoderState();
		if(!BrotliDecoderStateInit(state, 0, 0, 0)) {
			return -1;
		}

		*context = (void*)state;
		return 0;
	}

	if(state == nullptr) {
		return -1;
	}

	if(sourceLength == CONTENT_CODER_END) {
		BrotliDecoderStateCleanup(state);
		delete state;
		return 0;
	}

	size_t totalOut = 0;
	size_t availableIn = sourceLength;
	const uint8_t* nextIn = source;
	size_t availableOut = 0;
	uint8_t* nextOut = NULL;

	/* Invariant: input stream is never overconsumed:
	    - invalid input implies that the whole stream is invalid -> any amount of
	      input could be read and discarded
	    - when result is "needs more input", then at least one more byte is REQUIRED
	      to complete decoding; all input data MUST be consumed by decoder, so
	      client could swap the input buffer
	    - when result is "needs more output" decoder MUST ensure that it doesn't
	      hold more than 7 bits in bit reader; this saves client from swapping input
	      buffer ahead of time
	    - when result is "success" decoder MUST return all unused data back to input
	      buffer; this is possible because the invariant is held on enter
	*/
	BrotliDecoderResult result = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;
	while(result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
		result = BrotliDecoderDecompressStream(state, &availableIn, &nextIn, &availableOut, &nextOut, NULL);

		size_t bufferLength = 0; // Request all available output.
		const uint8_t* buffer = BrotliDecoderTakeOutput(state, &bufferLength);
		if(bufferLength) {
			// TODO: check that we have enough space in the destination buffer!!!!
			assert(bufferLength <= (*destinationLength - totalOut));
			memcpy(destination, buffer, bufferLength);
			destination += bufferLength;
			totalOut += bufferLength;
		}
	}

	bool success = (result != BROTLI_DECODER_RESULT_ERROR && !availableIn);
	if(success) {
		*destinationLength = totalOut;
	}

	return (success ? 0 : (int)result);
}
