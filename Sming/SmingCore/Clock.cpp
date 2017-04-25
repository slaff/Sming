/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/anakod/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 ****/

#include "../SmingCore/Clock.h"
#include "../Wiring/WiringFrameworkIncludes.h"

unsigned long millis(void)
{
	return system_get_time() / 1000UL;
}

unsigned long micros(void)
{
	return system_get_time();
}

#ifdef ENABLE_COOP_DELAY
#include <setjmp.h>

static os_timer_t delayTimer;

static jmp_buf delayPoint;

static void delayEnd(void *arg)
{
	debugf("Delay End");
	os_timer_disarm(&delayTimer);
	longjmp(delayPoint, 1);
}

void delay(uint32_t ms)
{
	if(!ms) {
		return;
	}

	if(!setjmp(delayPoint)) {
		debugf("Delay start: %d ms", ms);
		os_timer_disarm(&delayTimer);
		os_timer_setfn(&delayTimer, (os_timer_func_t*) &delayEnd, 0);
		os_timer_arm(&delayTimer, ms, 0);
	}
	else {
		debugf("Delay Finish");
	}
}

#else
void delay(uint32_t time)
{
	os_delay_us(time * 1000);
}
#endif

void delayMicroseconds(uint32_t time)
{
	os_delay_us(time);
}
