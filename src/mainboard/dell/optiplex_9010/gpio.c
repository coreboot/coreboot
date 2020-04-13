/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/common/gpio.h>

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0 = GPIO_MODE_NATIVE,
	.gpio1 = GPIO_MODE_GPIO,	/* CHASSIS_ID0 */
	.gpio2 = GPIO_MODE_GPIO,
	.gpio3 = GPIO_MODE_GPIO,
	.gpio4 = GPIO_MODE_GPIO,	/* VGA_CBL_DET# */
	.gpio5 = GPIO_MODE_GPIO,
	.gpio6 = GPIO_MODE_GPIO,	/* PCH_HS_DET# (unused?) */
	.gpio7 = GPIO_MODE_GPIO,	/* SKU2 */
	.gpio8 = GPIO_MODE_GPIO,
	.gpio9 = GPIO_MODE_NATIVE,
	.gpio10 = GPIO_MODE_NATIVE,
	.gpio11 = GPIO_MODE_GPIO,	/* PCIE_X4_WAKE*/
	.gpio12 = GPIO_MODE_NATIVE,
	.gpio13 = GPIO_MODE_GPIO,	/* PCIE_X1_WAKE (MT/DT only)*/
	.gpio14 = GPIO_MODE_GPIO,
	.gpio15 = GPIO_MODE_GPIO,
	.gpio16 = GPIO_MODE_GPIO,
	.gpio17 = GPIO_MODE_GPIO,	/* CHASSIS_ID1 */
	.gpio18 = GPIO_MODE_NATIVE,
	.gpio19 = GPIO_MODE_NATIVE,
	.gpio20 = GPIO_MODE_GPIO,	/* FLEXBAY_HDR_CBL_DET# */
	.gpio21 = GPIO_MODE_GPIO,	/* BOARD_REV0 */
	.gpio22 = GPIO_MODE_GPIO,
	.gpio23 = GPIO_MODE_GPIO,
	.gpio24 = GPIO_MODE_GPIO,
	.gpio25 = GPIO_MODE_NATIVE,
	.gpio26 = GPIO_MODE_NATIVE,
	.gpio27 = GPIO_MODE_GPIO,
	.gpio28 = GPIO_MODE_GPIO,
	.gpio29 = GPIO_MODE_GPIO,
	.gpio30 = GPIO_MODE_NATIVE,
	.gpio31 = GPIO_MODE_GPIO,	/* Password Clear Jumper */
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio1 = GPIO_DIR_INPUT,
	.gpio2 = GPIO_DIR_INPUT,
	.gpio3 = GPIO_DIR_INPUT,
	.gpio4 = GPIO_DIR_INPUT,
	.gpio5 = GPIO_DIR_INPUT,
	.gpio6 = GPIO_DIR_INPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_OUTPUT,
	.gpio11 = GPIO_DIR_INPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio14 = GPIO_DIR_OUTPUT,
	.gpio15 = GPIO_DIR_OUTPUT,
	.gpio16 = GPIO_DIR_INPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_INPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_INPUT,
	.gpio23 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
	.gpio31 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio8 = GPIO_LEVEL_HIGH,
	.gpio14 = GPIO_LEVEL_HIGH,
	.gpio15 = GPIO_LEVEL_LOW,
	.gpio27 = GPIO_LEVEL_LOW,
	.gpio28 = GPIO_LEVEL_LOW,
	.gpio29 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio2 = GPIO_INVERT,
	.gpio5 = GPIO_INVERT,
	.gpio6 = GPIO_INVERT,
	.gpio11 = GPIO_INVERT,
	.gpio13 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_GPIO,	/* SKU0 */
	.gpio33 = GPIO_MODE_GPIO,
	.gpio34 = GPIO_MODE_GPIO,
	.gpio35 = GPIO_MODE_GPIO,	/* SKU1 */
	.gpio36 = GPIO_MODE_NATIVE,
	.gpio37 = GPIO_MODE_NATIVE,
	.gpio38 = GPIO_MODE_GPIO,	/* CHASSIS_ID2 */
	.gpio39 = GPIO_MODE_GPIO,	/* FP_PRESENCE# */
	.gpio40 = GPIO_MODE_NATIVE,
	.gpio41 = GPIO_MODE_NATIVE,
	.gpio42 = GPIO_MODE_NATIVE,
	.gpio43 = GPIO_MODE_NATIVE,
	.gpio44 = GPIO_MODE_GPIO,	/* INTRUD_CBL_DET# */
	.gpio45 = GPIO_MODE_GPIO,	/* COM_SER2_DET# (unused?) */
	.gpio46 = GPIO_MODE_GPIO,	/* BOARD_REV1 */
	.gpio47 = GPIO_MODE_NATIVE,
	.gpio48 = GPIO_MODE_GPIO,
	.gpio49 = GPIO_MODE_GPIO,
	.gpio50 = GPIO_MODE_NATIVE,
	.gpio51 = GPIO_MODE_NATIVE,
	.gpio52 = GPIO_MODE_NATIVE,
	.gpio53 = GPIO_MODE_NATIVE,
	.gpio54 = GPIO_MODE_NATIVE,
	.gpio55 = GPIO_MODE_NATIVE,
	.gpio56 = GPIO_MODE_NATIVE,
	.gpio57 = GPIO_MODE_GPIO,
	.gpio58 = GPIO_MODE_NATIVE,
	.gpio59 = GPIO_MODE_NATIVE,
	.gpio60 = GPIO_MODE_GPIO,
	.gpio61 = GPIO_MODE_NATIVE,
	.gpio62 = GPIO_MODE_NATIVE,
	.gpio63 = GPIO_MODE_NATIVE,
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio34 = GPIO_DIR_OUTPUT,
	.gpio35 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio44 = GPIO_DIR_INPUT,
	.gpio45 = GPIO_DIR_INPUT,
	.gpio46 = GPIO_DIR_INPUT,
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_OUTPUT,
	.gpio57 = GPIO_DIR_OUTPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
	.gpio63 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio33 = GPIO_LEVEL_HIGH,
	.gpio34 = GPIO_LEVEL_HIGH,
	.gpio49 = GPIO_LEVEL_HIGH,
	.gpio57 = GPIO_LEVEL_LOW,
	.gpio60 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set2 pch_gpio_set2_reset = {
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NATIVE,
	.gpio65 = GPIO_MODE_NATIVE,
	.gpio66 = GPIO_MODE_NATIVE,
	.gpio67 = GPIO_MODE_NATIVE,
	.gpio68 = GPIO_MODE_GPIO,	/* BOARD_REV2 */
	.gpio69 = GPIO_MODE_GPIO,	/* USB_HDR_DET# */
	.gpio70 = GPIO_MODE_GPIO,	/* FP_CHAS_DET# */
	.gpio71 = GPIO_MODE_GPIO,
	.gpio72 = GPIO_MODE_GPIO,
	.gpio73 = GPIO_MODE_GPIO,
	.gpio74 = GPIO_MODE_GPIO,	/* ME_MFG_MODE */
	.gpio75 = GPIO_MODE_NATIVE,
};

static const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio68 = GPIO_DIR_INPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio70 = GPIO_DIR_INPUT,
	.gpio71 = GPIO_DIR_OUTPUT,
	.gpio72 = GPIO_DIR_OUTPUT,
	.gpio73 = GPIO_DIR_INPUT,
	.gpio74 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio71 = GPIO_LEVEL_HIGH,
	.gpio72 = GPIO_LEVEL_HIGH,
	.gpio74 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set3 pch_gpio_set3_reset = {
	.gpio74 = GPIO_RESET_RSMRST,
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
