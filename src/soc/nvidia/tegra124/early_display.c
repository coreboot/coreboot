/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <arch/cache.h>
#include "chip.h"
#include <soc/display.h>
#include "gpio.h"

/* the primary purpose of this function is to kick off things in
 * romstage that are time consuming. No more should be done at this
 * point than the bare minimum that will allow us to pick up the work
 * in the ramstage and not require lots of udelays.  ramstage will do
 * the rest. As it happens, that merely means 'turn off everything you can,
 * save the minimum long-delay bit that you need to get graphics going'. It's
 * important not to have the backlight on as people don't like display trash
 * on startup, even if I do.
 */
void setup_display(struct soc_nvidia_tegra124_config *config)
{
	if (config->panel_vdd_gpio){
		gpio_output(config->panel_vdd_gpio, 1);
		printk(BIOS_SPEW,"%s: setting gpio %08x to %d\n",
			__func__, config->panel_vdd_gpio, 1);
	}
	if (config->lvds_shutdown_gpio){
		gpio_output(config->lvds_shutdown_gpio, 0);
		printk(BIOS_SPEW,"%s: setting gpio %08x to %d\n",
			__func__, config->lvds_shutdown_gpio, 0);
	}
	if (config->backlight_en_gpio){
		gpio_output(config->backlight_en_gpio, 0);
		printk(BIOS_SPEW,"%s: setting gpio %08x to %d\n",
			__func__, config->backlight_en_gpio, 0);
	}
	if (config->backlight_vdd_gpio){
		gpio_output(config->backlight_vdd_gpio, 0);
		printk(BIOS_SPEW,"%s: setting gpio %08x to %d\n",
			__func__, config->backlight_vdd_gpio, 0);
	}

}

