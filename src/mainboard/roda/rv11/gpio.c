/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 secunet Security Networks AG
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

#include <southbridge/intel/common/gpio.h>

/*
 * TODO: Investigate somehow... Current values are taken from a running
 *       system with vendor supplied firmware.
 */
static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_GPIO,   /* strap */
	.gpio1  = GPIO_MODE_GPIO,   /* EC SMI# */
	.gpio2  = GPIO_MODE_GPIO,   /* strap */
	.gpio3  = GPIO_MODE_GPIO,   /* strapped weak high */
	.gpio4  = GPIO_MODE_GPIO,   /* DDR3 thermal ALERT# */
	.gpio5  = GPIO_MODE_GPIO,   /* DDR3 thermal THERM# */
	.gpio6  = GPIO_MODE_GPIO,   /* DGPU hotplug? */
	.gpio7  = GPIO_MODE_GPIO,   /* EC SCI# */
	.gpio8  = GPIO_MODE_GPIO,   /* strap */
	.gpio9  = GPIO_MODE_NATIVE, /* USB OC #5 */
	.gpio10 = GPIO_MODE_NATIVE, /* USB OC #6; strapped weak high */
	.gpio11 = GPIO_MODE_GPIO,   /* strapped weak high */
	.gpio12 = GPIO_MODE_NATIVE, /* LAN PHY Power Control */
	.gpio13 = GPIO_MODE_NATIVE, /* HDA Audio Dock Reset */
	.gpio14 = GPIO_MODE_GPIO,   /* EC wake SCI# */
	.gpio15 = GPIO_MODE_GPIO,   /* strapped high */
	.gpio16 = GPIO_MODE_NATIVE, /* SATA 4 GP */
	.gpio17 = GPIO_MODE_GPIO,   /* strapped weak low */
	.gpio18 = GPIO_MODE_NATIVE, /* PCIe clock request 1 */
	.gpio19 = GPIO_MODE_NATIVE, /* SATA 9 GP */
	.gpio20 = GPIO_MODE_NATIVE, /* PCIe clock request 2 */
	.gpio21 = GPIO_MODE_NATIVE, /* SATA 0 GP */
	.gpio22 = GPIO_MODE_GPIO,   /* strap */
	.gpio23 = GPIO_MODE_NATIVE, /* LPC DMA request 1; strapped weak high */
	.gpio24 = GPIO_MODE_GPIO,   /* strapped weak high */
	.gpio25 = GPIO_MODE_NATIVE, /* PCIe clock request 3 */
	.gpio26 = GPIO_MODE_NATIVE, /* PCIe clock request 4 */
	.gpio27 = GPIO_MODE_GPIO,   /* SATA power (active low) */
	.gpio28 = GPIO_MODE_GPIO,   /* PCH strap ODVR, Output LOW */
	.gpio29 = GPIO_MODE_GPIO,   /* Sleep LAN power (sleep low) */
	.gpio30 = GPIO_MODE_NATIVE, /* Suspend Warning */
	.gpio31 = GPIO_MODE_NATIVE, /* AC present */
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio1  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio2  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio3  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio4  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio5  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio6  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio7  = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio8  = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio9  = GPIO_DIR_INPUT,  /* Native */
	.gpio10 = GPIO_DIR_INPUT,  /* Native */
	.gpio11 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio12 = GPIO_DIR_INPUT,  /* Native */
	.gpio13 = GPIO_DIR_INPUT,  /* Native */
	.gpio14 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio15 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio16 = GPIO_DIR_INPUT,  /* Native */
	.gpio17 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio18 = GPIO_DIR_INPUT,  /* Native */
	.gpio19 = GPIO_DIR_INPUT,  /* Native */
	.gpio20 = GPIO_DIR_INPUT,  /* Native */
	.gpio21 = GPIO_DIR_INPUT,  /* Native */
	.gpio22 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio23 = GPIO_DIR_INPUT,  /* Native */
	.gpio24 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio25 = GPIO_DIR_INPUT,  /* Native */
	.gpio26 = GPIO_DIR_INPUT,  /* Native */
	.gpio27 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio28 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio29 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio30 = GPIO_DIR_INPUT,  /* Native */
	.gpio31 = GPIO_DIR_INPUT,  /* Native */
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio1  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio2  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio3  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio4  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio5  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio6  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio7  = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio8  = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio9  = GPIO_LEVEL_LOW,  /* Native */
	.gpio10 = GPIO_LEVEL_LOW,  /* Native */
	.gpio11 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio12 = GPIO_LEVEL_LOW,  /* Native */
	.gpio13 = GPIO_LEVEL_LOW,  /* Native */
	.gpio14 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio15 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio16 = GPIO_LEVEL_LOW,  /* Native */
	.gpio17 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio18 = GPIO_LEVEL_LOW,  /* Native */
	.gpio19 = GPIO_LEVEL_LOW,  /* Native */
	.gpio20 = GPIO_LEVEL_LOW,  /* Native */
	.gpio21 = GPIO_LEVEL_LOW,  /* Native */
	.gpio22 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio23 = GPIO_LEVEL_LOW,  /* Native */
	.gpio24 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio25 = GPIO_LEVEL_LOW,  /* Native */
	.gpio26 = GPIO_LEVEL_LOW,  /* Native */
	.gpio27 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio28 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio29 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio30 = GPIO_LEVEL_LOW,  /* Native */
	.gpio31 = GPIO_LEVEL_LOW,  /* Native */
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
	.gpio0  = GPIO_RESET_PWROK,
	.gpio1  = GPIO_RESET_PWROK,
	.gpio2  = GPIO_RESET_PWROK,
	.gpio3  = GPIO_RESET_PWROK,
	.gpio4  = GPIO_RESET_PWROK,
	.gpio5  = GPIO_RESET_PWROK,
	.gpio6  = GPIO_RESET_PWROK,
	.gpio7  = GPIO_RESET_PWROK,
	.gpio8  = GPIO_RESET_PWROK,
	.gpio9  = GPIO_RESET_PWROK,
	.gpio10 = GPIO_RESET_PWROK,
	.gpio11 = GPIO_RESET_PWROK,
	.gpio12 = GPIO_RESET_PWROK,
	.gpio13 = GPIO_RESET_PWROK,
	.gpio14 = GPIO_RESET_PWROK,
	.gpio15 = GPIO_RESET_PWROK,
	.gpio16 = GPIO_RESET_PWROK,
	.gpio17 = GPIO_RESET_PWROK,
	.gpio18 = GPIO_RESET_PWROK,
	.gpio19 = GPIO_RESET_PWROK,
	.gpio20 = GPIO_RESET_PWROK,
	.gpio21 = GPIO_RESET_PWROK,
	.gpio22 = GPIO_RESET_PWROK,
	.gpio23 = GPIO_RESET_PWROK,
	.gpio24 = GPIO_RESET_PWROK,
	.gpio25 = GPIO_RESET_PWROK,
	.gpio26 = GPIO_RESET_PWROK,
	.gpio27 = GPIO_RESET_PWROK,
	.gpio28 = GPIO_RESET_PWROK,
	.gpio29 = GPIO_RESET_PWROK,
	.gpio30 = GPIO_RESET_RSMRST,
	.gpio31 = GPIO_RESET_PWROK,
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio0  = GPIO_NO_INVERT,
	.gpio1  = GPIO_INVERT,
	.gpio2  = GPIO_NO_INVERT,
	.gpio3  = GPIO_INVERT,
	.gpio4  = GPIO_NO_INVERT,
	.gpio5  = GPIO_NO_INVERT,
	.gpio6  = GPIO_NO_INVERT,
	.gpio7  = GPIO_INVERT,
	.gpio8  = GPIO_NO_INVERT,
	.gpio9  = GPIO_NO_INVERT,
	.gpio10 = GPIO_NO_INVERT,
	.gpio11 = GPIO_NO_INVERT,
	.gpio12 = GPIO_NO_INVERT,
	.gpio13 = GPIO_NO_INVERT,
	.gpio14 = GPIO_INVERT,
	.gpio15 = GPIO_INVERT,
	.gpio16 = GPIO_NO_INVERT,
	.gpio17 = GPIO_NO_INVERT,
	.gpio18 = GPIO_NO_INVERT,
	.gpio19 = GPIO_NO_INVERT,
	.gpio20 = GPIO_NO_INVERT,
	.gpio21 = GPIO_NO_INVERT,
	.gpio22 = GPIO_NO_INVERT,
	.gpio23 = GPIO_NO_INVERT,
	.gpio24 = GPIO_NO_INVERT,
	.gpio25 = GPIO_NO_INVERT,
	.gpio26 = GPIO_NO_INVERT,
	.gpio27 = GPIO_NO_INVERT,
	.gpio28 = GPIO_NO_INVERT,
	.gpio29 = GPIO_NO_INVERT,
	.gpio30 = GPIO_NO_INVERT,
	.gpio31 = GPIO_NO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
	.gpio0  = GPIO_NO_BLINK,
	.gpio1  = GPIO_NO_BLINK,
	.gpio2  = GPIO_NO_BLINK,
	.gpio3  = GPIO_NO_BLINK,
	.gpio4  = GPIO_NO_BLINK,
	.gpio5  = GPIO_NO_BLINK,
	.gpio6  = GPIO_NO_BLINK,
	.gpio7  = GPIO_NO_BLINK,
	.gpio8  = GPIO_NO_BLINK,
	.gpio9  = GPIO_NO_BLINK,
	.gpio10 = GPIO_NO_BLINK,
	.gpio11 = GPIO_NO_BLINK,
	.gpio12 = GPIO_NO_BLINK,
	.gpio13 = GPIO_NO_BLINK,
	.gpio14 = GPIO_NO_BLINK,
	.gpio15 = GPIO_NO_BLINK,
	.gpio16 = GPIO_NO_BLINK,
	.gpio17 = GPIO_NO_BLINK,
	.gpio18 = GPIO_NO_BLINK,
	.gpio19 = GPIO_NO_BLINK,
	.gpio20 = GPIO_NO_BLINK,
	.gpio21 = GPIO_NO_BLINK,
	.gpio22 = GPIO_NO_BLINK,
	.gpio23 = GPIO_NO_BLINK,
	.gpio24 = GPIO_NO_BLINK,
	.gpio25 = GPIO_NO_BLINK,
	.gpio26 = GPIO_NO_BLINK,
	.gpio27 = GPIO_NO_BLINK,
	.gpio28 = GPIO_NO_BLINK,
	.gpio29 = GPIO_NO_BLINK,
	.gpio30 = GPIO_NO_BLINK,
	.gpio31 = GPIO_NO_BLINK,
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE, /* Native */
	.gpio33 = GPIO_MODE_NATIVE, /* Native */
	.gpio34 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio35 = GPIO_MODE_NATIVE, /* Native */
	.gpio36 = GPIO_MODE_NATIVE, /* Native */
	.gpio37 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio38 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio39 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio40 = GPIO_MODE_NATIVE, /* USB OC #1 */
	.gpio41 = GPIO_MODE_NATIVE, /* USB OC #2 */
	.gpio42 = GPIO_MODE_NATIVE, /* USB OC #3 */
	.gpio43 = GPIO_MODE_NATIVE, /* USB OC #4 */
	.gpio44 = GPIO_MODE_NATIVE, /* Native */
	.gpio45 = GPIO_MODE_NATIVE, /* Native */
	.gpio46 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio47 = GPIO_MODE_NATIVE, /* Native */
	.gpio48 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio49 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio50 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio51 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio52 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio53 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio54 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio55 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio56 = GPIO_MODE_NATIVE, /* Native */
	.gpio57 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio58 = GPIO_MODE_NATIVE, /* Native */
	.gpio59 = GPIO_MODE_NATIVE, /* USB OC #0 */
	.gpio60 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio61 = GPIO_MODE_NATIVE, /* Native */
	.gpio62 = GPIO_MODE_NATIVE, /* Native */
	.gpio63 = GPIO_MODE_NATIVE, /* Native */
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,  /* Native */
	.gpio33 = GPIO_DIR_INPUT,  /* Native */
	.gpio34 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio35 = GPIO_DIR_INPUT,  /* Native */
	.gpio36 = GPIO_DIR_INPUT,  /* Native */
	.gpio37 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio38 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio39 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio40 = GPIO_DIR_INPUT,  /* Native */
	.gpio41 = GPIO_DIR_INPUT,  /* Native */
	.gpio42 = GPIO_DIR_INPUT,  /* Native */
	.gpio43 = GPIO_DIR_INPUT,  /* Native */
	.gpio44 = GPIO_DIR_INPUT,  /* Native */
	.gpio45 = GPIO_DIR_INPUT,  /* Native */
	.gpio46 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio47 = GPIO_DIR_INPUT,  /* Native */
	.gpio48 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio49 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio50 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio51 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio52 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio53 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio54 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio55 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio56 = GPIO_DIR_INPUT,  /* Native */
	.gpio57 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio58 = GPIO_DIR_INPUT,  /* Native */
	.gpio59 = GPIO_DIR_INPUT,  /* Native */
	.gpio60 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio61 = GPIO_DIR_INPUT,  /* Native */
	.gpio62 = GPIO_DIR_INPUT,  /* Native */
	.gpio63 = GPIO_DIR_INPUT,  /* Native */
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_LOW,  /* Native */
	.gpio33 = GPIO_LEVEL_LOW,  /* Native */
	.gpio34 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio35 = GPIO_LEVEL_LOW,  /* Native */
	.gpio36 = GPIO_LEVEL_LOW,  /* Native */
	.gpio37 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio38 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio39 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio40 = GPIO_LEVEL_LOW,  /* Native */
	.gpio41 = GPIO_LEVEL_LOW,  /* Native */
	.gpio42 = GPIO_LEVEL_LOW,  /* Native */
	.gpio43 = GPIO_LEVEL_LOW,  /* Native */
	.gpio44 = GPIO_LEVEL_LOW,  /* Native */
	.gpio45 = GPIO_LEVEL_LOW,  /* Native */
	.gpio46 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio47 = GPIO_LEVEL_LOW,  /* Native */
	.gpio48 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio49 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio50 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio51 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio52 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio53 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio54 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio55 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio56 = GPIO_LEVEL_LOW,  /* Native */
	.gpio57 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio58 = GPIO_LEVEL_LOW,  /* Native */
	.gpio59 = GPIO_LEVEL_LOW,  /* Native */
	.gpio60 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio61 = GPIO_LEVEL_LOW,  /* Native */
	.gpio62 = GPIO_LEVEL_LOW,  /* Native */
	.gpio63 = GPIO_LEVEL_LOW,  /* Native */
};

