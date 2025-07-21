/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include <option.h>
#include <southbridge/intel/common/gpio.h>
#include "chip.h"

/*
 * Switch panel mux or backlight mux to active GPU.
 * In case both GPUs are active switch panel mux to integrated.
 */
static void lenovo_hybrid_graphics_enable(struct device *dev)
{
	const struct drivers_lenovo_hybrid_graphics_config *config;
	enum hybrid_graphics_req mode;

	/* Don't confuse anyone else and disable the fake device */
	dev->enabled = 0;

	config = dev->chip_info;
	if (!config || (gpio_get(config->detect_gpio) == DGPU_NOT_INSTALLED)) {
		printk(BIOS_DEBUG, "Hybrid graphics: Not installed\n");
		return;
	}

	mode = get_uint_option("hybrid_graphics_mode", HYBRID_GRAPHICS_DEFAULT_GPU);

	if (mode == HYBRID_GRAPHICS_DISCRETE) {
		printk(BIOS_DEBUG, "Hybrid graphics:"
		       " Switching panel to discrete GPU.\n");

		if (config->has_panel_hybrid_gpio)
			set_gpio(config->panel_hybrid_gpio,
				 !config->panel_integrated_lvl);

		if (config->has_backlight_gpio)
			set_gpio(config->backlight_gpio,
				 !config->backlight_integrated_lvl);
	} else {
		printk(BIOS_DEBUG, "Hybrid graphics:"
		       " Switching panel to integrated GPU.\n");

		if (config->has_panel_hybrid_gpio)
			set_gpio(config->panel_hybrid_gpio,
				 config->panel_integrated_lvl);

		if (config->has_backlight_gpio)
			set_gpio(config->backlight_gpio,
				 config->backlight_integrated_lvl);
	}
}

struct chip_operations drivers_lenovo_hybrid_graphics_ops = {
	.name = "Lenovo hybrid graphics driver",
	.enable_dev = lenovo_hybrid_graphics_enable
};
