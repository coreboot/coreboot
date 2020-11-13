/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_GPIO_BANKS_H
#define AMD_BLOCK_GPIO_BANKS_H

#include <types.h>

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

#define GPIO_MASTER_SWITCH	0xFC
#define   GPIO_MASK_STS_EN	BIT(28)
#define   GPIO_INTERRUPT_EN	BIT(30)
#define   GPIO_WAKE_EN		BIT(31)

#define GPIO_WAKE_STAT_0	0x2F0
#define GPIO_WAKE_STAT_1	0x2F4
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
void gpio_add_events(const struct gpio_wake_state *state);

#define GPIO_PIN_IN		(1 << 0)	/* for byte access */
#define GPIO_PIN_OUT		(1 << 6)	/* for byte access */

/* Pad trigger type - Level or Edge */
#define GPIO_TRIGGER_EDGE	(0 << 8)
#define GPIO_TRIGGER_LEVEL	(1 << 8)
#define GPIO_TRIGGER_MASK	(1 << 8)

/*
 * Pad polarity:
 * Level trigger - High or Low
 * Edge trigger - High (Rising), Low (Falling), Both
 */
#define GPIO_ACTIVE_HIGH	(0 << 9)
#define GPIO_ACTIVE_LOW		(1 << 9)
#define GPIO_ACTIVE_BOTH	(2 << 9)
#define GPIO_ACTIVE_MASK	(3 << 9)

/*
 * Pad trigger and polarity configuration.
 * This determines the filtering applied on the input signal at the pad.
 */
#define GPIO_TRIGGER_EDGE_HIGH		(GPIO_ACTIVE_HIGH | GPIO_TRIGGER_EDGE)
#define GPIO_TRIGGER_EDGE_LOW		(GPIO_ACTIVE_LOW | GPIO_TRIGGER_EDGE)
#define GPIO_TRIGGER_BOTH_EDGES		(GPIO_ACTIVE_BOTH | GPIO_TRIGGER_EDGE)
#define GPIO_TRIGGER_LEVEL_HIGH		(GPIO_ACTIVE_HIGH | GPIO_TRIGGER_LEVEL)
#define GPIO_TRIGGER_LEVEL_LOW		(GPIO_ACTIVE_LOW | GPIO_TRIGGER_LEVEL)

#define GPIO_INT_ENABLE_STATUS		(1 << 11)
#define GPIO_INT_ENABLE_DELIVERY	(1 << 12)
#define GPIO_INT_ENABLE_STATUS_DELIVERY	\
				(GPIO_INT_ENABLE_STATUS | GPIO_INT_ENABLE_DELIVERY)
#define GPIO_INT_ENABLE_MASK		(3 << 11)

#define GPIO_S0I3_WAKE_EN	(1 << 13)
#define GPIO_S3_WAKE_EN		(1 << 14)
#define GPIO_S4_S5_WAKE_EN	(1 << 15)

#define GPIO_PIN_STS		(1 << 16)
#define GPIO_8KPULLUP_SELECT	(1 << 19)
#define GPIO_PULLUP_ENABLE	(1 << 20)
#define GPIO_PULLDOWN_ENABLE	(1 << 21)
#define GPIO_PULL_MASK		(7 << 19)

#define GPIO_OUTPUT_SHIFT	22
#define GPIO_OUTPUT_VALUE	(1 << GPIO_OUTPUT_SHIFT)
#define GPIO_OUTPUT_ENABLE	(1 << 23)
#define GPIO_OUTPUT_MASK	(3 << GPIO_OUTPUT_SHIFT)

#define GPIO_INT_STATUS		(1 << 28)
#define GPIO_WAKE_STATUS	(1 << 29)
#define GPIO_STATUS_MASK	(3 << 28)

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

#define AMD_GPIO_MUX_MASK			0x03

/*
 * Flags used for GPIO configuration. These provide additional information that does not go
 * directly into GPIO control register. These are stored in `flags` field in soc_amd_gpio.
 */
#define GPIO_FLAG_EVENT_TRIGGER_LEVEL	(1 << 0)
#define GPIO_FLAG_EVENT_TRIGGER_EDGE	(0 << 0)
#define GPIO_FLAG_EVENT_TRIGGER_MASK	(1 << 0)
#define GPIO_FLAG_EVENT_ACTIVE_HIGH	(1 << 1)
#define GPIO_FLAG_EVENT_ACTIVE_LOW	(0 << 1)
#define GPIO_FLAG_EVENT_ACTIVE_MASK	(1 << 1)
#define GPIO_FLAG_SCI			(1 << 2)
#define GPIO_FLAG_SMI			(1 << 3)

