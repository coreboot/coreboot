/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __AMDBLOCK_GPIO_BANKS_H__
#define __AMDBLOCK_GPIO_BANKS_H__

#include <stdint.h>
#include <stddef.h>
#include <amdblocks/acpimmio_map.h>

struct soc_amd_gpio {
	uint8_t gpio;
	uint8_t function;
	uint32_t control;
	uint32_t flags;
};

struct soc_amd_event {
	uint8_t gpio;
	uint8_t event;
};

#define GPIO_BANK0_CONTROL(gpio) (ACPIMMIO_GPIO0_BASE + ((gpio) * 4))
#define GPIO_BANK1_CONTROL(gpio) (ACPIMMIO_GPIO1_BASE + (((gpio) - 64) * 4))
#define GPIO_BANK2_CONTROL(gpio) (ACPIMMIO_GPIO2_BASE + (((gpio) - 128) * 4))

#define GPIO_MASTER_SWITCH	0xFC
#define   GPIO_MASK_STS_EN	BIT(28)
#define   GPIO_INTERRUPT_EN	BIT(30)

#define GPIO_PIN_IN		(1 << 0)	/* for byte access */
#define GPIO_PIN_OUT		(1 << 6)	/* for byte access */

#define GPIO_EDGE_TRIG		(0 << 8)
#define GPIO_LEVEL_TRIG		(1 << 8)
#define GPIO_TRIGGER_MASK	(1 << 8)

#define GPIO_ACTIVE_HIGH	(0 << 9)
#define GPIO_ACTIVE_LOW		(1 << 9)
#define GPIO_ACTIVE_BOTH	(2 << 9)
#define GPIO_ACTIVE_MASK	(3 << 9)

#define GPIO_INT_STATUS_EN	(1 << 11)
#define GPIO_INT_DELIVERY_EN	(1 << 12)
#define GPIO_INTERRUPT_MASK	(3 << 11)
#define GPIO_S0I3_WAKE_EN	(1 << 13)
#define GPIO_S3_WAKE_EN		(1 << 14)
#define GPIO_S4_S5_WAKE_EN	(1 << 15)

#define GPIO_PIN_STS		(1 << 16)
#define GPIO_8KPULLUP_SELECT	(1 << 19)
#define GPIO_PULLUP_ENABLE	(1 << 20)
#define GPIO_PULLDOWN_ENABLE	(1 << 21)
#define GPIO_OUTPUT_SHIFT	22
#define GPIO_OUTPUT_MASK	(1 << GPIO_OUTPUT_SHIFT)
#define GPIO_OUTPUT_VALUE	(1 << GPIO_OUTPUT_SHIFT)
#define GPIO_OUTPUT_ENABLE	(1 << 23)

#define GPIO_INT_STATUS		(1 << 28)
#define GPIO_WAKE_STATUS	(1 << 29)

enum {
	GEVENT_0,
	GEVENT_1,
	GEVENT_2,
	GEVENT_3,
	GEVENT_4,
	GEVENT_5,
	GEVENT_6,
	GEVENT_7,
	GEVENT_8,
	GEVENT_9,
	GEVENT_10,
	GEVENT_11,
	GEVENT_12,
	GEVENT_13,
	GEVENT_14,
	GEVENT_15,
	GEVENT_16,
	GEVENT_17,
	GEVENT_18,
	GEVENT_19,
	GEVENT_20,
	GEVENT_21,
	GEVENT_22,
	GEVENT_23,
	GEVENT_24,
	GEVENT_25,
	GEVENT_26,
	GEVENT_27,
	GEVENT_28,
	GEVENT_29,
	GEVENT_30,
	GEVENT_31,
};

#define GPIO_OUTPUT_OUT_HIGH (GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE)
#define GPIO_OUTPUT_OUT_LOW GPIO_OUTPUT_ENABLE

#define GPIO_PULL_PULL_UP_8K (GPIO_PULLUP_ENABLE | GPIO_8KPULLUP_SELECT)
#define GPIO_PULL_PULL_UP GPIO_PULLUP_ENABLE
#define GPIO_PULL_PULL_DOWN GPIO_PULLDOWN_ENABLE
#define GPIO_PULL_PULL_NONE 0

#define AMD_GPIO_CONTROL_MASK			0x00f4ff00
#define AMD_GPIO_MUX_MASK			0x03

