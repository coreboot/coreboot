/* SPDX-License-Identifier: GPL-2.0-only */

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

	bool has_panel_hybrid_gpio;
	unsigned int panel_hybrid_gpio;
	bool panel_integrated_lvl;

	bool has_backlight_gpio;
	unsigned int backlight_gpio;
	bool backlight_integrated_lvl;

	bool has_dgpu_power_gpio;
	unsigned int dgpu_power_gpio;
	bool dgpu_power_off_lvl;

	bool has_thinker1;
};

#endif /* _LENOVO_HYBRID_GRAPHICS_CHIP_H_ */
