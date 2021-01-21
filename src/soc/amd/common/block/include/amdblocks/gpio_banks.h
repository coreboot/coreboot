/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_GPIO_BANKS_H
#define AMD_BLOCK_GPIO_BANKS_H

#include <types.h>
#include "gpio_defs.h"

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

struct gpio_wake_state {
	uint32_t control_switch;
	uint32_t wake_stat[2];
	/* Number of wake_gpio with a valid setting. */
	uint32_t num_valid_wake_gpios;
	/* GPIO index number that caused a wake. */
	uint8_t wake_gpios[16];
};

/* Fill gpio_wake_state object for future event reporting. */
void gpio_fill_wake_state(struct gpio_wake_state *state);
/* Add gpio events to the eventlog. */
void gpio_add_events(void);

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

static inline bool is_gpio_event_level_triggered(uint32_t flags)
{
	return (flags & GPIO_FLAG_EVENT_TRIGGER_MASK) == GPIO_FLAG_EVENT_TRIGGER_LEVEL;
}

static inline bool is_gpio_event_edge_triggered(uint32_t flags)
{
	return (flags & GPIO_FLAG_EVENT_TRIGGER_MASK) == GPIO_FLAG_EVENT_TRIGGER_EDGE;
}

static inline bool is_gpio_event_active_high(uint32_t flags)
{
	return (flags & GPIO_FLAG_EVENT_ACTIVE_MASK) == GPIO_FLAG_EVENT_ACTIVE_HIGH;
}

static inline bool is_gpio_event_active_low(uint32_t flags)
{
	return (flags & GPIO_FLAG_EVENT_ACTIVE_MASK) == GPIO_FLAG_EVENT_ACTIVE_LOW;
}

typedef uint32_t gpio_t;

/*
 * gpio_configure_pads_with_override accepts as input two GPIO tables:
 * 1. Base config
 * 2. Override config
 *
 * This function configures raw pads in base config and applies override in
 * override config if any. Thus, for every GPIO_x in base config, this function
 * looks up the GPIO in override config and if it is present there, then applies
 * the configuration from override config.
 */
void gpio_configure_pads_with_override(const struct soc_amd_gpio *base_cfg,
					size_t base_num_pads,
					const struct soc_amd_gpio *override_cfg,
					size_t override_num_pads);

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
/* Implemented by soc, provides table of available GPIO mapping to Gevents */
void soc_get_gpio_event_table(const struct soc_amd_event **table, size_t *items);
/* May be implemented by soc to handle special cases */
void soc_gpio_hook(uint8_t gpio, uint8_t mux);

#endif /* AMD_BLOCK_GPIO_BANKS_H */
