#ifndef _SMING_CORE_PRODUCERCONSUMER_H_
#define _SMING_CORE_PRODUCERCONSUMER_H_

#include <user_config.h>
#include "../Wiring/WString.h"
#include "../Wiring/WHashMap.h"
#include "DataSourceStream.h"

class ProducerBuffer {
public:
	/**
	 * Meta-data that can be passed from the producer
	 * to the consumers.
	 */
	void *meta;

	ProducerBuffer(int length=512) {
		this->length = length;
		buffer = (uint8_t *)malloc(length);
	}
	~ProducerBuffer() {
		if(buffer) {
			free(buffer);
		}
	}

	int available(int rpos) {
		if(rpos < 0 || rpos >= length) {
			rpos = 0;
		}

		return ( rpos >  wpos ?  length - (rpos -  wpos) : wpos - rpos );
	}

	int peek(int rpos = -1) {
		if(rpos==-1) {
			rpos =  wpos;
		}
		else if(++rpos >  length) {
			rpos = 0;
		}

		return buffer[rpos];
	}

	int write(uint8_t ch) {
		buffer[wpos] = ch;
		if(wpos++ == length) {
			wpos = 0;
		}
		return 1;
	}

	int size() {
		return length;
	}

	int resize(int newSize) {
		realloc(buffer, newSize);
	}
private:
	uint8_t *buffer;
	int wpos = 0;
	int length;
};

class Consumer: public Stream, public IDataSourceStream {
public:
	Consumer(ProducerBuffer*  buffer) {
		this->buffer = buffer;
	}

	int available() {
		return buffer->available(rpos);
	}

	int read() {
		int ch = buffer->peek(rpos);
		rpos++;
		return ch;
	}

	int peek() {
		return buffer->peek(rpos);
	}

	void flush() {

	}

	size_t write(uint8_t ch) {
		// writing to the consumer is not recommended.
		return 0;
	}


	StreamType getStreamType() {
		return eSST_Memory;
	}

	uint16_t readMemoryBlock(char* data, int bufSize) {
		return readBytes(data, bufSize);
	}

	bool seek(int len) {
		if(len < 0) {
			return false;
		}

		rpos = len % buffer->size();

		return true;
	}

	bool isFinished() {
		return false;
	}


private:
	// The current read position
	int rpos = -1;
	ProducerBuffer*  buffer;
};

class Producer: public Stream {
public:

	Producer(int bufferSize = 0) {
		if(bufferSize) {
			buffer = new ProducerBuffer(bufferSize);
		}
		else {
			buffer = new ProducerBuffer();
		}
	}

	~Producer() {
		if(buffer) {
			delete buffer;
		}
	}

	int available() {
		return buffer->available(0);
	}

	int peek() {
		return buffer->peek();
	}

	void flush() {

	}


	int read() {
		return -1;
	}

	size_t write(uint8_t ch) {
		buffer->write(ch);
		return 1;
	}

	Consumer *getConsumer() {
		Consumer *comsumer = new Consumer(buffer);
		return comsumer;
	}
private:
	ProducerBuffer *buffer = nullptr;
};



#endif /* _SMING_CORE_PRODUCERCONSUMER_H_ */
