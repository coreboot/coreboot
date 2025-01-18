/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0 = GPIO_MODE_GPIO,
	.gpio1 = GPIO_MODE_GPIO, /* SMI# of ASM1042 USB3 */
	.gpio2 = GPIO_MODE_GPIO,
	.gpio3 = GPIO_MODE_GPIO,
	.gpio4 = GPIO_MODE_GPIO,
	.gpio5 = GPIO_MODE_GPIO,
	.gpio6 = GPIO_MODE_GPIO,
	.gpio7 = GPIO_MODE_GPIO, /* PCIEX16_3 card presence, active low */
	.gpio8 = GPIO_MODE_GPIO, /* Power LED */
	.gpio9 = GPIO_MODE_NATIVE,
	.gpio10 = GPIO_MODE_NATIVE,
	.gpio11 = GPIO_MODE_GPIO,
	.gpio12 = GPIO_MODE_GPIO,
	.gpio13 = GPIO_MODE_GPIO,
	.gpio14 = GPIO_MODE_GPIO,
	.gpio15 = GPIO_MODE_GPIO,
	.gpio16 = GPIO_MODE_GPIO,
	.gpio17 = GPIO_MODE_GPIO,
	.gpio19 = GPIO_MODE_GPIO,
	.gpio20 = GPIO_MODE_GPIO,
	.gpio21 = GPIO_MODE_GPIO,
	.gpio22 = GPIO_MODE_GPIO,
	.gpio23 = GPIO_MODE_GPIO,
	.gpio24 = GPIO_MODE_GPIO,
	.gpio27 = GPIO_MODE_GPIO, /* ME_UNLOCK (Set high to unlock SPI descriptor) */
	.gpio28 = GPIO_MODE_GPIO, /* S_IVR_EN */
	.gpio29 = GPIO_MODE_GPIO, /* EC_SPI_WP# */
	.gpio30 = GPIO_MODE_NATIVE,
	.gpio31 = GPIO_MODE_GPIO, /* AI1314 BCS1 */
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0 = GPIO_DIR_INPUT,
	.gpio1 = GPIO_DIR_INPUT,
	.gpio2 = GPIO_DIR_INPUT,
	.gpio3 = GPIO_DIR_INPUT,
	.gpio4 = GPIO_DIR_INPUT,
	.gpio5 = GPIO_DIR_INPUT,
	.gpio6 = GPIO_DIR_INPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_OUTPUT,
	.gpio11 = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_INPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_INPUT,
	.gpio16 = GPIO_DIR_INPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio19 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_INPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_INPUT,
	.gpio23 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
	.gpio31 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio8 = GPIO_LEVEL_HIGH,
	.gpio28 = GPIO_LEVEL_LOW,
	.gpio29 = GPIO_LEVEL_HIGH,
	.gpio31 = GPIO_LEVEL_LOW,
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
	.gpio27 = GPIO_RESET_RSMRST
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio1 = GPIO_INVERT,
	.gpio11 = GPIO_INVERT,
	.gpio13 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_GPIO,
	.gpio33 = GPIO_MODE_GPIO,
	.gpio34 = GPIO_MODE_GPIO,
	.gpio35 = GPIO_MODE_GPIO,
	.gpio36 = GPIO_MODE_GPIO,
	.gpio37 = GPIO_MODE_GPIO,
	.gpio38 = GPIO_MODE_GPIO,
	.gpio39 = GPIO_MODE_GPIO,
	.gpio40 = GPIO_MODE_NATIVE,
	.gpio41 = GPIO_MODE_NATIVE,
	.gpio42 = GPIO_MODE_NATIVE,
	.gpio43 = GPIO_MODE_NATIVE,
	.gpio44 = GPIO_MODE_GPIO, /* EPU switch, active low */
	.gpio45 = GPIO_MODE_GPIO, /* TPU switch, active low */
	.gpio46 = GPIO_MODE_GPIO, /* QSWU1 (High = PCH PCIe lanes 3/4 goes to PCIEX16_3) */
	.gpio48 = GPIO_MODE_GPIO,
	.gpio49 = GPIO_MODE_GPIO,
	.gpio50 = GPIO_MODE_GPIO,
	.gpio51 = GPIO_MODE_GPIO,
	.gpio52 = GPIO_MODE_GPIO,
	.gpio53 = GPIO_MODE_GPIO,
	.gpio54 = GPIO_MODE_GPIO,
	.gpio55 = GPIO_MODE_GPIO,
	.gpio57 = GPIO_MODE_GPIO, /* AI1314 BCS0 */
	.gpio58 = GPIO_MODE_GPIO,
	.gpio59 = GPIO_MODE_NATIVE,
	.gpio60 = GPIO_MODE_GPIO,
	.gpio61 = GPIO_MODE_GPIO,
	.gpio62 = GPIO_MODE_GPIO,
	.gpio63 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,
	.gpio33 = GPIO_DIR_INPUT,
	.gpio34 = GPIO_DIR_INPUT,
	.gpio35 = GPIO_DIR_INPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio44 = GPIO_DIR_INPUT,
	.gpio45 = GPIO_DIR_INPUT,
	.gpio46 = GPIO_DIR_OUTPUT,
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_INPUT,
	.gpio50 = GPIO_DIR_INPUT,
	.gpio51 = GPIO_DIR_INPUT,
	.gpio52 = GPIO_DIR_INPUT,
	.gpio53 = GPIO_DIR_INPUT,
	.gpio54 = GPIO_DIR_INPUT,
	.gpio55 = GPIO_DIR_INPUT,
	.gpio57 = GPIO_DIR_OUTPUT,
	.gpio58 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_INPUT,
	.gpio61 = GPIO_DIR_INPUT,
	.gpio62 = GPIO_DIR_INPUT,
	.gpio63 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio46 = GPIO_LEVEL_LOW, /* Needs to be set at runtime */
	.gpio57 = GPIO_LEVEL_LOW,
};

static const struct pch_gpio_set2 pch_gpio_set2_reset = {
	.gpio46 = GPIO_RESET_RSMRST,
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_GPIO,
	.gpio65 = GPIO_MODE_GPIO,
	.gpio66 = GPIO_MODE_NATIVE,
	.gpio67 = GPIO_MODE_NATIVE,
	.gpio68 = GPIO_MODE_GPIO, /* RTL8111 ISOLATE# */
	.gpio69 = GPIO_MODE_GPIO, /* A BIOS flashback signal */
	.gpio70 = GPIO_MODE_GPIO,
	.gpio71 = GPIO_MODE_GPIO,
	.gpio72 = GPIO_MODE_GPIO, /* WP# of main SPI flash chip */
	.gpio74 = GPIO_MODE_GPIO,
	.gpio75 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_INPUT,
	.gpio65 = GPIO_DIR_INPUT,
	.gpio68 = GPIO_DIR_OUTPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio70 = GPIO_DIR_INPUT,
	.gpio71 = GPIO_DIR_INPUT,
	.gpio72 = GPIO_DIR_OUTPUT,
	.gpio74 = GPIO_DIR_INPUT,
	.gpio75 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio68 = GPIO_LEVEL_HIGH,
	.gpio72 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.blink		= &pch_gpio_set1_blink,
		.invert		= &pch_gpio_set1_invert,
		.reset		= &pch_gpio_set1_reset,
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
	},
};
