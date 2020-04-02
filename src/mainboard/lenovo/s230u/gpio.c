/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0 = GPIO_MODE_GPIO,
	.gpio1 = GPIO_MODE_GPIO,
	.gpio2 = GPIO_MODE_GPIO,  /* POUT1# (from palm sensor 1) */
	.gpio3 = GPIO_MODE_GPIO,
	.gpio4 = GPIO_MODE_GPIO,  /* POUT2# (from palm sensor 2) */
	.gpio5 = GPIO_MODE_GPIO,
	.gpio6 = GPIO_MODE_GPIO,  /* PA0_WAKEUP (To sensor hub PA0) */
	.gpio7 = GPIO_MODE_GPIO,  /* EC_SCI# (from EC) */
	.gpio8 = GPIO_MODE_GPIO,  /* EC_SMI# (from EC) */
	.gpio9 = GPIO_MODE_GPIO,
	.gpio10 = GPIO_MODE_GPIO,
	.gpio11 = GPIO_MODE_GPIO,
	.gpio12 = GPIO_MODE_GPIO,
	.gpio13 = GPIO_MODE_GPIO,
	.gpio14 = GPIO_MODE_GPIO,
	.gpio15 = GPIO_MODE_GPIO,  /* EC_LID_OUT# */
	.gpio16 = GPIO_MODE_GPIO,  /* RAM_ID2 */
	.gpio17 = GPIO_MODE_GPIO,
	.gpio20 = GPIO_MODE_GPIO,
	.gpio21 = GPIO_MODE_GPIO,
	.gpio22 = GPIO_MODE_GPIO,
	.gpio23 = GPIO_MODE_GPIO,
	.gpio24 = GPIO_MODE_GPIO,
	.gpio26 = GPIO_MODE_GPIO,
	.gpio27 = GPIO_MODE_GPIO,  /* mSATA_DET# (from WWAN/mSATA mPCIe pin 51) */
	.gpio28 = GPIO_MODE_GPIO,
	.gpio29 = GPIO_MODE_GPIO,
	.gpio30 = GPIO_MODE_GPIO,
	.gpio31 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0 = GPIO_DIR_OUTPUT,
	.gpio1 = GPIO_DIR_OUTPUT,
	.gpio2 = GPIO_DIR_INPUT,
	.gpio3 = GPIO_DIR_OUTPUT,
	.gpio4 = GPIO_DIR_INPUT,
	.gpio5 = GPIO_DIR_OUTPUT,
	.gpio6 = GPIO_DIR_INPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_INPUT,
	.gpio9 = GPIO_DIR_OUTPUT,
	.gpio10 = GPIO_DIR_OUTPUT,
	.gpio11 = GPIO_DIR_OUTPUT,
	.gpio12 = GPIO_DIR_OUTPUT,
	.gpio13 = GPIO_DIR_OUTPUT,
	.gpio14 = GPIO_DIR_OUTPUT,
	.gpio15 = GPIO_DIR_INPUT,
	.gpio16 = GPIO_DIR_INPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_OUTPUT,
	.gpio21 = GPIO_DIR_OUTPUT,
	.gpio22 = GPIO_DIR_OUTPUT,
	.gpio23 = GPIO_DIR_OUTPUT,
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio26 = GPIO_DIR_OUTPUT,
	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
	.gpio30 = GPIO_DIR_OUTPUT,
	.gpio31 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0 = GPIO_LEVEL_HIGH,
	.gpio1 = GPIO_LEVEL_HIGH,
	.gpio3 = GPIO_LEVEL_HIGH,
	.gpio5 = GPIO_LEVEL_HIGH,
	.gpio9 = GPIO_LEVEL_HIGH,
	.gpio10 = GPIO_LEVEL_HIGH,
	.gpio11 = GPIO_LEVEL_HIGH,
	.gpio12 = GPIO_LEVEL_HIGH,
	.gpio13 = GPIO_LEVEL_HIGH,
	.gpio14 = GPIO_LEVEL_HIGH,
	.gpio20 = GPIO_LEVEL_HIGH,
	.gpio21 = GPIO_LEVEL_HIGH,
	.gpio22 = GPIO_LEVEL_HIGH,
	.gpio23 = GPIO_LEVEL_HIGH,
	.gpio24 = GPIO_LEVEL_HIGH,
	.gpio26 = GPIO_LEVEL_HIGH,
	.gpio28 = GPIO_LEVEL_HIGH,
	.gpio29 = GPIO_LEVEL_LOW,
	.gpio30 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
	.gpio24 = GPIO_RESET_RSMRST,
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio2 = GPIO_INVERT,
	.gpio4 = GPIO_INVERT,
	.gpio6 = GPIO_NO_INVERT,
	.gpio7 = GPIO_INVERT,
	.gpio8 = GPIO_INVERT,
	.gpio15 = GPIO_INVERT,
	.gpio16 = GPIO_NO_INVERT,
	.gpio17 = GPIO_NO_INVERT,
	.gpio27 = GPIO_NO_INVERT,
	.gpio31 = GPIO_NO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio33 = GPIO_MODE_GPIO,  /* PCH_WLBT_OFF_5# (to WLAN mPCIe pin 5) */
	/* GPIO34 marked as PCH_BT_ON#, but is native (STP_PCI#) */
	.gpio35 = GPIO_MODE_GPIO,  /* 3G_DET# (from WWAN/mSATA mPCIe pin 43) */
	.gpio36 = GPIO_MODE_GPIO,  /* PCH_WLBT_OFF_51# (to WLAN mPCIe pin 51) */
	.gpio37 = GPIO_MODE_GPIO,
	.gpio38 = GPIO_MODE_GPIO,
	.gpio39 = GPIO_MODE_GPIO,  /* 3G_OFF# (to WWAN/mSATA mPCIe pin 20) */
	.gpio41 = GPIO_MODE_GPIO,
	.gpio42 = GPIO_MODE_GPIO,
	.gpio43 = GPIO_MODE_GPIO,
	.gpio44 = GPIO_MODE_GPIO,
	.gpio45 = GPIO_MODE_GPIO,
	.gpio46 = GPIO_MODE_GPIO,
	.gpio48 = GPIO_MODE_GPIO,  /* RAM_ID3 */
	.gpio49 = GPIO_MODE_GPIO,
	.gpio50 = GPIO_MODE_GPIO,
	.gpio51 = GPIO_MODE_GPIO,
	.gpio52 = GPIO_MODE_GPIO,
	.gpio53 = GPIO_MODE_GPIO,
	.gpio54 = GPIO_MODE_GPIO,
	/* GPIO55 marekd as WL_OFF#, but is native (GNT3#) */
	.gpio56 = GPIO_MODE_GPIO,
	.gpio57 = GPIO_MODE_GPIO,
	.gpio60 = GPIO_MODE_GPIO,  /* DRAMRST_CNTRL_PCH */
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio35 = GPIO_DIR_INPUT,
	.gpio36 = GPIO_DIR_OUTPUT,
	.gpio37 = GPIO_DIR_OUTPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_OUTPUT,
	.gpio41 = GPIO_DIR_OUTPUT,
	.gpio42 = GPIO_DIR_OUTPUT,
	.gpio43 = GPIO_DIR_OUTPUT,
	.gpio44 = GPIO_DIR_OUTPUT,
	.gpio45 = GPIO_DIR_OUTPUT,
	.gpio46 = GPIO_DIR_OUTPUT,
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_OUTPUT,
	.gpio50 = GPIO_DIR_OUTPUT,
	.gpio51 = GPIO_DIR_OUTPUT,
	.gpio52 = GPIO_DIR_OUTPUT,
	.gpio53 = GPIO_DIR_OUTPUT,
	.gpio54 = GPIO_DIR_OUTPUT,
	.gpio56 = GPIO_DIR_OUTPUT,
	.gpio57 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio33 = GPIO_LEVEL_HIGH,
	.gpio36 = GPIO_LEVEL_HIGH,
	.gpio37 = GPIO_LEVEL_HIGH,
	.gpio39 = GPIO_LEVEL_LOW,
	.gpio41 = GPIO_LEVEL_HIGH,
	.gpio42 = GPIO_LEVEL_HIGH,
	.gpio43 = GPIO_LEVEL_HIGH,
	.gpio44 = GPIO_LEVEL_HIGH,
	.gpio45 = GPIO_LEVEL_HIGH,
	.gpio46 = GPIO_LEVEL_LOW,
	.gpio49 = GPIO_LEVEL_HIGH,
	.gpio50 = GPIO_LEVEL_LOW,
	.gpio51 = GPIO_LEVEL_HIGH,
	.gpio52 = GPIO_LEVEL_HIGH,
	.gpio53 = GPIO_LEVEL_HIGH,
	.gpio54 = GPIO_LEVEL_LOW,
	.gpio56 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set2 pch_gpio_set2_reset = {
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_GPIO,
	.gpio65 = GPIO_MODE_GPIO,
	.gpio68 = GPIO_MODE_GPIO,
	.gpio69 = GPIO_MODE_GPIO,
	.gpio70 = GPIO_MODE_GPIO,  /* RAM_ID1 */
	.gpio71 = GPIO_MODE_GPIO,  /* RAM_ID0 */
	.gpio72 = GPIO_MODE_GPIO,
	.gpio74 = GPIO_MODE_GPIO,
};

static const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_OUTPUT,
	.gpio65 = GPIO_DIR_OUTPUT,
	.gpio68 = GPIO_DIR_OUTPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio70 = GPIO_DIR_INPUT,
	.gpio71 = GPIO_DIR_INPUT,
	.gpio72 = GPIO_DIR_OUTPUT,
	.gpio74 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_HIGH,
	.gpio65 = GPIO_LEVEL_HIGH,
	.gpio68 = GPIO_LEVEL_LOW,
	.gpio72 = GPIO_LEVEL_HIGH,
	.gpio74 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set3 pch_gpio_set3_reset = {
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
		.reset		= &pch_gpio_set3_reset,
	},
};
