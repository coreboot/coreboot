/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CHROMEOS_H__
#define __CHROMEOS_H__

#include <stddef.h>
#include <stdint.h>
#include <types.h>

#if CONFIG(CHROMEOS)
/* functions implemented in watchdog.c */
void mark_watchdog_tombstone(void);
void reboot_from_watchdog(void);
bool reset_watchdog_tombstone(void);
#else
static inline void mark_watchdog_tombstone(void) { return; }
static inline void reboot_from_watchdog(void) { return; }
#endif /* CONFIG_CHROMEOS */

#define UNDEFINED_FACTORY_CONFIG	~((uint64_t)0)

/**
 * Perform any platform specific actions required prior to resetting the Cr50.
 * Defined as weak function in cr50_enable_update.c
 */
void mainboard_prepare_cr50_reset(void);

void cbmem_add_vpd_calibration_data(void);
void chromeos_set_me_hash(u32*, int);
void chromeos_set_ramoops(void *ram_oops, size_t size);
/*
 * The factory config space is a one-time programmable info page.
 * For the unprovisioned one, the read will be 0x0.
 * Return "UNDEFINED_FACTORY_CONFIG" in case of error.
 */
uint64_t chromeos_get_factory_config(void);
/*
 * Determines whether a ChromeOS device is branded as a Chromebook Plus
 * based on specific bit flags:
 *
 * - Bit 4 (0x10): Indicates whether the device chassis has the
 *                 "chromebook-plus" branding.
 * - Bits 3-0 (0x1): Must be 0x1 to signify compliance with Chromebook Plus
 *                   hardware specifications.
 *
 * To be considered a Chromebook Plus, either of these conditions needs to be met.
 */
bool chromeos_device_branded_plus(void);

/*
 * Declaration for mainboards to use to generate ACPI-specific ChromeOS needs.
 */
void chromeos_acpi_gpio_generate(void);

enum {
	CROS_GPIO_REC = 1, /* Recovery */
	CROS_GPIO_DEPRECATED_DEV = 2, /* Developer;
				       * deprecated (chromium:942901) */
	CROS_GPIO_WP = 3, /* Write Protect */
	CROS_GPIO_PE = 4, /* Phase enforcement for final product */

	CROS_GPIO_ACTIVE_LOW = 0,
	CROS_GPIO_ACTIVE_HIGH = 1,

	CROS_GPIO_VIRTUAL = -1,
};

struct cros_gpio {
	int type;
	int polarity;
	int gpio_num;
	const char *device;
};

#define CROS_GPIO_INITIALIZER(typ, pol, num, dev) \
	{				\
		.type = (typ),		\
		.polarity = (pol),	\
		.gpio_num = (num),	\
		.device = (dev),	\
	}

#define CROS_GPIO_REC_INITIALIZER(pol, num, dev) \
	CROS_GPIO_INITIALIZER(CROS_GPIO_REC, pol, num, dev)

#define CROS_GPIO_REC_AL(num, dev) \
	CROS_GPIO_REC_INITIALIZER(CROS_GPIO_ACTIVE_LOW, num, dev)

#define CROS_GPIO_REC_AH(num, dev) \
	CROS_GPIO_REC_INITIALIZER(CROS_GPIO_ACTIVE_HIGH, num, dev)

#define CROS_GPIO_WP_INITIALIZER(pol, num, dev) \
	CROS_GPIO_INITIALIZER(CROS_GPIO_WP, pol, num, dev)

#define CROS_GPIO_WP_AL(num, dev) \
	CROS_GPIO_WP_INITIALIZER(CROS_GPIO_ACTIVE_LOW, num, dev)

#define CROS_GPIO_WP_AH(num, dev) \
	CROS_GPIO_WP_INITIALIZER(CROS_GPIO_ACTIVE_HIGH, num, dev)

#define CROS_GPIO_PE_INITIALIZER(pol, num, dev) \
	CROS_GPIO_INITIALIZER(CROS_GPIO_PE, pol, num, dev)

#define CROS_GPIO_PE_AL(num, dev) \
	CROS_GPIO_PE_INITIALIZER(CROS_GPIO_ACTIVE_LOW, num, dev)

#define CROS_GPIO_PE_AH(num, dev) \
	CROS_GPIO_PE_INITIALIZER(CROS_GPIO_ACTIVE_HIGH, num, dev)

struct cros_gpio_pack {
	int count;
	const struct cros_gpio *gpios;
};

extern const struct cros_gpio_pack variant_cros_gpio;

#define DECLARE_NO_CROS_GPIOS() \
	const struct cros_gpio_pack variant_cros_gpio = \
		{ .count = 0, .gpios = NULL }

#define DECLARE_CROS_GPIOS(x) \
	const struct cros_gpio_pack variant_cros_gpio = \
		{ .count = ARRAY_SIZE(x), .gpios = x }

#define DECLARE_WEAK_CROS_GPIOS(x) \
	const struct cros_gpio_pack __weak variant_cros_gpio = \
		{ .count = ARRAY_SIZE(x), .gpios = x }

#endif /* __CHROMEOS_H__ */