static const struct pch_gpio_set2 pch_gpio_set2_reset = {
	.gpio32 = GPIO_RESET_PWROK,
	.gpio33 = GPIO_RESET_PWROK,
	.gpio34 = GPIO_RESET_PWROK,
	.gpio35 = GPIO_RESET_PWROK,
	.gpio36 = GPIO_RESET_PWROK,
	.gpio37 = GPIO_RESET_PWROK,
	.gpio38 = GPIO_RESET_PWROK,
	.gpio39 = GPIO_RESET_PWROK,
	.gpio40 = GPIO_RESET_PWROK,
	.gpio41 = GPIO_RESET_PWROK,
	.gpio42 = GPIO_RESET_PWROK,
	.gpio43 = GPIO_RESET_PWROK,
	.gpio44 = GPIO_RESET_PWROK,
	.gpio45 = GPIO_RESET_PWROK,
	.gpio46 = GPIO_RESET_PWROK,
	.gpio47 = GPIO_RESET_PWROK,
	.gpio48 = GPIO_RESET_PWROK,
	.gpio49 = GPIO_RESET_PWROK,
	.gpio50 = GPIO_RESET_PWROK,
	.gpio51 = GPIO_RESET_PWROK,
	.gpio52 = GPIO_RESET_PWROK,
	.gpio53 = GPIO_RESET_PWROK,
	.gpio54 = GPIO_RESET_PWROK,
	.gpio55 = GPIO_RESET_PWROK,
	.gpio56 = GPIO_RESET_PWROK,
	.gpio57 = GPIO_RESET_PWROK,
	.gpio58 = GPIO_RESET_PWROK,
	.gpio59 = GPIO_RESET_PWROK,
	.gpio60 = GPIO_RESET_PWROK,
	.gpio61 = GPIO_RESET_PWROK,
	.gpio62 = GPIO_RESET_PWROK,
	.gpio63 = GPIO_RESET_PWROK,
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NATIVE, /* Native */
	.gpio65 = GPIO_MODE_NATIVE, /* Native */
	.gpio66 = GPIO_MODE_GPIO,   /* Unknown Output LOW */
	.gpio67 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio68 = GPIO_MODE_GPIO,   /* Unknown Output HIGH */
	.gpio69 = GPIO_MODE_GPIO,   /* Unknown Input */
	.gpio70 = GPIO_MODE_NATIVE, /* Native */
	.gpio71 = GPIO_MODE_NATIVE, /* Native */
	.gpio72 = GPIO_MODE_NATIVE, /* Native */
	.gpio73 = GPIO_MODE_NATIVE, /* Native */
	.gpio74 = GPIO_MODE_NATIVE, /* Native */
	.gpio75 = GPIO_MODE_NATIVE, /* Native */
};