/* Definitions for PAD_INT. */
#define GPIO_INT_EDGE_HIGH		(GPIO_ACTIVE_HIGH | GPIO_EDGE_TRIG)
#define GPIO_INT_EDGE_LOW		(GPIO_ACTIVE_LOW | GPIO_EDGE_TRIG)
#define GPIO_INT_BOTH_EDGES		(GPIO_ACTIVE_BOTH | GPIO_EDGE_TRIG)
#define GPIO_INT_LEVEL_HIGH		(GPIO_ACTIVE_HIGH | GPIO_LEVEL_TRIG)
#define GPIO_INT_LEVEL_LOW		(GPIO_ACTIVE_LOW | GPIO_LEVEL_TRIG)

enum {
	GPIO_TRIGGER_LEVEL_LOW,
	GPIO_TRIGGER_LEVEL_HIGH,
	GPIO_TRIGGER_EDGE_LOW,
	GPIO_TRIGGER_EDGE_HIGH,
};

#define GPIO_TRIGGER_INVALID		-1
#define SCI_TRIGGER_EDGE		0
#define SCI_TRIGGER_LEVEL		1

#define GPIO_SPECIAL_FLAG		(1 << 31)
#define GPIO_DEBOUNCE_FLAG		(1 << 30)
#define GPIO_WAKE_FLAG			(1 << 29)
#define GPIO_INT_FLAG			(1 << 28)
#define GPIO_SMI_FLAG			(1 << 27)
#define GPIO_SCI_FLAG			(1 << 26)
#define GPIO_FLAG_DEBOUNCE		(GPIO_SPECIAL_FLAG | GPIO_DEBOUNCE_FLAG)
#define GPIO_FLAG_WAKE			(GPIO_SPECIAL_FLAG | GPIO_WAKE_FLAG)
#define GPIO_FLAG_INT			(GPIO_SPECIAL_FLAG | GPIO_INT_FLAG)
#define GPIO_FLAG_SCI			(GPIO_SPECIAL_FLAG | GPIO_SCI_FLAG)
#define GPIO_FLAG_SMI			(GPIO_SPECIAL_FLAG | GPIO_SMI_FLAG)

#define FLAGS_TRIGGER_MASK		0x00000003
#define GPIO_SPECIAL_MASK		0x7c000000
#define GPIO_DEBOUNCE_MASK		0x000000ff
#define INT_TRIGGER_MASK		0x00000700
#define INT_WAKE_MASK			0x0000e700
#define INT_SCI_SMI_MASK		0x00f40000

#define IN_GLITCH_SHIFT			5
#define GLITCH_LOW			1
#define GLITCH_HIGH			2
#define GLITCH_NONE			3
#define GPIO_IN_PRESERVE_LOW_GLITCH	(GLITCH_LOW << IN_GLITCH_SHIFT)
#define GPIO_IN_PRESERVE_HIGH_GLITCH	(GLITCH_HIGH << IN_GLITCH_SHIFT)
#define GPIO_IN_REMOVE_GLITCH		(GLITCH_NONE << IN_GLITCH_SHIFT)

#define GPIO_TIMEBASE_61uS		0
#define GPIO_TIMEBASE_183uS		(1 << 4)
#define GPIO_TIMEBASE_15560uS		(1 << 7)
#define GPIO_TIMEBASE_62440uS		(GPIO_TIMEBASE_183uS | \
					GPIO_TIMEBASE_15560uS)
#define GPIO_IN_DEBOUNCE_DISABLED	(0 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_60uS			(1 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_120uS			(2 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_200uS			(3 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_500uS			(8 | GPIO_TIMEBASE_61uS)
#define GPIO_IN_1mS			(5 | GPIO_TIMEBASE_183uS)
#define GPIO_IN_2mS			(11 | GPIO_TIMEBASE_183uS)
#define GPIO_IN_15mS			(1 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_50mS			(3 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_100mS			(6 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_200mS			(13 | GPIO_TIMEBASE_15560uS)
#define GPIO_IN_500mS			(8 | GPIO_TIMEBASE_62440uS)

#define GPIO_EVENT_INT_STATUS		GPIO_INT_STATUS_EN
#define GPIO_EVENT_INT_DELIVER		GPIO_INT_DELIVERY_EN
#define GPIO_EVENT_INT_STATUS_DELIVER	(GPIO_INT_STATUS_EN | \
					GPIO_INT_DELIVERY_EN)
#define GPIO_WAKE_S0i3			(1 << 13)
#define GPIO_WAKE_S3			(1 << 14)
#define GPIO_WAKE_S4_S5			(1 << 15)
#define GPIO_WAKE_S0i3_S4_S5		(GPIO_WAKE_S0i3 | GPIO_WAKE_S4_S5)
#define GPIO_WAKE_S3_S4_S5		(GPIO_WAKE_S3 | GPIO_WAKE_S4_S5)

