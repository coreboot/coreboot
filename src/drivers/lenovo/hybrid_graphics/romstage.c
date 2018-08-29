/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Patrick Rudolph <siro@das-labor.org>
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

#include <types.h>
#include <option.h>
#include <device/device.h>

#include <southbridge/intel/common/gpio.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <console/console.h>

#include "hybrid_graphics.h"
#include "chip.h"

/*
 * Returns the hybrid graphics presence and user's card preferences.
 */
void early_hybrid_graphics(bool *enable_igd, bool *enable_peg)
{
	const struct drivers_lenovo_hybrid_graphics_config *config;
	const struct device *dev;
	enum hybrid_graphics_req mode = HYBRID_GRAPHICS_DEFAULT_GPU;

	/* TODO: Use generic device instead of dummy PNP device */
	dev = dev_find_slot_pnp(HYBRID_GRAPHICS_PORT, HYBRID_GRAPHICS_DEVICE);

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "Hybrid graphics: ERROR\n");
		*enable_igd = true;
		*enable_peg = false;
		return;
	}

	config = dev->chip_info;
	if (get_gpio(config->detect_gpio) == DGPU_NOT_INSTALLED) {
		printk(BIOS_DEBUG, "Hybrid graphics:"
		       " No discrete GPU present.\n");
		*enable_igd = true;
		*enable_peg = false;
		return;
	}

	get_option(&mode, "hybrid_graphics_mode");

	if (mode == HYBRID_GRAPHICS_DISCRETE) {
		printk(BIOS_DEBUG, "Hybrid graphics:"
		       " Disabling integrated GPU.\n");

		*enable_igd = false;
		*enable_peg = true;
	} else if (mode == HYBRID_GRAPHICS_INTEGRATED) {
		printk(BIOS_DEBUG, "Hybrid graphics:"
		       " Disabling discrete GPU.\n");

		*enable_igd = true;
		*enable_peg = false;
	} else {
		printk(BIOS_DEBUG, "Hybrid graphics:"
		       " Activating Switchable (both GPUs).\n");

		*enable_igd = true;
		*enable_peg = true;
	}

	/*
	 * Need to do power handling here as we know there's a dGPU.
	 * Support GPIO and Thinker1.
	 */
	if (config->has_dgpu_power_gpio) {
		if (*enable_peg)
			set_gpio(config->dgpu_power_gpio,
				 !config->dgpu_power_off_lvl);
		else
			set_gpio(config->dgpu_power_gpio,
				 config->dgpu_power_off_lvl);
	} else if (config->has_thinker1) {
		bool power_en = pmh7_dgpu_power_state();
		if (*enable_peg != power_en)
			pmh7_dgpu_power_enable(!power_en);
	} else {
		printk(BIOS_ERR, "Hybrid graphics:"
		       " FIXME: dGPU power handling not implemented\n");
	}
}