/* Trigger configuration for GPIO SCI/SMI events. */
#define GPIO_FLAG_EVENT_TRIGGER_LEVEL_HIGH	(GPIO_FLAG_EVENT_TRIGGER_LEVEL | \
						 GPIO_FLAG_EVENT_ACTIVE_HIGH)
#define GPIO_FLAG_EVENT_TRIGGER_LEVEL_LOW	(GPIO_FLAG_EVENT_TRIGGER_LEVEL | \
						 GPIO_FLAG_EVENT_ACTIVE_LOW)
#define GPIO_FLAG_EVENT_TRIGGER_EDGE_HIGH	(GPIO_FLAG_EVENT_TRIGGER_EDGE | \
						 GPIO_FLAG_EVENT_ACTIVE_HIGH)
#define GPIO_FLAG_EVENT_TRIGGER_EDGE_LOW	(GPIO_FLAG_EVENT_TRIGGER_EDGE | \
						 GPIO_FLAG_EVENT_ACTIVE_LOW)

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

#define DEB_GLITCH_SHIFT		5
#define DEB_GLITCH_LOW			1
#define DEB_GLITCH_HIGH			2
#define DEB_GLITCH_NONE			3
#define GPIO_DEB_PRESERVE_LOW_GLITCH	(DEB_GLITCH_LOW << DEB_GLITCH_SHIFT)
#define GPIO_DEB_PRESERVE_HIGH_GLITCH	(DEB_GLITCH_HIGH << DEB_GLITCH_SHIFT)
#define GPIO_DEB_REMOVE_GLITCH		(DEB_GLITCH_NONE << DEB_GLITCH_SHIFT)

#define GPIO_TIMEBASE_61uS		0
#define GPIO_TIMEBASE_183uS		(1 << 4)
#define GPIO_TIMEBASE_15560uS		(1 << 7)
#define GPIO_TIMEBASE_62440uS		(GPIO_TIMEBASE_183uS | \
					GPIO_TIMEBASE_15560uS)
#define GPIO_DEB_DEBOUNCE_DISABLED	(0 | GPIO_TIMEBASE_61uS)
#define GPIO_DEB_60uS			(1 | GPIO_TIMEBASE_61uS)
#define GPIO_DEB_120uS			(2 | GPIO_TIMEBASE_61uS)
#define GPIO_DEB_200uS			(3 | GPIO_TIMEBASE_61uS)
#define GPIO_DEB_500uS			(8 | GPIO_TIMEBASE_61uS)
#define GPIO_DEB_1mS			(5 | GPIO_TIMEBASE_183uS)
#define GPIO_DEB_2mS			(11 | GPIO_TIMEBASE_183uS)
#define GPIO_DEB_15mS			(1 | GPIO_TIMEBASE_15560uS)
#define GPIO_DEB_50mS			(3 | GPIO_TIMEBASE_15560uS)
#define GPIO_DEB_100mS			(6 | GPIO_TIMEBASE_15560uS)
#define GPIO_DEB_200mS			(13 | GPIO_TIMEBASE_15560uS)
#define GPIO_DEB_500mS			(8 | GPIO_TIMEBASE_62440uS)

#define GPIO_DEB_MASK			0xff

#define GPIO_WAKE_S0i3			(1 << 13)
#define GPIO_WAKE_S3			(1 << 14)
#define GPIO_WAKE_S4_S5			(1 << 15)
#define GPIO_WAKE_S0i3_S4_S5		(GPIO_WAKE_S0i3 | GPIO_WAKE_S4_S5)
#define GPIO_WAKE_S3_S4_S5		(GPIO_WAKE_S3 | GPIO_WAKE_S4_S5)
#define GPIO_WAKE_MASK			(7 << 13)

/*
 * Mask used to reset bits in GPIO control register when configuring pad using `program_gpios()`
 * Bits that are preserved/untouched:
 * - Reserved bits
 * - Drive strength bits
 * - Read only bits
 */
#define PAD_CFG_MASK		(GPIO_DEB_MASK | GPIO_TRIGGER_MASK | GPIO_ACTIVE_MASK | \
				 GPIO_INT_ENABLE_MASK | GPIO_WAKE_MASK | GPIO_PULL_MASK | \
				 GPIO_OUTPUT_MASK | GPIO_STATUS_MASK)

