/*
 * A6.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: slavey
 */

#ifndef LIBRARIES_A6_H_
#define LIBRARIES_A6_H_

#include "../SmingCore/HardwareSerial.h"
#include "../Wiring/FILO.h"

typedef struct {
	String name;
	String responseOk;
	String responseNOK;
	int timeout;
	int retries;
	// callback, if needed
} A6Command;

template<typename T, int rawSize>
class SimpleQueue: public FILO<T, rawSize> {
	virtual const T& operator[](unsigned int) const { }
	virtual T& operator[](unsigned int) { }
};

class A6 {

public:
	A6(HardwareSerial& stream);

	// Low Level Functions

	/**
	 * @brief Sends command to the A6 device
	 * @param command String
	 */
	void send(String name, String responseOK, String responseNOK, uint32_t timeoutMs = 1000, int retries = 0);
	void send(A6Command command);
	void sendDirect(A6Command command);

private:
	SimpleQueue<A6Command, 10> queue; // << Queue for the commands to be executed
	A6Command currentCommand; // << The current command
	HardwareSerial* stream; // << The main communication stream
	Timer commandTimer; // timer used for commands with timeout

	void processor(Stream &source, char arrivedChar, uint16_t availableCharsCount);

	/**
	 * @brief Timeout checker method
	 */
	void timeout();
};




#endif /* LIBRARIES_A6_H_ */
