/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * pins_arduino.h
 *
 ****/

// File name selected for compatibility

#pragma once

#include "peripheral.h"

#define EXTERNAL_NUM_INTERRUPTS 16
#define NUM_DIGITAL_PINS 40
#define NUM_ANALOG_INPUTS 16

#define analogInputToDigitalPin(p) (((p) < 20) ? (esp32_adc2gpio[(p)]) : -1)
#define digitalPinToInterrupt(p) (((p) < 40) ? (p) : -1)
#define digitalPinHasPWM(p) (p < 34)

constexpr uint8_t A0{36};

#define GPIO_REG_TYPE uint32_t

#define digitalPinToTimer(pin)      (0)
#define analogInPinToBit(P)         (P)
#if SOC_GPIO_PIN_COUNT <= 32
#define digitalPinToPort(pin)       (0)
#define digitalPinToBitMask(pin)    (1UL << (pin))
#define portOutputRegister(port)    ((volatile uint32_t*)GPIO_OUT_REG)
#define portInputRegister(port)     ((volatile uint32_t*)GPIO_IN_REG)
#define portModeRegister(port)      ((volatile uint32_t*)GPIO_ENABLE_REG)
#elif SOC_GPIO_PIN_COUNT <= 64
#define digitalPinToPort(pin)       (((pin)>31)?1:0)
#define digitalPinToBitMask(pin)    (1UL << (((pin)>31)?((pin)-32):(pin)))
#define portOutputRegister(port)    ((volatile uint32_t*)((port)?GPIO_OUT1_REG:GPIO_OUT_REG))
#define portInputRegister(port)     ((volatile uint32_t*)((port)?GPIO_IN1_REG:GPIO_IN_REG))
#define portModeRegister(port)      ((volatile uint32_t*)((port)?GPIO_ENABLE1_REG:GPIO_ENABLE_REG))
#else
#error SOC_GPIO_PIN_COUNT > 64 not implemented
#endif