static const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_INPUT,  /* Native */
	.gpio65 = GPIO_DIR_INPUT,  /* Native */
	.gpio66 = GPIO_DIR_OUTPUT, /* Unknown Output LOW */
	.gpio67 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio68 = GPIO_DIR_OUTPUT, /* Unknown Output HIGH */
	.gpio69 = GPIO_DIR_INPUT,  /* Unknown Input */
	.gpio70 = GPIO_DIR_INPUT,  /* Native */
	.gpio71 = GPIO_DIR_INPUT,  /* Native */
	.gpio72 = GPIO_DIR_INPUT,  /* Native */
	.gpio73 = GPIO_DIR_INPUT,  /* Native */
	.gpio74 = GPIO_DIR_INPUT,  /* Native */
	.gpio75 = GPIO_DIR_INPUT,  /* Native */
};

static const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_LOW,  /* Native */
	.gpio65 = GPIO_LEVEL_LOW,  /* Native */
	.gpio66 = GPIO_LEVEL_LOW,  /* Unknown Output LOW */
	.gpio67 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio68 = GPIO_LEVEL_HIGH, /* Unknown Output HIGH */
	.gpio69 = GPIO_LEVEL_LOW,  /* Unknown Input */
	.gpio70 = GPIO_LEVEL_LOW,  /* Native */
	.gpio71 = GPIO_LEVEL_LOW,  /* Native */
	.gpio72 = GPIO_LEVEL_LOW,  /* Native */
	.gpio73 = GPIO_LEVEL_LOW,  /* Native */
	.gpio74 = GPIO_LEVEL_LOW,  /* Native */
	.gpio75 = GPIO_LEVEL_LOW,  /* Native */
};

static const struct pch_gpio_set3 pch_gpio_set3_reset = {
	.gpio64 = GPIO_RESET_PWROK,
	.gpio65 = GPIO_RESET_PWROK,
	.gpio66 = GPIO_RESET_PWROK,
	.gpio67 = GPIO_RESET_PWROK,
	.gpio68 = GPIO_RESET_PWROK,
	.gpio69 = GPIO_RESET_PWROK,
	.gpio70 = GPIO_RESET_PWROK,
	.gpio71 = GPIO_RESET_PWROK,
	.gpio72 = GPIO_RESET_PWROK,
	.gpio73 = GPIO_RESET_PWROK,
	.gpio74 = GPIO_RESET_PWROK,
	.gpio75 = GPIO_RESET_PWROK,
};

const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.reset		= &pch_gpio_set1_reset,
		.invert		= &pch_gpio_set1_invert,
		.blink		= &pch_gpio_set1_blink,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
		.reset		= &pch_gpio_set2_reset,
	},
	.set3 = {
		.mode		= &pch_gpio_set3_mode,
		.direction	= &pch_gpio_set3_direction,
		.level		= &pch_gpio_set3_level,
		.reset		= &pch_gpio_set3_reset,
	},
};
