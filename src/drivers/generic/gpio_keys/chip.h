/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_GENERIC_GPIO_KEYS_H__
#define __DRIVERS_GENERIC_GPIO_KEYS_H__

#include <acpi/acpi_device.h>
#include <stdint.h>

/* Linux input type */
enum {
	/* Switch event */
	EV_SW = 0x5,
};

/* Switch events type (Linux code emitted for EV_SW) */
enum {
	SW_MUTE_DEVICE = 0xe,
	SW_PEN_INSERTED = 0xf,
};

/* Trigger for wakeup event action */
enum {
	EV_ACT_ANY,
	EV_ACT_ASSERTED,
	EV_ACT_DEASSERTED,
};

enum {
	/*
	 * GPIO key uses SCI route to wake the system from suspend state. This is typically used
	 * when the input line is dual routed i.e. one for IRQ and other for SCI or if the GPIO
	 * controller is capable of handling the filtering for IRQ and SCI separately. This
	 * requires "wake" property to be provided by the board which represents the GPE # for
	 * wake. It is exposed as _PRW in ACPI tables.
	 */
	WAKEUP_ROUTE_SCI,
	/*
	 * GPIO key uses GPIO controller IRQ route for wake. This is used when IRQ and wake are
	 * routed to the same pad and the GPIO controller is not capable of handling the trigger
	 * filtering separately for IRQ and wake. Kernel driver for gpio-keys takes care of
	 * reconfiguring the IRQ trigger as both edges when used in S0 and the edge requested by
	 * BIOS (as per wakeup_event_action) when entering suspend. In this case, _PRW is not
	 * exposed for the key device.
	 */
	WAKEUP_ROUTE_GPIO_IRQ,
	/* GPIO key does not support wake. */
	WAKEUP_ROUTE_DISABLED,
};

/* Details of the child node defining key */
struct key_info {
	/* Device name of the child node - Mandatory */
	const char *dev_name;
	/* Keycode emitted for this key - Mandatory */
	uint32_t linux_code;
	/*
	 * Event type generated for this key
	 * See EV_* above.
	 */
	uint32_t linux_input_type;
	/* Descriptive name of the key */
	const char *label;
	/* Wakeup route (if any) for the key. See WAKEUP_ROUTE_* macros above. */
	unsigned int wakeup_route;
	/* Wake GPE -- SCI GPE # for wake. Required for WAKEUP_ROUTE_SCI. */
	unsigned int wake_gpe;
	/* Trigger for Wakeup Event Action as defined in EV_ACT_* enum */
	unsigned int wakeup_event_action;
	/* Can this key be disabled? */
	bool can_be_disabled;
	/* Debounce interval time in milliseconds */
	uint32_t debounce_interval;
};

struct drivers_generic_gpio_keys_config {
	/* Device name of the parent gpio-keys node */
	const char *name;
	/* Name of the input device - Optional */
	const char *label;
	/* GPIO line providing the key - Mandatory */
	struct acpi_gpio gpio;
	/* Is this a polled GPIO button? - Optional */
	bool is_polled;
	/* Poll interval - Mandatory only if GPIO is polled. */
	uint32_t poll_interval;
	/* Details about the key - Mandatory */
	struct key_info key;
};

#endif /* __DRIVERS_GENERIC_GPIO_KEYS_H__ */
