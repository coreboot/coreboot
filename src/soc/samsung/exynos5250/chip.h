/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef CPU_SAMSUNG_EXYNOS5250_H
#define CPU_SAMSUNG_EXYNOS5250_H

#include <soc/gpio.h>

struct soc_samsung_exynos5250_config {
	/* special magic numbers! */
	int clkval_f;
	int upper_margin;
	int lower_margin;
	int vsync;
	int left_margin;
	int right_margin;
	int hsync;

	int xres;
	int yres;
	int framebuffer_bits_per_pixel;

	int usb_vbus_gpio;
	int usb_hsic_gpio;

	u32 lcdbase;
};

#endif /* CPU_SAMSUNG_EXYNOS5250_H */
