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

#ifndef _LENOVO_HYBRID_GRAPHICS_CHIP_H_
#define _LENOVO_HYBRID_GRAPHICS_CHIP_H_

#define HYBRID_GRAPHICS_PORT 0xff

#define HYBRID_GRAPHICS_DEVICE 0xf

enum hybrid_graphics_req {
	HYBRID_GRAPHICS_INTEGRATED = 0,
	HYBRID_GRAPHICS_DISCRETE = 1,
	HYBRID_GRAPHICS_DUAL = 2
};

enum dgpu_detect_lvl {
	DGPU_INSTALLED = 0,
	DGPU_NOT_INSTALLED = 1,
};

#define HYBRID_GRAPHICS_DEFAULT_GPU HYBRID_GRAPHICS_INTEGRATED

struct drivers_lenovo_hybrid_graphics_config {
	unsigned int detect_gpio;

	unsigned int has_panel_hybrid_gpio;
	unsigned int panel_hybrid_gpio;
	unsigned int panel_integrated_lvl;

	unsigned int has_backlight_gpio;
	unsigned int backlight_gpio;
	unsigned int backlight_integrated_lvl;

	unsigned int has_dgpu_power_gpio;
	unsigned int dgpu_power_gpio;
	unsigned int dgpu_power_off_lvl;

	unsigned int has_thinker1;
};

#endif /* _LENOVO_HYBRID_GRAPHICS_CHIP_H_ */
