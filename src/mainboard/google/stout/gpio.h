/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef STOUT_GPIO_H
#define STOUT_GPIO_H

#include "southbridge/intel/bd82x6x/gpio.h"

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0  = GPIO_MODE_GPIO,	/* GPIO0 */
	.gpio1  = GPIO_MODE_GPIO,	/* SIO_EXT_SMI# */
	.gpio2  = GPIO_MODE_NONE,	/* NOT USED */
	.gpio3  = GPIO_MODE_NONE,	/* NOT USED */
	.gpio4  = GPIO_MODE_NONE,	/* NOT USED */
	.gpio5  = GPIO_MODE_GPIO,	/* INTH# */
	.gpio6  = GPIO_MODE_GPIO,	/* SIO_EXT_SCI# */
	.gpio7  = GPIO_MODE_GPIO,	/* GE_SCR_WP# */
	.gpio8  = GPIO_MODE_NONE,	/* NOT USED */
	.gpio9  = GPIO_MODE_NATIVE,	/* USB_OC5# */
	.gpio10 = GPIO_MODE_NATIVE,	/* USB_OC6# */
	.gpio11 = GPIO_MODE_NATIVE,	/* SMBALERT# */
	.gpio12 = GPIO_MODE_GPIO,	/* GPIO12 */
	.gpio13 = GPIO_MODE_GPIO,	/* GPIO13 */
	.gpio14 = GPIO_MODE_NATIVE,	/* USB_OC7# */
	.gpio15 = GPIO_MODE_GPIO,	/* GPIO15 */
	.gpio16 = GPIO_MODE_GPIO,	/* WWAN_LED_ON */
	.gpio17 = GPIO_MODE_GPIO,	/* WLAN_LED_ON */
	.gpio18 = GPIO_MODE_NATIVE,	/* PCIE_CLKREQ_WLAN# */
	.gpio19 = GPIO_MODE_GPIO,	/* BBS_BIT0 */
	.gpio20 = GPIO_MODE_NATIVE,	/* PCIE_CLKREQ_CARD# */
	.gpio21 = GPIO_MODE_GPIO,	/* BT_DET# / TP29 */
	.gpio22 = GPIO_MODE_GPIO,	/* MODEL_ID0 */
	.gpio23 = GPIO_MODE_GPIO,	/* LCD_BK_OFF */
	.gpio24 = GPIO_MODE_NATIVE,	/* GPIO24 */
	.gpio25 = GPIO_MODE_NATIVE,	/* PCIE_REQ_WWAN# / TP89 */
	.gpio26 = GPIO_MODE_NATIVE,	/* CLK_PCIE_REQ4# / TP59 */
	.gpio27 = GPIO_MODE_GPIO,	/* MSATA_DTCT# */
	.gpio28 = GPIO_MODE_GPIO,	/* PLL_ODVR_EN */
	.gpio29 = GPIO_MODE_GPIO,	/* WLAN_AOAC_ON */
	.gpio30 = GPIO_MODE_NATIVE,	/* SUS_PWR_ACK */
	.gpio31 = GPIO_MODE_NATIVE,	/* AC_PRESENT */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	/*
	 * Note: Only gpio configured as "gpio" or "none" need to have the
	 *       direction configured.
	 */
	.gpio0  = GPIO_DIR_OUTPUT,
	.gpio1  = GPIO_DIR_INPUT,
	.gpio2  = GPIO_DIR_INPUT,
	.gpio3  = GPIO_DIR_INPUT,
	.gpio4  = GPIO_DIR_INPUT,
	.gpio5  = GPIO_DIR_OUTPUT,
	.gpio6  = GPIO_DIR_INPUT,
	.gpio7  = GPIO_DIR_INPUT,
	.gpio8  = GPIO_DIR_INPUT,

	.gpio12 = GPIO_DIR_OUTPUT,
	.gpio13 = GPIO_DIR_OUTPUT,

	.gpio15 = GPIO_DIR_INPUT,
	.gpio16 = GPIO_DIR_OUTPUT,
	.gpio17 = GPIO_DIR_OUTPUT,

	.gpio19 = GPIO_DIR_OUTPUT,

	.gpio21 = GPIO_DIR_OUTPUT,
	.gpio22 = GPIO_DIR_INPUT,
	.gpio23 = GPIO_DIR_OUTPUT,

	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_OUTPUT,
	.gpio29 = GPIO_DIR_OUTPUT,
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	/*
	 * Note: Only gpio configured as "gpio" or "none" need to have the
	 *       level set.
	 */
	.gpio0  = GPIO_LEVEL_HIGH,
	.gpio1  = GPIO_LEVEL_LOW,
	.gpio2  = GPIO_LEVEL_LOW,
	.gpio3  = GPIO_LEVEL_LOW,
	.gpio4  = GPIO_LEVEL_LOW,
	.gpio5  = GPIO_LEVEL_HIGH,
	.gpio6  = GPIO_LEVEL_LOW,
	.gpio7  = GPIO_LEVEL_HIGH,
	.gpio8  = GPIO_LEVEL_LOW,

	.gpio12 = GPIO_LEVEL_LOW,
	.gpio13 = GPIO_LEVEL_LOW,

	.gpio15 = GPIO_LEVEL_LOW,
	.gpio16 = GPIO_LEVEL_HIGH,
	.gpio17 = GPIO_LEVEL_LOW,

	.gpio19 = GPIO_LEVEL_LOW,

	.gpio21 = GPIO_LEVEL_LOW,
	.gpio22 = GPIO_LEVEL_LOW,
	.gpio23 = GPIO_LEVEL_LOW,

	.gpio27 = GPIO_LEVEL_LOW,
	.gpio28 = GPIO_LEVEL_HIGH,
	.gpio29 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_set1 pch_gpio_set1_invert = {
	.gpio1 = GPIO_INVERT,
	.gpio6 = GPIO_INVERT,
	.gpio8 = GPIO_INVERT,
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE, /* PCI_CLKRUN# */
	.gpio33 = GPIO_MODE_GPIO,   /* GPIO33 */
	.gpio34 = GPIO_MODE_GPIO,   /* CCD_ON */
	.gpio35 = GPIO_MODE_GPIO,   /* BT_ON */
	.gpio36 = GPIO_MODE_NONE,   /* NOT USED */
	.gpio37 = GPIO_MODE_NONE,   /* NOT USED */
	.gpio38 = GPIO_MODE_NONE,   /* NOT USED */
	.gpio39 = GPIO_MODE_NONE,   /* NOT USED */
	.gpio40 = GPIO_MODE_GPIO,   /* USB_OC1# */
	.gpio41 = GPIO_MODE_GPIO,   /* USB_OC2# */
	.gpio42 = GPIO_MODE_NATIVE, /* USB_OC3# */
	.gpio43 = GPIO_MODE_NATIVE, /* USB_OC4_AUO4# */
	.gpio44 = GPIO_MODE_NATIVE, /* PCIE_CLKREQ_LAN# */
	.gpio45 = GPIO_MODE_NATIVE, /* PCIECLKRQ6# / TP48 */
	.gpio46 = GPIO_MODE_NATIVE, /* PCIECLKRQ7# / TP57 */
	.gpio47 = GPIO_MODE_NATIVE, /* CLK_PEGA_REQ# */
	.gpio48 = GPIO_MODE_GPIO,   /* DIS_BT_ON# */
	.gpio49 = GPIO_MODE_GPIO,   /* GPIO49 */
	.gpio50 = GPIO_MODE_NATIVE, /* PCI_REQ1# */
	.gpio51 = GPIO_MODE_GPIO,   /* BBS_BIT1 */
	.gpio52 = GPIO_MODE_NATIVE, /* PCI_REQ2# */
	.gpio53 = GPIO_MODE_GPIO,   /* PWM_SELECT# / TP44 */
	.gpio54 = GPIO_MODE_GPIO,   /* PCI_REQ3# */
	.gpio55 = GPIO_MODE_NATIVE, /* PCI_GNT3# */
	.gpio56 = GPIO_MODE_NATIVE, /* CLK_PEGB_REQ# / TP60 */
	.gpio57 = GPIO_MODE_GPIO,   /* PCH_GPIO57 */
	.gpio58 = GPIO_MODE_NATIVE, /* SMB_ME1_CLK */
	.gpio59 = GPIO_MODE_GPIO,   /* USB_OC0_1# */
	.gpio60 = GPIO_MODE_GPIO,   /* DRAMRST_CNTRL_PCH */
	.gpio61 = GPIO_MODE_GPIO,   /* LPCPD# */
	.gpio62 = GPIO_MODE_NATIVE, /* PCH_SUSCLK_L / TP54 */
	.gpio63 = GPIO_MODE_NATIVE, /* TP51 */
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	/*
	 * Note: Only gpio configured as "gpio" or "none" need to have the
	 *       direction configured.
	 */
	.gpio33 = GPIO_DIR_OUTPUT,
	.gpio34 = GPIO_DIR_OUTPUT,
	.gpio35 = GPIO_DIR_OUTPUT,
	.gpio36 = GPIO_DIR_INPUT,
	.gpio37 = GPIO_DIR_INPUT,
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	.gpio40 = GPIO_DIR_INPUT,
	.gpio41 = GPIO_DIR_INPUT,

	.gpio48 = GPIO_DIR_OUTPUT,
	.gpio49 = GPIO_DIR_INPUT,

	.gpio51 = GPIO_DIR_OUTPUT,

	.gpio53 = GPIO_DIR_OUTPUT,
	.gpio54 = GPIO_DIR_INPUT,

	.gpio57 = GPIO_DIR_INPUT,

	.gpio59 = GPIO_DIR_INPUT,
	.gpio60 = GPIO_DIR_OUTPUT,
	.gpio61 = GPIO_DIR_OUTPUT,
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	/*
	 * Note: Only gpio configured as "gpio" or "none" need to have the
	 *       level set.
	 */
	.gpio33 = GPIO_LEVEL_LOW,
	.gpio34 = GPIO_LEVEL_HIGH,
	.gpio35 = GPIO_LEVEL_HIGH,
	.gpio36 = GPIO_LEVEL_LOW,
	.gpio37 = GPIO_LEVEL_LOW,
	.gpio38 = GPIO_LEVEL_LOW,
	.gpio39 = GPIO_LEVEL_LOW,
	.gpio40 = GPIO_LEVEL_HIGH,
	.gpio41 = GPIO_LEVEL_LOW,

	.gpio48 = GPIO_LEVEL_LOW,
	.gpio49 = GPIO_LEVEL_HIGH,

	.gpio51 = GPIO_LEVEL_HIGH,

	.gpio53 = GPIO_LEVEL_HIGH,
	.gpio54 = GPIO_LEVEL_LOW,

	.gpio57 = GPIO_LEVEL_LOW,

	.gpio59 = GPIO_LEVEL_HIGH,
	.gpio60 = GPIO_LEVEL_HIGH,
	.gpio61 = GPIO_LEVEL_LOW,
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_GPIO,	/* CLK_FLEX0 / TP38 */
	.gpio65 = GPIO_MODE_GPIO,	/* CLK_FLEX1 / TP45 */
	.gpio66 = GPIO_MODE_GPIO,	/* CLK_FLEX2 / TP83 */
	.gpio67 = GPIO_MODE_GPIO,	/* CLK_FLEX3 / TP82 */
	.gpio68 = GPIO_MODE_GPIO,	/* WWAN_DTCT# */
	.gpio69 = GPIO_MODE_GPIO,	/* GPIO69 */
	.gpio70 = GPIO_MODE_GPIO,	/* WLAN_OFF# */
	.gpio71 = GPIO_MODE_GPIO,	/* WWAN_OFF# */
	.gpio72 = GPIO_MODE_GPIO,	/* PM_BATLOW# */
	.gpio73 = GPIO_MODE_NATIVE,	/* PCIECLKRQ0# / TP39 */
	.gpio74 = GPIO_MODE_NATIVE,	/* SML1ALERT#_R / TP56 */
	.gpio75 = GPIO_MODE_NATIVE,	/* SMB_ME1_DAT */
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
	/*
	 * Note: Only gpio configured as "gpio" or "none" need to have the
	 *       direction configured.
	 */
	.gpio64 = GPIO_DIR_OUTPUT,
	.gpio65 = GPIO_DIR_OUTPUT,
	.gpio66 = GPIO_DIR_OUTPUT,
	.gpio67 = GPIO_DIR_INPUT,
	.gpio68 = GPIO_DIR_INPUT,
	.gpio69 = GPIO_DIR_OUTPUT,
	.gpio70 = GPIO_DIR_OUTPUT,
	.gpio71 = GPIO_DIR_OUTPUT,
	.gpio72 = GPIO_DIR_OUTPUT,
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
	/*
	 * Note: Only gpio configured as "gpio" or "none" need to have the
	 *       level set.
	 */
	.gpio64 = GPIO_LEVEL_HIGH,
	.gpio65 = GPIO_LEVEL_LOW,
	.gpio66 = GPIO_LEVEL_HIGH,
	.gpio67 = GPIO_LEVEL_LOW,
	.gpio68 = GPIO_LEVEL_HIGH,
	.gpio69 = GPIO_LEVEL_LOW,
	.gpio70 = GPIO_LEVEL_HIGH,
	.gpio71 = GPIO_LEVEL_HIGH,
	.gpio72 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_map stout_gpio_map = {
	.set1 = {
		.mode      = &pch_gpio_set1_mode,
		.direction = &pch_gpio_set1_direction,
		.level     = &pch_gpio_set1_level,
		.invert    = &pch_gpio_set1_invert,
	},
	.set2 = {
		.mode      = &pch_gpio_set2_mode,
		.direction = &pch_gpio_set2_direction,
		.level     = &pch_gpio_set2_level,
	},
	.set3 = {
		.mode      = &pch_gpio_set3_mode,
		.direction = &pch_gpio_set3_direction,
		.level     = &pch_gpio_set3_level,
	},
};
#endif
