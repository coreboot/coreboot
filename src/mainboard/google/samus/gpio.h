/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc.
 */

#ifndef SAMUS_GPIO_H
#define SAMUS_GPIO_H

#include <soc/gpio.h>

#define SAMUS_GPIO_PP3300_AUTOBAHN_EN	23
#define SAMUS_GPIO_SSD_RESET_L		47
#define SAMUS_GPIO_WLAN_DISABLE_L	42
#define SAMUS_GPIO_LTE_DISABLE_L	59

static const struct gpio_config mainboard_gpio_config[] = {
	PCH_GPIO_UNUSED,        /* 0: UNUSED */
	PCH_GPIO_UNUSED,        /* 1: UNUSED */
	PCH_GPIO_UNUSED,        /* 2: UNUSED */
	PCH_GPIO_UNUSED,        /* 3: UNUSED */
	PCH_GPIO_NATIVE,        /* 4: NATIVE: I2C0_SDA_GPIO4 */
	PCH_GPIO_NATIVE,        /* 5: NATIVE: I2C0_SCL_GPIO5 */
	PCH_GPIO_NATIVE,        /* 6: NATIVE: I2C1_SDA_GPIO6 */
	PCH_GPIO_NATIVE,        /* 7: NATIVE: I2C1_SCL_GPIO7 */
	PCH_GPIO_ACPI_SCI,      /* 8: PCH_LTE_WAKE_L */
	PCH_GPIO_INPUT_INVERT,  /* 9: TRACKPAD_INT_L (WAKE) */
	PCH_GPIO_ACPI_SCI,      /* 10: PCH_WLAN_WAKE_L */
	PCH_GPIO_UNUSED,        /* 11: UNUSED */
	PCH_GPIO_UNUSED,        /* 12: UNUSED */
	PCH_GPIO_PIRQ,          /* 13: TRACKPAD_INT_L (PIRQL) */
	PCH_GPIO_PIRQ,          /* 14: TOUCH_INT_L (PIRQM) */
	PCH_GPIO_UNUSED,        /* 15: UNUSED (STRAP) */
	PCH_GPIO_INPUT,         /* 16: PCH_WP */
	PCH_GPIO_UNUSED,        /* 17: UNUSED */
	PCH_GPIO_UNUSED,        /* 18: UNUSED */
	PCH_GPIO_UNUSED,        /* 19: UNUSED */
	PCH_GPIO_NATIVE,        /* 20: PCIE_WLAN_CLKREQ_L */
	PCH_GPIO_OUT_HIGH,      /* 21: PP3300_SSD_EN */
	PCH_GPIO_UNUSED,        /* 22: UNUSED */
	PCH_GPIO_OUT_LOW,       /* 23: PP3300_AUTOBAHN_EN */
	PCH_GPIO_UNUSED,        /* 24: UNUSED */
	PCH_GPIO_INPUT,         /* 25: EC_IN_RW */
	PCH_GPIO_UNUSED,        /* 26: UNUSED */
	PCH_GPIO_ACPI_SCI,      /* 27: PCH_WAKE_L */
	PCH_GPIO_UNUSED,        /* 28: UNUSED */
	PCH_GPIO_UNUSED,        /* 29: UNUSED */
	PCH_GPIO_NATIVE,        /* 30: NATIVE: PCH_SUSWARN_L */
	PCH_GPIO_NATIVE,        /* 31: NATIVE: ACOK_BUF */
	PCH_GPIO_NATIVE,        /* 32: NATIVE: LPC_CLKRUN_L */
	PCH_GPIO_NATIVE,        /* 33: NATIVE: SSD_DEVSLP */
	PCH_GPIO_ACPI_SMI,      /* 34: EC_SMI_L */
	PCH_GPIO_ACPI_SMI,      /* 35: PCH_NMI_DBG_L (route in NMI_EN) */
	PCH_GPIO_ACPI_SCI,      /* 36: EC_SCI_L */
	PCH_GPIO_UNUSED,        /* 37: UNUSED */
	PCH_GPIO_UNUSED,        /* 38: UNUSED */
	PCH_GPIO_UNUSED,        /* 39: UNUSED */
	PCH_GPIO_NATIVE,        /* 40: NATIVE: PCH_USB1_OC_L */
	PCH_GPIO_NATIVE,        /* 41: NATIVE: PCH_USB2_OC_L */
	PCH_GPIO_UNUSED,        /* 42: WLAN_DISABLE_L */
	PCH_GPIO_OUT_HIGH,      /* 43: PP1800_CODEC_EN */
	PCH_GPIO_UNUSED,        /* 44: UNUSED */
	PCH_GPIO_ACPI_SCI,      /* 45: DSP_INT - Codec Wake */
	PCH_GPIO_PIRQ,          /* 46: HOTWORD_DET_L_3V3 (PIRQO) - Codec IRQ */
	PCH_GPIO_OUT_LOW,       /* 47: SSD_RESET_L */
	PCH_GPIO_UNUSED,        /* 48: UNUSED */
	PCH_GPIO_UNUSED,        /* 49: UNUSED */
	PCH_GPIO_UNUSED,        /* 50: UNUSED */
	PCH_GPIO_UNUSED,        /* 51: UNUSED */
	PCH_GPIO_INPUT,         /* 52: SIM_DET */
	PCH_GPIO_UNUSED,        /* 53: UNUSED */
	PCH_GPIO_UNUSED,        /* 54: UNUSED */
	PCH_GPIO_UNUSED,        /* 55: UNUSED */
	PCH_GPIO_UNUSED,        /* 56: UNUSED */
	PCH_GPIO_OUT_HIGH,      /* 57: CODEC_RESET_L */
	PCH_GPIO_UNUSED,        /* 58: UNUSED */
	PCH_GPIO_OUT_HIGH,      /* 59: LTE_DISABLE_L */
	PCH_GPIO_UNUSED,        /* 60: UNUSED */
	PCH_GPIO_NATIVE,        /* 61: NATIVE: PCH_SUS_STAT */
	PCH_GPIO_NATIVE,        /* 62: NATIVE: PCH_SUSCLK */
	PCH_GPIO_NATIVE,        /* 63: NATIVE: PCH_SLP_S5_L */
	PCH_GPIO_UNUSED,        /* 64: UNUSED */
	PCH_GPIO_INPUT,         /* 65: RAM_ID3 */
	PCH_GPIO_INPUT,         /* 66: RAM_ID3_OLD (STRAP) */
	PCH_GPIO_INPUT,         /* 67: RAM_ID0 */
	PCH_GPIO_INPUT,         /* 68: RAM_ID1 */
	PCH_GPIO_INPUT,         /* 69: RAM_ID2 */
	PCH_GPIO_UNUSED,        /* 70: UNUSED */
	PCH_GPIO_NATIVE,        /* 71: NATIVE: MODPHY_EN */
	PCH_GPIO_UNUSED,        /* 72: UNUSED */
	PCH_GPIO_UNUSED,        /* 73: UNUSED */
	PCH_GPIO_UNUSED,        /* 74: UNUSED */
	PCH_GPIO_UNUSED,        /* 75: UNUSED */
	PCH_GPIO_UNUSED,        /* 76: UNUSED */
	PCH_GPIO_UNUSED,        /* 77: UNUSED */
	PCH_GPIO_UNUSED,        /* 78: UNUSED */
	PCH_GPIO_UNUSED,        /* 79: UNUSED */
	PCH_GPIO_UNUSED,        /* 80: UNUSED */
	PCH_GPIO_UNUSED,        /* 81: UNUSED */
	PCH_GPIO_NATIVE,        /* 82: NATIVE: EC_RCIN_L */
	PCH_GPIO_NATIVE,        /* 83: GSPI0_CS */
	PCH_GPIO_NATIVE,        /* 84: GSPI0_CLK */
	PCH_GPIO_NATIVE,        /* 85: GSPI0_MISO */
	PCH_GPIO_NATIVE,        /* 86: GSPI0_MOSI (STRAP) */
	PCH_GPIO_UNUSED,        /* 87: UNUSED */
	PCH_GPIO_UNUSED,        /* 88: UNUSED */
	PCH_GPIO_OUT_HIGH,      /* 89: PP3300_SD_EN */
	PCH_GPIO_UNUSED,        /* 90: UNUSED */
	PCH_GPIO_UNUSED,        /* 91: UNUSED */
	PCH_GPIO_UNUSED,        /* 92: UNUSED */
	PCH_GPIO_UNUSED,        /* 93: UNUSED */
	PCH_GPIO_UNUSED,        /* 94: UNUSED */
	PCH_GPIO_END
};

#endif
