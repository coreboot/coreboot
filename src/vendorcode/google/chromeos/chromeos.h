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

/**
 * Perform any platform specific actions required prior to resetting the Cr50.
 * Defined as weak function in cr50_enable_update.c
 */
void mainboard_prepare_cr50_reset(void);

void cbmem_add_vpd_calibration_data(void);
void chromeos_set_me_hash(u32*, int);
void chromeos_set_ramoops(void *ram_oops, size_t size);
void chromeos_init_chromeos_acpi(void);

/**
 * get_dsm_calibration_from_key - Gets value related to DSM calibration from VPD
 * @key: The key in RO_VPD. The valid prefix is "dsm_calib_". The valid keys are
 *   documented in https://chromeos.google.com/partner/dlm/docs/factory/vpd.html.
 * @value: Output value. The value read from VPD parsed into uint64_t integer.
 *
 * Returns CB_SUCCESS on success or CB_ERR on failure.
 */
enum cb_err get_dsm_calibration_from_key(const char *key, uint64_t *value);

/*
 * Declaration for mainboards to use to generate ACPI-specific Chrome OS needs.
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
