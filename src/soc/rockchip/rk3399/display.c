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

void rk_display_init(device_t dev, uintptr_t lcdbase,
		     unsigned long fb_size)
{
	struct edid edid;
	uint32_t val;
	struct soc_rockchip_rk3399_config *conf = dev->chip_info;
	uintptr_t lower = ALIGN_DOWN(lcdbase, MiB);
	uintptr_t upper = ALIGN_UP(lcdbase + fb_size, MiB);
	enum vop_modes detected_mode = VOP_MODE_UNKNOWN;

	printk(BIOS_DEBUG, "LCD framebuffer @%p\n", (void *)(lcdbase));
	memset((void *)lcdbase, 0, fb_size);	/* clear the framebuffer */
	dcache_clean_invalidate_by_mva((void *)lower, upper - lower);
	mmu_config_range((void *)lower, upper - lower, UNCACHED_MEM);

	switch (conf->vop_mode) {
	case VOP_MODE_NONE:
		return;
	case VOP_MODE_AUTO_DETECT:
		/* try EDP first, then HDMI */
	case VOP_MODE_EDP:
		printk(BIOS_DEBUG, "Attempting to setup EDP display.\n");
		rkclk_configure_vop_aclk(conf->vop_id, 192 * MHz);

		/* select edp signal from vop0(big) or vop1(little) */
		val = (conf->vop_id == 1) ? RK_SETBITS(1 << 5) :
					    RK_CLRBITS(1 << 5);
		write32(&rk3399_grf->soc_con20, val);

		/* select edp clk from SoC interal 24M crystal, otherwise,
		 * it will source from edp's 24M clock(that depends on
		 * edp vendor, could be unstable)
		 */
		write32(&rk3399_grf->soc_con25, RK_SETBITS(1 << 11));

		rk_edp_init();

		if (rk_edp_get_edid(&edid) == 0) {
			detected_mode = VOP_MODE_EDP;
			break;
		}
		printk(BIOS_WARNING, "Cannot get EDID from EDP.\n");
		if (conf->vop_mode == VOP_MODE_EDP)
			return;
		/* fall thru */
	case VOP_MODE_HDMI:
		printk(BIOS_WARNING, "HDMI display is NOT supported yet.\n");
		return;
	default:
		printk(BIOS_WARNING, "Cannot read any edid info, aborting.\n");
		return;
	}

	if (rkclk_configure_vop_dclk(conf->vop_id,
				     edid.mode.pixel_clock * KHz)) {
		printk(BIOS_WARNING, "config vop err\n");
		return;
	}

	edid.framebuffer_bits_per_pixel = conf->framebuffer_bits_per_pixel;
	edid.bytes_per_line =
			edid.mode.ha * conf->framebuffer_bits_per_pixel / 8;
	edid.x_resolution = edid.mode.ha;
	edid.y_resolution = edid.mode.va;
	rkvop_mode_set(conf->vop_id, &edid, detected_mode);

	rkvop_enable(conf->vop_id, lcdbase, &edid);

	switch (detected_mode) {
	case VOP_MODE_HDMI:
		/* should not be here before HDMI supported */
		return;
	case VOP_MODE_EDP:
	default:
		if (rk_edp_enable()) {
			printk(BIOS_WARNING, "edp enable err\n");
			return;
		}
		mainboard_power_on_backlight();
		break;
	}

	set_vbe_mode_info_valid(&edid, (uintptr_t)lcdbase);
}
