/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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

#include <arch/cache.h>
#include <arch/mmu.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <edid.h>
#include <gpio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/edp.h>
#include <soc/gpio.h>
#include <soc/grf.h>
#include <soc/mmu_operations.h>
#include <soc/soc.h>
#include <soc/vop.h>

#include "chip.h"

static void reset_edp(void)
{
	/* rst edp */
	write32(&cru_ptr->softrst_con[17],
		RK_SETBITS(1 << 12 | 1 << 13));
		udelay(1);
	write32(&cru_ptr->softrst_con[17],
		RK_CLRBITS(1 << 12 | 1 << 13));
	printk(BIOS_WARNING, "Retrying epd initialization.\n");
}

void rk_display_init(device_t dev)
{
	struct edid edid;
	struct soc_rockchip_rk3399_config *conf = dev->chip_info;
	enum vop_modes detected_mode = VOP_MODE_UNKNOWN;
	int retry_count = 0;

	/* let's use vop0 in rk3399 */
	uint32_t vop_id = 0;

	switch (conf->vop_mode) {
	case VOP_MODE_NONE:
		return;
	case VOP_MODE_AUTO_DETECT:
		/* try EDP first, then HDMI */
	case VOP_MODE_EDP:
		printk(BIOS_DEBUG, "Attempting to set up EDP display.\n");
		rkclk_configure_vop_aclk(vop_id, 200 * MHz);

		/* select edp signal from vop0 */
		write32(&rk3399_grf->soc_con20, RK_CLRBITS(1 << 5));

		/* select edp clk from SoC internal 24M crystal, otherwise,
		 * it will source from edp's 24M clock (that depends on
		 * edp vendor, could be unstable)
		 */
		write32(&rk3399_grf->soc_con25, RK_SETBITS(1 << 11));

retry_edp:
		while (retry_count++ < 3) {
			rk_edp_init();
			if (rk_edp_get_edid(&edid) == 0) {
				detected_mode = VOP_MODE_EDP;
				break;
			}
			if (retry_count == 3) {
				printk(BIOS_WARNING, "Warning: epd initialization failed.\n");
				return;
			} else {
				reset_edp();
			}
		}
		break;
	case VOP_MODE_HDMI:
		printk(BIOS_WARNING, "HDMI display is NOT supported yet.\n");
		return;
	default:
		printk(BIOS_WARNING, "Cannot read any EDID info, aborting.\n");
		return;
	}

	if (rkclk_configure_vop_dclk(vop_id,
				     edid.mode.pixel_clock * KHz)) {
		printk(BIOS_WARNING, "config vop err\n");
		return;
	}

	edid_set_framebuffer_bits_per_pixel(&edid,
		conf->framebuffer_bits_per_pixel, 0);
	rkvop_mode_set(vop_id, &edid, detected_mode);

	rkvop_prepare(vop_id, &edid);

	switch (detected_mode) {
	case VOP_MODE_HDMI:
		/* should not be here before HDMI supported */
		return;
	case VOP_MODE_EDP:
	default:
		/* will enable edp in depthcharge */
		if (rk_edp_prepare()) {
			reset_edp();
			goto retry_edp; /* Rerun entire init sequence */
		}
		mainboard_power_on_backlight();
		break;
	}

	set_vbe_mode_info_valid(&edid, (uintptr_t)0);
	return;
}
