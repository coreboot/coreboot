/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio1 = GPIO_MODE_GPIO, /* DASP_BAY# */
	.gpio6 = GPIO_MODE_GPIO, /* LEGACYIO# */
	.gpio7 = GPIO_MODE_GPIO, /* BDC_PRESENCE# */
	.gpio8 = GPIO_MODE_GPIO, /* H8_WAKE# */
	.gpio9 = GPIO_MODE_GPIO, /* Pulled Up */
	.gpio10 = GPIO_MODE_GPIO, /* MDI_DETECT */
	.gpio12 = GPIO_MODE_GPIO, /* H8SCI# */
	.gpio13 = GPIO_MODE_GPIO, /* Pulled Up */
	.gpio14 = GPIO_MODE_GPIO, /* CPUSB# */
	.gpio15 = GPIO_MODE_GPIO, /* CPPE# */
	.gpio19 = GPIO_MODE_GPIO, /* GBE_RS# */
#if CONFIG(BOARD_LENOVO_R60)
	.gpio21 = GPIO_MODE_GPIO, /* LCD_PRESENCE# */
#endif
	.gpio22 = GPIO_MODE_GPIO, /* FWH_WP# */
	.gpio24 = GPIO_MODE_GPIO, /* Pulled Down */
	.gpio25 = GPIO_MODE_GPIO, /* MDC_KILL# */
	.gpio26 = GPIO_MODE_GPIO, /* Pulled Down */
	.gpio27 = GPIO_MODE_GPIO, /* EXC_PWR_CTRL */
	.gpio28 = GPIO_MODE_GPIO, /* EXC_AUX_CTRL */
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio1 = GPIO_DIR_INPUT,
	.gpio6 = GPIO_DIR_INPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_INPUT,
	.gpio9 = GPIO_DIR_INPUT,
	.gpio10 = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_INPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_INPUT,
	.gpio19 = GPIO_DIR_OUTPUT,
#if CONFIG(BOARD_LENOVO_R60)
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_OUTPUT,
#else
	.gpio22 = GPIO_DIR_INPUT,
#endif
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio25 = GPIO_DIR_OUTPUT,
	.gpio26 = GPIO_DIR_OUTPUT,
	.gpio27 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio19 = GPIO_LEVEL_HIGH,
	.gpio24 = GPIO_LEVEL_HIGH,
	.gpio25 = GPIO_LEVEL_HIGH,
	.gpio26 = GPIO_LEVEL_LOW,
	.gpio27 = GPIO_LEVEL_HIGH,
	.gpio28 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio1 = GPIO_INVERT,
	.gpio6 = GPIO_INVERT,
	.gpio7 = GPIO_INVERT,
	.gpio8 = GPIO_INVERT,
	.gpio12 = GPIO_INVERT,
	.gpio13 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
#if CONFIG(BOARD_LENOVO_R60)
	.gpio34 = GPIO_MODE_GPIO, /* SMB_3B_EN */
#endif
	.gpio36 = GPIO_MODE_GPIO, /* PLANARID0 */
	.gpio37 = GPIO_MODE_GPIO, /* PLANARID1 */
	.gpio38 = GPIO_MODE_GPIO, /* PLANARID2 */
	.gpio39 = GPIO_MODE_GPIO, /* PLANARID3 */
	.gpio48 = GPIO_MODE_GPIO, /* FWH_TBL# */
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
#if CONFIG(BOARD_LENOVO_R60)
	.gpio34 = GPIO_DIR_INPUT,
#endif
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio48 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio48 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.blink		= &pch_gpio_set1_blink,
		.invert		= &pch_gpio_set1_invert,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
	},
};
