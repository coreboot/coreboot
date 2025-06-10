/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BOOTSPLASH_H__
#define __BOOTSPLASH_H__

#include <types.h>

enum bootsplash_type {
	/* Indicates a low battery bootsplash logo. */
	BOOTSPLASH_LOW_BATTERY,
	/* Indicates a Main OEM defined bootsplash logo for center of the splash screen. */
	BOOTSPLASH_CENTER,
	/* Indicates an optional OEM defined bootsplash logo for footer of the splash screen. */
	BOOTSPLASH_FOOTER,

	/* It's used to determine the total number of bootsplash types. */
	BOOTSPLASH_MAX_NUM,
};

/**
 * Sets up the framebuffer with the bootsplash.jpg from cbfs.
 * Returns 0 on success
 * CB_ERR on cbfs errors
 * and >0 on jpeg errors.
 */
void set_bootsplash(unsigned char *framebuffer, unsigned int x_resolution,
		    unsigned int y_resolution, unsigned int bytes_per_line,
		    unsigned int fb_resolution);

/*
 * Allow platform-specific BMP logo overrides via HAVE_CUSTOM_BMP_LOGO config.
 * For example: Introduce configurable BMP logo for customization on platforms like ChromeOS
 */
const char *bmp_logo_filename(void);
void *bmp_load_logo(size_t *logo_size);
void *bmp_load_logo_by_type(enum bootsplash_type type, size_t *logo_size);
void bmp_release_logo(void);
/*
 * Platform specific callbacks for power-off handling.
 *
 * These callbacks allow the platform to determine if a power-off is
 * necessary due to various reasons, such as low battery detection.
 *
 * Additionally, API to perform platform specific power-off
 */
#if CONFIG(PLATFORM_HAS_LOW_BATTERY_INDICATOR)
bool platform_is_low_battery_shutdown_needed(void);
#else
static inline bool platform_is_low_battery_shutdown_needed(void) { return false; }
#endif

#endif