/*
 * Several macros are available to declare programming of GPIO pins, and if
 * needed, more than 1 macro can be used for any pin. However, some macros
 * will have no effect if combined. For example debounce only affects input
 * or one of the interrupts. Some macros should not be combined, such as SMI
 * and regular interrupt. The defined macros and their parameters are:
 * PAD_NF		Define native alternate function for the pin.
 *	pin		the pin to be programmed
 *	function	the native function
 *	pull		pull up, pull down or no pull
 * PAD_GPI		The pin is a GPIO input
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 * PAD_GPO		The pin is a GPIO output
 *	pin		the pin to be programmed
 *	direction	high or low
 * PAD_INT		The pin is regular interrupt that works while booting
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH, BOTH_EDGES
 *	action		STATUS, DELIVER, STATUS_DELIVER
 * PAD_SCI		The pin is a SCI source
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH
 * PAD_SMI		The pin is a SMI source
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH
 * PAD_WAKE		The pin can wake, use after PAD_INT or PAD_SCI
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH, BOTH_EDGES
 *	type		S0i3, S3, S4_S5 or S4_S5 combinations (S0i3_S3 invalid)
 * PAD_DEBOUNCE		The input or interrupt will be debounced, invalid after
 *			PAD_NF
 *	pin		the pin to be programmed
 *	debounce_type	preserve low glitch, preserve high glitch, no glitch
 *	debounce_time	the debounce time
 */

/* Native function pad configuration */
#define PAD_NF(pin, func, pull) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## func, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = 0 }
/* General purpose input pad configuration */
#define PAD_GPI(pin, pull) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = 0 }
/* General purpose output pad configuration */
#define PAD_GPO(pin, direction) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_OUTPUT ## _OUT_ ## direction, \
		.flags = 0 }
/* Auxiliary macro for legacy interrupt and wake */
#define PAD_AUX1(pull, trigger) (GPIO_PULL ## _ ## pull | \
				 GPIO_INT ## _ ## trigger)
/* Legacy interrupt pad configuration */
#define PAD_INT(pin, pull, trigger, action) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = (PAD_AUX1(pull, trigger) | \
			    GPIO_EVENT_INT ## _ ## action), \
		.flags = GPIO_FLAG_INT }
/* Auxiliary macro for SCI and SMI */
#define PAD_AUX2(trigger, flag) (GPIO_TRIGGER ## _ ## trigger | flag)
/* SCI pad configuration */
#define PAD_SCI(pin, pull, trigger) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = PAD_AUX2(trigger, GPIO_FLAG_SCI) }
/* SMI pad configuration */
#define PAD_SMI(pin, pull, trigger) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = GPIO_PULL ## _ ## pull, \
		.flags = PAD_AUX2(trigger, GPIO_FLAG_SMI) }
/* WAKE pad configuration */
#define PAD_WAKE(pin, pull, trigger, type) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = (PAD_AUX1(pull, trigger) | \
			    GPIO_WAKE ## _ ## type), \
		.flags = GPIO_FLAG_WAKE }
/* pin debounce configuration */
#define PAD_DEBOUNCE(pin, type, time) \
	{ .gpio = (pin), \
		.function = pin ## _IOMUX_ ## GPIOxx, \
		.control = (GPIO_IN  ## _ ## type | GPIO_IN  ## _ ## time), \
		.flags = GPIO_FLAG_DEBOUNCE }

typedef uint32_t gpio_t;

/* Get the address of the control register of a particular pin */
uintptr_t gpio_get_address(gpio_t gpio_num);

/**
 * @brief program a particular set of GPIO
 *
 * @param gpio_list_ptr = pointer to array of gpio configurations
 * @param size = number of entries in array
 *
 * @return none
 */
void program_gpios(const struct soc_amd_gpio *gpio_list_ptr, size_t size);
/* Return the interrupt status and clear if set. */
int gpio_interrupt_status(gpio_t gpio);
/* Implemented by soc, provides table of avaialable GPIO mapping to Gevents */
void soc_get_gpio_event_table(const struct soc_amd_event **table, size_t *items);
/* May be implemented by soc to handle special cases */
void soc_gpio_hook(uint8_t gpio, uint8_t mux);

#endif /* __AMDBLOCK_GPIO_BANKS_H__ */
