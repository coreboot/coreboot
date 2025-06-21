/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_CFG_H
#define SOC_INTEL_COMMON_BLOCK_CFG_H

#include <boot/coreboot_tables.h>
#include <intelblocks/gspi.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <intelblocks/mmc.h>

enum {
	CHIPSET_LOCKDOWN_COREBOOT = 0,	/* coreboot handles locking */
	CHIPSET_LOCKDOWN_FSP,		/* FSP handles locking per UPDs */
};

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

/*
 * This structure will hold data required by common blocks.
 * These are soc specific configurations which will be filled by soc.
 * We'll fill this structure once during init and use the data in common block.
 */
struct soc_intel_common_config {
	int chipset_lockdown;
	struct gspi_cfg gspi[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	struct dw_i2c_bus_config i2c[CONFIG_SOC_INTEL_I2C_DEV_MAX];
	/* PCH Thermal Trip Temperature in deg C */
	uint8_t pch_thermal_trip;
	struct mmc_dll_params emmc_dll;
	enum lb_fb_orientation panel_orientation;
	enum fw_splash_vertical_alignment logo_valignment;
};

/* This function to retrieve soc config structure required by common code */
struct soc_intel_common_config *chip_get_common_soc_structure(void);

#endif /* SOC_INTEL_COMMON_BLOCK_CFG_H */