/*
 * Several macros are available to declare programming of GPIO pins. The defined macros and
 * their parameters are:
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
 *	event trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH
 * PAD_SMI		The pin is a SMI source
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	event trigger		LEVEL_LOW, LEVEL_HIGH
 * PAD_WAKE		The pin can wake, use after PAD_INT or PAD_SCI
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	trigger		LEVEL_LOW, LEVEL_HIGH, EDGE_LOW, EDGE_HIGH, BOTH_EDGES
 *	type		S0i3, S3, S4_S5 or S4_S5 combinations (S0i3_S3 invalid)
 * PAD_DEBOUNCE		The input or interrupt will be debounced
 *	pin		the pin to be programmed
 *	pull		pull up, pull down or no pull
 *	debounce_type	preserve low glitch, preserve high glitch, no glitch
 *	debounce_time	the debounce time
 */

#define PAD_CFG_STRUCT_FLAGS(__pin, __function, __control, __flags)	\
	{								\
		.gpio = __pin,						\
		.function = __function,					\
		.control = __control,					\
		.flags = __flags,					\
	}

#define PAD_CFG_STRUCT(__pin, __function, __control)	\
	PAD_CFG_STRUCT_FLAGS(__pin, __function, __control, 0)

#define PAD_PULL(__pull)		GPIO_PULL_ ## __pull
#define PAD_OUTPUT(__dir)		GPIO_OUTPUT_OUT_ ## __dir
#define PAD_TRIGGER(__trig)		GPIO_TRIGGER_ ## __trig
#define PAD_INT_ENABLE(__action)	GPIO_INT_ENABLE_ ## __action
#define PAD_FLAG_EVENT_TRIGGER(__trig)	GPIO_FLAG_EVENT_TRIGGER_ ## __trig
#define PAD_WAKE_ENABLE(__wake)		GPIO_WAKE_ ## __wake
#define PAD_DEBOUNCE_CONFIG(__deb)	GPIO_DEB_ ## __deb

/* Native function pad configuration */
#define PAD_NF(pin, func, pull)						\
	PAD_CFG_STRUCT(pin, pin ## _IOMUX_ ## func, PAD_PULL(pull))

/* General purpose input pad configuration */
#define PAD_GPI(pin, pull)							\
	PAD_CFG_STRUCT(pin, pin ## _IOMUX_GPIOxx, PAD_PULL(pull))

/* General purpose output pad configuration */
#define PAD_GPO(pin, direction)			\
	PAD_CFG_STRUCT(pin, pin ## _IOMUX_GPIOxx, PAD_OUTPUT(direction))

/* Legacy interrupt pad configuration */
#define PAD_INT(pin, pull, trigger, action)				\
	PAD_CFG_STRUCT(pin, pin ## _IOMUX_GPIOxx,			\
		PAD_PULL(pull) | PAD_TRIGGER(trigger) | PAD_INT_ENABLE(action))

/* SCI pad configuration */
#define PAD_SCI(pin, pull, trigger)					\
	PAD_CFG_STRUCT_FLAGS(pin, pin ## _IOMUX_GPIOxx,		\
		PAD_PULL(pull) | PAD_TRIGGER(LEVEL_HIGH),		\
		PAD_FLAG_EVENT_TRIGGER(trigger) | GPIO_FLAG_SCI)

/* SMI pad configuration */
#define PAD_SMI(pin, pull, trigger)					\
	PAD_CFG_STRUCT_FLAGS(pin, pin ## _IOMUX_GPIOxx,		\
		PAD_PULL(pull) | PAD_TRIGGER(LEVEL_HIGH),		\
		PAD_FLAG_EVENT_TRIGGER(trigger) | GPIO_FLAG_SMI)

/* WAKE pad configuration */
#define PAD_WAKE(pin, pull, trigger, type)				\
	PAD_CFG_STRUCT(pin, pin ## _IOMUX_GPIOxx,			\
		PAD_PULL(pull) | PAD_TRIGGER(trigger) | PAD_WAKE_ENABLE(type))

/* pin debounce configuration */
#define PAD_DEBOUNCE(pin, pull, type, time)				\
	PAD_CFG_STRUCT(pin, pin ## _IOMUX_GPIOxx,			\
		PAD_PULL(pull) | PAD_DEBOUNCE_CONFIG(type) | PAD_DEBOUNCE_CONFIG(time))

/* No-connect pad - configured as input with PULL_DOWN */
#define PAD_NC(pin)							\
	PAD_CFG_STRUCT(pin, pin ## _IOMUX_GPIOxx, PAD_PULL(PULL_DOWN))

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
