/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <stdint.h>
#include <string.h>
#include <southbridge/intel/common/gpio.h>
#include <northbridge/intel/gm45/gm45.h>
#include <console/console.h>
#include <option.h>

#define HYBRID_GRAPHICS_INTEGRATED_ONLY 0
#define HYBRID_GRAPHICS_DISCRETE_ONLY 1
#define HYBRID_GRAPHICS_SWITCHABLE 2

#define MUX_GPIO 22
#define BCL_CTL_GPIO 19
#define GFX_PWR_EN_GPIO 49

#define HYBRID_DETECT_GPIO 21

void hybrid_graphics_init(sysinfo_t *sysinfo);

static bool hybrid_graphics_installed(void)
{
	if (get_gpio(HYBRID_DETECT_GPIO))
		return false;
	else
		return true;
}

void hybrid_graphics_init(sysinfo_t *sysinfo)
{
	/* Set default mode */
	uint8_t hybrid_graphics_mode =
	    HYBRID_GRAPHICS_INTEGRATED_ONLY;

	if (!hybrid_graphics_installed()) {
		printk(BIOS_DEBUG, "Hybrid graphics not installed.\n");
		/* The display is not connected to a mux or switchable. */
		sysinfo->enable_igd = 1;
		sysinfo->enable_peg = 0;
		return;
	}

	printk(BIOS_DEBUG, "Hybrid graphics available: ");
	get_option(&hybrid_graphics_mode, "hybrid_graphics_mode");

	/* Select appropriate hybrid graphics device */
	switch (hybrid_graphics_mode) {
	default:
	case HYBRID_GRAPHICS_INTEGRATED_ONLY:
		printk(BIOS_DEBUG, "Activating Integrated Only.\n");
		set_gpio(MUX_GPIO, GPIO_LEVEL_LOW);
		set_gpio(BCL_CTL_GPIO, GPIO_LEVEL_LOW);
		set_gpio(GFX_PWR_EN_GPIO, GPIO_LEVEL_LOW);

		sysinfo->enable_igd = 1;
		sysinfo->enable_peg = 0;
		break;
	case HYBRID_GRAPHICS_DISCRETE_ONLY:
		printk(BIOS_DEBUG, "Activating Discrete Only.\n");
		set_gpio(MUX_GPIO, GPIO_LEVEL_HIGH);
		set_gpio(BCL_CTL_GPIO, GPIO_LEVEL_HIGH);
		set_gpio(GFX_PWR_EN_GPIO, GPIO_LEVEL_HIGH);

		sysinfo->enable_igd = 0;
		sysinfo->enable_peg = 1;
		break;
	case HYBRID_GRAPHICS_SWITCHABLE:
		printk(BIOS_DEBUG, "Activating Switchable (both GPUs).\n");
		set_gpio(MUX_GPIO, GPIO_LEVEL_LOW);
		set_gpio(BCL_CTL_GPIO, GPIO_LEVEL_LOW);
		set_gpio(GFX_PWR_EN_GPIO, GPIO_LEVEL_HIGH);

		sysinfo->enable_igd = 1;
		sysinfo->enable_peg = 1;
		break;
	}
}
