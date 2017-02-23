#include <user_config.h>
#include <SmingCore/SmingCore.h>

HardwareSerial Serial1(UART1);

void echoCallback(Stream& stream, char arrivedChar, unsigned short availableCharsCount)
{
	char *buffer = new char[availableCharsCount];
	stream.readBytes(buffer, availableCharsCount);
	Serial1.write(buffer, availableCharsCount);
}

void camCfg()
{
	Serial.write("AT\r");
	debugf("Wrote AT ...");
	delay(200);

	Serial.write("AT+CAMSTART\r");
	debugf("Wrote AT+CAMSTART ...");
	delay(200);

	Serial.write("AT+CAMCFG=0,1\r");
	debugf("Wrote AT+CAMCFG=0,1 ...");
	delay(200);
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	/*
	 * @brief Toggle between use of GPIO13/GPIO15 or GPIO3/GPIO(1/2) as RX and TX
	 * @note UART0 uses pins GPIO1 (TX) and GPIO3 (RX). It may be swapped to GPIO15 (TX) and GPIO13 (RX) by calling .swap() after .begin. C
	 * @note Calling swap again maps UART0 back to GPIO1 and GPIO3.
	 */
	Serial.swap();
	Serial.setCallback(echoCallback);

	/**
	 * Serial1 uses UART1, TX pin is GPIO2.
	 * UART1 can not be used to receive data because normally
	 * it's RX pin is occupied for flash chip connection.
	 *
	 * If you have a spare serial to USB converter do the following to see the
	 * messages printed on UART1:
	 * - connect converter GND to esp8266 GND
	 * - connect converter RX to esp8266 GPIO2
	 */
	Serial1.begin(SERIAL_BAUD_RATE);
	Serial1.systemDebugOutput(true);

	debugf("Starting...");


	camCfg();
}
