/*
 * This file is part of the coreboot project.
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

static const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_GPIO,   // -USB30_SMIB - input
	.gpio1  = GPIO_MODE_GPIO,   // -EC_SCI - input
	.gpio2  = GPIO_MODE_GPIO,   // -LCD_PRESENCE - input
	.gpio3  = GPIO_MODE_GPIO,   // DOCKID0 - input
	.gpio4  = GPIO_MODE_GPIO,   // DOCKID1 - input
	.gpio5  = GPIO_MODE_GPIO,   // DOCKID2 - input
	.gpio6  = GPIO_MODE_GPIO,   // SYSTEM_DP_HPD - input
	.gpio7  = GPIO_MODE_GPIO,   // pullup
	.gpio8  = GPIO_MODE_GPIO,   // pulldown - INTEGRATED ENABLED(FCIM) 0 / DISABLED (BTM) 1
	.gpio9  = GPIO_MODE_NATIVE, // OC5 - -USB_PORT9_OC5 - input
	.gpio10 = GPIO_MODE_GPIO,   // DRAMRST_GATE_ON - output
	.gpio11 = GPIO_MODE_NATIVE, // SMBALERT# pullup
	.gpio12 = GPIO_MODE_NATIVE, // LANPHYPC - output
	.gpio13 = GPIO_MODE_GPIO,   // -EC_WAKE - input
	.gpio14 = GPIO_MODE_NATIVE, // OC7 - pullup
	.gpio15 = GPIO_MODE_GPIO,   // pullup - ME CRYPTO STRAP WITH TLS CONFIDENTIALITY
	.gpio16 = GPIO_MODE_NATIVE, // SATA4GP - SATA_DOCK_DTCT - input from gpio33
	.gpio17 = GPIO_MODE_GPIO,   // DGFX_PW RGD - input
	.gpio18 = GPIO_MODE_NATIVE, // PCIECLKRQ1 - -CLKREQ_WLAN - input
	.gpio19 = GPIO_MODE_NATIVE, // SATA1GP - SATA_BAY_DTCT - input to gpio22
	.gpio20 = GPIO_MODE_NATIVE, // PCIECLKRQ2 - pullup
	.gpio21 = GPIO_MODE_GPIO,   // -DISCRETE_GFX_PRESENCE - input
	.gpio22 = GPIO_MODE_GPIO,   // SATA_BAY_DTCT - output to SATA1GP
	.gpio23 = GPIO_MODE_NATIVE, // LDRQ1 - TP84
	.gpio24 = GPIO_MODE_GPIO,   // pullup
	.gpio25 = GPIO_MODE_NATIVE, // PCIECLKRQ3 - -CLKREQ_EXC - input
	.gpio26 = GPIO_MODE_NATIVE, // PCIECLKRQ4 - pullup
	.gpio27 = GPIO_MODE_GPIO,   // -MSATA_DTCT - input
	.gpio28 = GPIO_MODE_GPIO,   // pulldown possible
	.gpio29 = GPIO_MODE_GPIO,   // SLP_LAN - -PCH_SLP_LAN
	.gpio30 = GPIO_MODE_NATIVE, // SUSPWRDNACK - output
	.gpio31 = GPIO_MODE_NATIVE, // ACPRESENT - input
};

static const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0  = GPIO_DIR_INPUT,
	.gpio1  = GPIO_DIR_INPUT,
	.gpio2  = GPIO_DIR_INPUT,
	.gpio3  = GPIO_DIR_INPUT,
	.gpio4  = GPIO_DIR_INPUT,
	.gpio5  = GPIO_DIR_INPUT,
	.gpio6  = GPIO_DIR_INPUT,
	.gpio7  = GPIO_DIR_INPUT,
	.gpio8  = GPIO_DIR_OUTPUT,
	.gpio9  = GPIO_DIR_INPUT,
	.gpio10 = GPIO_DIR_OUTPUT,
	.gpio11 = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_OUTPUT,
	.gpio13 = GPIO_DIR_INPUT,
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_OUTPUT,
	.gpio16 = GPIO_DIR_INPUT,
	.gpio17 = GPIO_DIR_INPUT,
	.gpio18 = GPIO_DIR_INPUT,
	.gpio19 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_INPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_OUTPUT,
	.gpio23 = GPIO_DIR_INPUT,
	.gpio24 = GPIO_DIR_OUTPUT,
	.gpio25 = GPIO_DIR_INPUT,
	.gpio26 = GPIO_DIR_INPUT,
	.gpio27 = GPIO_DIR_OUTPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
	.gpio30 = GPIO_DIR_OUTPUT,
	.gpio31 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio0  = GPIO_LEVEL_HIGH,
	.gpio1  = GPIO_LEVEL_HIGH,
	.gpio2  = GPIO_LEVEL_LOW,
	.gpio3  = GPIO_LEVEL_HIGH,
	.gpio4  = GPIO_LEVEL_HIGH,
	.gpio5  = GPIO_LEVEL_HIGH,
	.gpio6  = GPIO_LEVEL_LOW,
	.gpio7  = GPIO_LEVEL_HIGH,
	.gpio8  = GPIO_LEVEL_HIGH,
	.gpio9  = GPIO_LEVEL_HIGH,
	.gpio10 = GPIO_LEVEL_HIGH,
	.gpio11 = GPIO_LEVEL_HIGH,
	.gpio12 = GPIO_LEVEL_HIGH,
	.gpio13 = GPIO_LEVEL_HIGH,
	.gpio14 = GPIO_LEVEL_HIGH,
	.gpio15 = GPIO_LEVEL_LOW,
	.gpio16 = GPIO_LEVEL_HIGH,
	.gpio17 = GPIO_LEVEL_LOW,
	.gpio18 = GPIO_LEVEL_HIGH,
	.gpio19 = GPIO_LEVEL_LOW,
	.gpio20 = GPIO_LEVEL_HIGH,
	.gpio21 = GPIO_LEVEL_LOW,
	.gpio22 = GPIO_LEVEL_LOW,
	.gpio23 = GPIO_LEVEL_HIGH,
	.gpio24 = GPIO_LEVEL_LOW,
	.gpio25 = GPIO_LEVEL_HIGH,
	.gpio26 = GPIO_LEVEL_HIGH,
	.gpio27 = GPIO_LEVEL_HIGH,
	.gpio28 = GPIO_LEVEL_LOW,
	.gpio29 = GPIO_LEVEL_HIGH,
	.gpio30 = GPIO_LEVEL_HIGH,
	.gpio31 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio1  = GPIO_INVERT,
	.gpio7  = GPIO_INVERT,
	.gpio13 = GPIO_INVERT,
};

static const struct pch_gpio_set1 pch_gpio_set1_blink = {
	.gpio18 = GPIO_NO_BLINK,
};

static const struct pch_gpio_set1 pch_gpio_set1_reset = {
	.gpio24 = GPIO_RESET_RSMRST,
};

static const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE, // CLKRUN - output
	.gpio33 = GPIO_MODE_GPIO,   // SATA_DOCK_DTCT - output to SATA4GP
	.gpio34 = GPIO_MODE_GPIO,   // DGFX_VRM_ID - input - HIGH: 1GB / LOW: 2GB
	.gpio35 = GPIO_MODE_GPIO,   // pullup
	.gpio36 = GPIO_MODE_GPIO,   // pulldown
	.gpio37 = GPIO_MODE_NATIVE, // SATA2GP - eSATA_DTCT - input to gpio
	.gpio38 = GPIO_MODE_GPIO,   // planarid2 - input - pulldown
	.gpio39 = GPIO_MODE_GPIO,   // planarid3 - input - pulldown
	.gpio40 = GPIO_MODE_NATIVE, // OC1 - -USB_PORT1_OC1 - input
	.gpio41 = GPIO_MODE_GPIO,   // OC2 - -USB_PORTX_OC2 - input
	.gpio42 = GPIO_MODE_GPIO,   // SMB_3B_EN - output
	.gpio43 = GPIO_MODE_NATIVE, // OC4 - pullup
	.gpio44 = GPIO_MODE_NATIVE, // PCIECLKRQ5 - -CLKREQ_GBE - input
	.gpio45 = GPIO_MODE_NATIVE, // PCIECLKRQ6 - -CLKREQ_USB30_TR - input
	.gpio46 = GPIO_MODE_NATIVE, // PCIECLKRQ7 - pullup
	.gpio47 = GPIO_MODE_NATIVE, // PEG_A_CLKRQ# - input
	.gpio48 = GPIO_MODE_GPIO,   // planarid0 - input - PDV low / FVT high
	.gpio49 = GPIO_MODE_GPIO,   // planarid1 - input - pulldown
	.gpio50 = GPIO_MODE_GPIO,   // -SC_DTCT - input
	.gpio51 = GPIO_MODE_GPIO,   // pullup
	.gpio52 = GPIO_MODE_GPIO,   // OPTIMUS_ENABLE - output - high: igpu / low: dgpu
	.gpio53 = GPIO_MODE_GPIO,   // pullup
	.gpio54 = GPIO_MODE_GPIO,   // -BDC_PRESENCE - input
	.gpio55 = GPIO_MODE_GPIO,   // pullup
	.gpio56 = GPIO_MODE_NATIVE, // PEG_B_CLKRQ - pullup
	.gpio57 = GPIO_MODE_GPIO,   // -DTPM_PRESENCE - input
	.gpio58 = GPIO_MODE_NATIVE, // SML1CLK - EC_SCL2 - output
	.gpio59 = GPIO_MODE_NATIVE, // OC0 - pullup
	.gpio60 = GPIO_MODE_NATIVE, // SML0ALERT# - pullup
	.gpio61 = GPIO_MODE_NATIVE, // SUS_STAT - output
	.gpio62 = GPIO_MODE_NATIVE, // SUSCLK - output
	.gpio63 = GPIO_MODE_NATIVE, // SLP_S5 - output
};

static const struct pch_gpio_set2 pch_gpio_set2_direction = {
	.gpio32 = GPIO_DIR_INPUT,
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio34 = GPIO_DIR_INPUT,
	.gpio35 = GPIO_DIR_OUTPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio40 = GPIO_DIR_INPUT,
	.gpio41 = GPIO_DIR_OUTPUT,
	.gpio42 = GPIO_DIR_OUTPUT,
	.gpio43 = GPIO_DIR_INPUT,
	.gpio44 = GPIO_DIR_INPUT,
	.gpio45 = GPIO_DIR_INPUT,
	.gpio46 = GPIO_DIR_INPUT,
	.gpio47 = GPIO_DIR_INPUT,
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_INPUT,
	.gpio50 = GPIO_DIR_INPUT,
	.gpio51 = GPIO_DIR_OUTPUT,
	.gpio52 = GPIO_DIR_OUTPUT,
	.gpio53 = GPIO_DIR_OUTPUT,
	.gpio54 = GPIO_DIR_INPUT,
	.gpio55 = GPIO_DIR_OUTPUT,
	.gpio56 = GPIO_DIR_INPUT,
	.gpio57 = GPIO_DIR_INPUT,
	.gpio58 = GPIO_DIR_INPUT,
	.gpio59 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_INPUT,
	.gpio61 = GPIO_DIR_OUTPUT,
	.gpio62 = GPIO_DIR_OUTPUT,
	.gpio63 = GPIO_DIR_OUTPUT,
};

static const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio32 = GPIO_LEVEL_HIGH,
	.gpio33 = GPIO_LEVEL_HIGH,
	.gpio34 = GPIO_LEVEL_HIGH,
	.gpio35 = GPIO_LEVEL_LOW,
	.gpio36 = GPIO_LEVEL_LOW,
	.gpio37 = GPIO_LEVEL_LOW,
	.gpio38 = GPIO_LEVEL_HIGH,
	.gpio39 = GPIO_LEVEL_LOW,
	.gpio40 = GPIO_LEVEL_HIGH,
	.gpio41 = GPIO_LEVEL_HIGH,
	.gpio42 = GPIO_LEVEL_HIGH,
	.gpio43 = GPIO_LEVEL_HIGH,
	.gpio44 = GPIO_LEVEL_HIGH,
	.gpio45 = GPIO_LEVEL_HIGH,
	.gpio46 = GPIO_LEVEL_HIGH,
	.gpio47 = GPIO_LEVEL_HIGH,
	.gpio48 = GPIO_LEVEL_HIGH,
	.gpio49 = GPIO_LEVEL_HIGH,
	.gpio50 = GPIO_LEVEL_HIGH,
	.gpio51 = GPIO_LEVEL_HIGH,
	.gpio52 = GPIO_LEVEL_HIGH,
	.gpio53 = GPIO_LEVEL_HIGH,
	.gpio54 = GPIO_LEVEL_LOW,
	.gpio55 = GPIO_LEVEL_HIGH,
	.gpio56 = GPIO_LEVEL_HIGH,
	.gpio57 = GPIO_LEVEL_LOW,
	.gpio58 = GPIO_LEVEL_HIGH,
	.gpio59 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
	.gpio61 = GPIO_LEVEL_HIGH,
	.gpio62 = GPIO_LEVEL_HIGH,
	.gpio63 = GPIO_LEVEL_HIGH,
};

static const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NATIVE, // NC
	.gpio65 = GPIO_MODE_NATIVE, // VIDEO_CLK_27M_NSS - output
	.gpio66 = GPIO_MODE_NATIVE, // NC
	.gpio67 = GPIO_MODE_NATIVE, // VIDEO_CLK_27M_SS - output
	.gpio68 = GPIO_MODE_GPIO,   // -INT_MIC_DTCT - input
	.gpio69 = GPIO_MODE_GPIO,   // mic enable bit - low enable - pulldown
	.gpio70 = GPIO_MODE_GPIO,   // pullup
	.gpio71 = GPIO_MODE_GPIO,   // pullup
	.gpio72 = GPIO_MODE_NATIVE, // BATLOW - input
	.gpio73 = GPIO_MODE_NATIVE, // pullup
	.gpio74 = GPIO_MODE_NATIVE, // pullup
	.gpio75 = GPIO_MODE_NATIVE, // SML1DATA - EC_SDA2 - i/o
};

static const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_OUTPUT,
	.gpio65 = GPIO_DIR_OUTPUT,
	.gpio66 = GPIO_DIR_OUTPUT,
	.gpio67 = GPIO_DIR_OUTPUT,
	.gpio68 = GPIO_DIR_INPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio70 = GPIO_DIR_INPUT,
	.gpio71 = GPIO_DIR_INPUT,
	.gpio72 = GPIO_DIR_INPUT,
	.gpio73 = GPIO_DIR_INPUT,
	.gpio74 = GPIO_DIR_INPUT,
	.gpio75 = GPIO_DIR_INPUT,
};

static const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_HIGH,
	.gpio65 = GPIO_LEVEL_HIGH,
	.gpio66 = GPIO_LEVEL_HIGH,
	.gpio67 = GPIO_LEVEL_HIGH,
	.gpio68 = GPIO_LEVEL_LOW,
	.gpio69 = GPIO_LEVEL_LOW,
	.gpio70 = GPIO_LEVEL_LOW,
	.gpio71 = GPIO_LEVEL_HIGH,
	.gpio72 = GPIO_LEVEL_HIGH,
	.gpio73 = GPIO_LEVEL_HIGH,
	.gpio74 = GPIO_LEVEL_HIGH,
	.gpio75 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.invert		= &pch_gpio_set1_invert,
		.blink		= &pch_gpio_set1_blink,
		.reset		= &pch_gpio_set1_reset,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
	},
	.set3 = {
		.mode		= &pch_gpio_set3_mode,
		.direction	= &pch_gpio_set3_direction,
		.level		= &pch_gpio_set3_level,
	},
};
