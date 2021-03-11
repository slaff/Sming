/*
  Based off of ArduinoCore-SAMD I2S interface.  Modified for the
  ESP8266 by Earle F. Philhower, III <earlephilhower@yahoo.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "I2SClass.h"

I2SClass::I2SClass(bool enableTransmit, bool enableRecv, bool driveClocks)
{
	enableTx = enableTransmit;
	enableRx = enableRecv;
	driveClk = driveClocks;
	running = false;
	_onTransmit = nullptr;
	_onReceive = nullptr;
	havePeeked = 0;
	peekedData = 0;
	bps = 0;
	writtenData = 0;
	writtenHalf = false;
}

int I2SClass::begin(i2s_mode_t mode, long sampleRate, int bitsPerSample)
{
	if(running || (mode != I2S_MODE_MASTER) || ((bitsPerSample != 16) && (bitsPerSample != 24))) {
		return 0;
	}
	//	if(!i2s_rxtxdrive_begin(enableRx, enableTx, driveClk, driveClk)) {
	//		return 0;
	//	}
	i2s_set_rate(sampleRate);
	i2s_set_callback(_onTransmit);
	i2s_rx_set_callback(_onReceive);
	bps = bitsPerSample;
	running = true;
	return 1;
}

void I2SClass::end()
{
	if(running) {
		i2s_end();
	}
	i2s_set_callback(nullptr);
	i2s_rx_set_callback(nullptr);
	running = false;
}

void I2SClass::onTransmit(void (*fcn)(void))
{
	i2s_set_callback(fcn);
	_onTransmit = fcn;
}

void I2SClass::onReceive(void (*fcn)(void))
{
	i2s_rx_set_callback(fcn);
	_onReceive = fcn;
}

int I2SClass::available()
{
	if(!running)
		return 0;
	return i2s_rx_available();
}

int I2SClass::availableForWrite()
{
	if(!running)
		return 0;
	return i2s_available();
}

void I2SClass::flush()
{
	/* No-op */
}

int I2SClass::read()
{
	if(!running)
		return -1;
	// Always just read from the peeked value to simplify operation
	if(!havePeeked) {
		peek();
	}
	if(havePeeked) {
		if(bps == 16) {
			havePeeked--;
			int ret = peekedData;
			peekedData >>= 16;
			return ret;
		} else /* _bps == 24 */ {
			havePeeked = 0;
			return peekedData;
		}
	}
	return 0;
}

int I2SClass::peek()
{
	if(!running)
		return -1;
	if(havePeeked) {
		if(bps == 16) {
			int16_t sample = (int16_t)peekedData; // Will extends sign on return
			return sample;
		} else {
			return peekedData;
		}
	}
	int16_t l, r;
	i2s_read_sample(&l, &r, true);
	peekedData = ((int)l << 16) | (0xffff & (int)r);
	havePeeked = 2; // We now have 2 16-bit quantities which can also be used as 1 32-bit(24-bit)
	if(bps == 16) {
		return r;
	} else {
		return peekedData;
	}
}

int I2SClass::read(void* buffer, size_t size)
{
	if(!running)
		return -1;
	int cnt = 0;

	if(((bps == 24) && (size % 4)) || ((bps == 16) && (size % 2)) || (size < 2)) {
		return 0; // Invalid, can only read in units of samples
	}
	// Make sure any peeked data is consumed first
	if(havePeeked) {
		if(bps == 16) {
			while(havePeeked && size) {
				uint16_t* p = (uint16_t*)buffer;
				*(p++) = peekedData;
				peekedData >>= 16;
				havePeeked--;
				buffer = (void*)p;
				size -= 2;
				cnt += 2;
			}
		} else {
			uint32_t* p = (uint32_t*)buffer;
			*(p++) = peekedData;
			buffer = (void*)p;
			size -= 4;
			cnt += 4;
		}
	}
	// Now just non-blocking read up to the remaining size
	int16_t l, r;
	int16_t* p = (int16_t*)buffer;
	while(size && i2s_read_sample(&l, &r, false)) {
		*(p++) = l;
		size--;
		cnt++;
		if(size) {
			*(p++) = r;
			size--;
			cnt++;
		} else {
			// We read a simple we can't return, stuff it in the peeked data
			havePeeked = 1;
			peekedData = r;
		}
	}
	return cnt;
}

size_t I2SClass::write(uint8_t s)
{
	if(!running)
		return 0;
	return write((int32_t)s);
}

size_t I2SClass::write(const uint8_t* buffer, size_t size)
{
	return write((const void*)buffer, size);
}

size_t I2SClass::write(int32_t s)
{
	if(!running)
		return 0;
	// Because our HW really wants 32b writes, store any 16b writes until another
	// 16b write comes in and then send the combined write down.
	if(bps == 16) {
		if(writtenHalf) {
			writtenData <<= 16;
			writtenData |= 0xffff & s;
			writtenHalf = false;
			return i2s_write_sample(writtenData) ? 1 : 0;
		} else {
			writtenHalf = true;
			writtenData = s & 0xffff;
			return 1;
		}
	} else {
		return i2s_write_sample((uint32_t)s) ? 1 : 0;
	}
}

// SAMD core has this as non-blocking
size_t I2SClass::write(const void* buffer, size_t size)
{
	if(!running)
		return 0;
	return i2s_write_buffer_nb((int16_t*)buffer, size / 2);
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_I2S)
I2SClass I2S;
#endif
