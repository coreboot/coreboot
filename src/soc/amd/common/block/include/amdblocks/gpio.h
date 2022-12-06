/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_GPIO_BANKS_H
#define AMD_BLOCK_GPIO_BANKS_H

#include <types.h>
#include <amdblocks/gpio_defs.h>

typedef uint32_t gpio_t;

struct soc_amd_gpio {
	gpio_t gpio;
	uint8_t function;
	uint32_t control;
	uint32_t flags;
};

struct soc_amd_gpio_register_save {
	uint32_t control_value;
	uint8_t mux_value;
};

struct soc_amd_event {
	gpio_t gpio;
	uint8_t event;
};

struct gpio_wake_state {
	uint32_t control_switch;
	uint32_t wake_stat[2];
	/* Number of wake_gpio with a valid setting. */
	uint32_t num_valid_wake_gpios;
	/* GPIO index number that caused a wake. */
	gpio_t wake_gpios[16];
};

/* Fill gpio_wake_state object for future event reporting. */
void gpio_fill_wake_state(struct gpio_wake_state *state);
/* Add gpio events to the eventlog. */
void gpio_add_events(void);

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

/*
 * gpio_configure_pads_with_override accepts as input two GPIO tables:
 * 1. Base config
 * 2. Override config
 *
 * This function configures raw pads in base config and applies override in
 * override config if any. Thus, for every GPIO_x in base config, this function
 * looks up the GPIO in override config and if it is present there, then applies
 * the configuration from override config. GPIOs that are only specified in the
 * override, but not in the base configuration, will be ignored.
 */
void gpio_configure_pads_with_override(const struct soc_amd_gpio *base_cfg,
					size_t base_num_pads,
					const struct soc_amd_gpio *override_cfg,
					size_t override_num_pads);

/**
 * @brief program a particular set of GPIO
 *
 * @param gpio_list_ptr = pointer to array of gpio configurations
 * @param size = number of entries in array
 *
 * @return none
 */
void gpio_configure_pads(const struct soc_amd_gpio *gpio_list_ptr, size_t size);
/* Return the interrupt status and clear if set. */
int gpio_interrupt_status(gpio_t gpio);
/* Implemented by soc, provides table of available GPIO mapping to Gevents */
void soc_get_gpio_event_table(const struct soc_amd_event **table, size_t *items);

void gpio_save_pin_registers(gpio_t gpio, struct soc_amd_gpio_register_save *save);
void gpio_restore_pin_registers(gpio_t gpio, struct soc_amd_gpio_register_save *save);

#endif /* AMD_BLOCK_GPIO_BANKS_H */
