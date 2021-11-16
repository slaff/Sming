#pragma once

#include <stdint.h>
#include <driver/gpio.h>
#include_next <esp_sleep.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sleep_type {
	NONE_SLEEP_T = 0,
	LIGHT_SLEEP_T,
	MODEM_SLEEP_T,
};

bool system_deep_sleep(uint32_t time_in_us);
bool system_deep_sleep_set_option(uint8_t option);

bool system_deep_sleep_enable_wakeup(uint8_t pins[], size_t pinCount, uint8_t mode);
int system_deep_sleep_wakeup_reason();

/* These aren't defined in the RTOS SDK */

enum sleep_level {
	MIN_SLEEP_T,
	MAX_SLEEP_T,
};

bool wifi_set_sleep_type(enum sleep_type type);
enum sleep_type wifi_get_sleep_type(void);
bool wifi_set_sleep_level(enum sleep_level level);
enum sleep_level wifi_get_sleep_level(void);
bool wifi_set_listen_interval(uint8_t interval);
uint8_t wifi_get_listen_interval(void);

bool esp_sleep_is_valid_wakeup_gpio(gpio_num_t gpio_num);

#ifdef __cplusplus
}
#endif
