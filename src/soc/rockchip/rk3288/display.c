/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
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
#include <soc/soc.h>
#include <soc/vop.h>

#include "chip.h"

void rk_display_init(device_t dev, u32 lcdbase,
		unsigned long fb_size)
{
	struct edid edid;
	struct soc_rockchip_rk3288_config *conf = dev->chip_info;
	uint32_t lower = ALIGN_DOWN(lcdbase, MiB);
	uint32_t upper = ALIGN_UP(lcdbase + fb_size, MiB);

	printk(BIOS_SPEW, "LCD framebuffer @%p\n", (void *)(lcdbase));
	memset((void *)lcdbase, 0, fb_size);	/* clear the framebuffer */
	dcache_clean_invalidate_by_mva((void *)lower, upper - lower);
	mmu_config_range(lower / MiB, (upper - lower) / MiB, DCACHE_OFF);

	rkclk_configure_edp();

	rkclk_configure_vop_aclk(conf->vop_id, 192 * MHz);

	rk_edp_init(conf->vop_id);
	udelay(conf->lcd_power_on_udelay);

	if (rk_edp_get_edid(&edid)) {
		printk(BIOS_WARNING, "can not get edid\n");
		return;
	}

	if (rkclk_configure_vop_dclk(conf->vop_id, edid.pixel_clock * KHz)) {
		printk(BIOS_WARNING, "config vop err\n");
		return;
	}

	edid.framebuffer_bits_per_pixel = conf->framebuffer_bits_per_pixel;
	edid.bytes_per_line = edid.ha * conf->framebuffer_bits_per_pixel / 8;
	edid.x_resolution = edid.ha;
	edid.y_resolution = edid.va;
	rkvop_mode_set(conf->vop_id, &edid);

	rkvop_enable(conf->vop_id, lcdbase, &edid);

	if (rk_edp_enable()) {
		printk(BIOS_WARNING, "edp enable err\n");
		return;
	}

	set_vbe_mode_info_valid(&edid, (uintptr_t)lcdbase);
	gpio_output(conf->lcd_bl_pwm_gpio, 0);
	gpio_output(conf->lcd_bl_en_gpio, 1); /* LCD_BL */
	udelay(conf->bl_power_on_udelay);
	gpio_output(conf->lcd_bl_pwm_gpio, 1); /* BL_EN */
}
