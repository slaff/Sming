#include <esp_sleep.h>
#include <rom/gpio.h>

bool system_deep_sleep(uint32_t time_in_us)
{
	esp_deep_sleep(time_in_us);
	return true;
}

bool system_deep_sleep_set_option(uint8_t option)
{
	(void)option; // Ignore
	return true;
}

bool system_deep_sleep_enable_wakeup(uint8_t pins[], size_t pinCount, uint8_t mode)
{
	for(unsigned i = 0; i < pinCount; i++) {
		uint64_t pin = pins[i];
		if(pin < 32) {
			pin = BIT(pin);
		} else {
			pin = ((uint64_t)(((uint64_t)1) << pin));
		}

		int err = esp_sleep_enable_ext1_wakeup(pin, mode);
		if(err != ESP_OK) {
			return false;
		}
	}

	return true;
}

int system_deep_sleep_wakeup_reason()
{
	return esp_sleep_get_wakeup_cause();
}

/* GPIO */

void wifi_enable_gpio_wakeup(uint32_t i, GPIO_INT_TYPE intr_status)
{
}

void wifi_disable_gpio_wakeup(void)
{
}

/* These aren't defined in the RTOS SDK */

bool wifi_set_sleep_type(enum sleep_type type)
{
	return type == NONE_SLEEP_T;
}

enum sleep_type wifi_get_sleep_type(void)
{
	return NONE_SLEEP_T;
}

bool wifi_set_sleep_level(enum sleep_level level)
{
	return false;
}

enum sleep_level wifi_get_sleep_level(void)
{
	return MIN_SLEEP_T;
}

bool wifi_set_listen_interval(uint8_t interval)
{
	return false;
}

uint8_t wifi_get_listen_interval(void)
{
	return 0;
}
