/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BOOTSPLASH_H__
#define __BOOTSPLASH_H__

#include <boot/coreboot_tables.h>
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
 * Specifies the vertical alignment for the splash screen image.
 *
 * Visual Guide (representing the display area and the [LOGO]):
 *
 * Each option dictates the vertical placement of the splash image
 * within the display's height.
 */
enum fw_splash_vertical_alignment {
	/*
	 * The splash image is centered vertically `(Y-axis - logo_height)/2` on the screen.
	 * The center of the [LOGO] aligns with the vertical center of the screen.
	 *
	 * +---------------+
	 * |               |
	 * |               |
	 * |    [LOGO]     |  <-- Vertically Centered
	 * |               |
	 * |               |
	 * +---------------+
	 */
	FW_SPLASH_VALIGNMENT_CENTER = 0,

	/*
	 * The splash image is aligned to the top edge of the screen.
	 *
	 * +---------------+
	 * |    [LOGO]     |  <-- Top Aligned
	 * |               |
	 * |               |
	 * |               |
	 * |               |
	 * +---------------+
	 */
	FW_SPLASH_VALIGNMENT_TOP = 1,

	/*
	 * The splash image is aligned to the bottom edge of the screen.
	 *
	 * +---------------+
	 * |               |
	 * |               |
	 * |               |
	 * |               |
	 * |    [LOGO]     |  <-- Bottom Aligned
	 * +---------------+
	 */
	FW_SPLASH_VALIGNMENT_BOTTOM = 2,

	/*
	 * The splash image is placed in the vertical middle `(Y-axis/2)` of the screen
	 * (without considering the `logo height`). This means the TOP EDGE of the
	 * [LOGO] aligns with the screen's vertical midpoint line.
	 *
	 * +---------------+
	 * | (Upper Half)  |
	 * |               |
	 * |    [LOGO]     | <-- [LOGO] aligns at Middle of the Y-axis
	 * |               |
	 * | (Lower Half)  |
	 * +---------------+
	 *
	 * Note: The distinction between CENTER and MIDDLE is relevant as in for MIDDLE
	 * alignment, it ignores the logo height (i.e., the logo's top edge is placed
	 * at the screen's Y-midpoint). CENTER alignment, by contrast, would place
	 * the geometrical center of the logo at the screen's Y-midpoint.
	 */
	FW_SPLASH_VALIGNMENT_MIDDLE = 3,
};

enum fw_splash_horizontal_alignment {
	/*
	 * The splash image is centered horizontally `(X-axis - logo_width)/2` on the screen.
	 * The center of the [LOGO] aligns with the horizontal center of the screen.
	 *
	 * +-----------------+
	 * |      [LOGO]     |
	 * +-----------------+
	 */
	FW_SPLASH_HALIGNMENT_CENTER = 0,

	/*
	 * The splash image is aligned to the left edge of the screen.
	 *
	 * +-----------------+
	 * |[LOGO]           |
	 * +-----------------+
	 */
	FW_SPLASH_HALIGNMENT_LEFT = 1,

	/*
	 * The splash image is aligned to the right edge of the screen.
	 *
	 * +-----------------+
	 * |           [LOGO]|
	 * +-----------------+
	 */
	FW_SPLASH_HALIGNMENT_RIGHT = 2,
};

struct logo_config {
	uintptr_t framebuffer_base;
	uint32_t horizontal_resolution;
	uint32_t vertical_resolution;
	uint32_t bytes_per_scanline;
	enum lb_fb_orientation panel_orientation;
	enum fw_splash_horizontal_alignment halignment;
	enum fw_splash_vertical_alignment valignment;
	uint8_t logo_bottom_margin;
};

void render_logo_to_framebuffer(struct logo_config *config);
void load_and_convert_bmp_to_blt(uintptr_t *logo, size_t *logo_size,
	uintptr_t *blt, size_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation);
void convert_bmp_to_blt(uintptr_t logo, size_t logo_size,
	uintptr_t *blt, size_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation);

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
