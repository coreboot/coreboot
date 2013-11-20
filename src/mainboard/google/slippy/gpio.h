/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

#ifndef SLIPPY_GPIO_H
#define SLIPPY_GPIO_H

struct pch_lp_gpio_map;

const struct pch_lp_gpio_map mainboard_gpio_map[] = {
	LP_GPIO_UNUSED,        /* 0: UNUSED */
	LP_GPIO_UNUSED,        /* 1: UNUSED */
	LP_GPIO_UNUSED,        /* 2: UNUSED */
	LP_GPIO_UNUSED,        /* 3: UNUSED */
	LP_GPIO_NATIVE,        /* 4: NATIVE: I2C0_SDA_GPIO4 */
	LP_GPIO_NATIVE,        /* 5: NATIVE: I2C0_SCL_GPIO5 */
	LP_GPIO_NATIVE,        /* 6: NATIVE: I2C1_SDA_GPIO6 */
	LP_GPIO_NATIVE,        /* 7: NATIVE: I2C1_SCL_GPIO7 */
	LP_GPIO_ACPI_SCI,      /* 8: LTE_WAKE_L_Q */
	LP_GPIO_INPUT,         /* 9: RAM_ID1 */
	LP_GPIO_ACPI_SCI,      /* 10: WLAN_WAKE_L_Q */
	LP_GPIO_UNUSED,        /* 11: UNUSED */
	LP_GPIO_INPUT,         /* 12: TRACKPAD_INT_L (WAKE) */
	LP_GPIO_INPUT,         /* 13: RAM_ID0 */
	LP_GPIO_INPUT,         /* 14: EC_IN_RW */
	LP_GPIO_UNUSED,        /* 15: UNUSED (STRAP) */
	LP_GPIO_OUT_LOW,       /* 16: PCH_SSD_12_EN (iSSD VDDC) */
	LP_GPIO_OUT_LOW,       /* 17: PCH_SSD_18_EN (iSSD VCCQ) */
	LP_GPIO_NATIVE,        /* 18: PCIE_CLKREQ_WLAN# */
	LP_GPIO_UNUSED,        /* 19: UNUSED */
	LP_GPIO_UNUSED,        /* 20: UNUSED */
	LP_GPIO_UNUSED,        /* 21: UNUSED */
	LP_GPIO_UNUSED,        /* 22: UNUSED */
	LP_GPIO_UNUSED,        /* 23: UNUSED */
	LP_GPIO_UNUSED,        /* 24: UNUSED */
	LP_GPIO_INPUT,         /* 25: TOUCH_INT_L (WAKE) */
	LP_GPIO_UNUSED,        /* 26: UNUSED */
	LP_GPIO_UNUSED,        /* 27: UNUSED */
	LP_GPIO_UNUSED,        /* 28: UNUSED */
	LP_GPIO_UNUSED,        /* 29: UNUSED */
	LP_GPIO_NATIVE,        /* 30: NATIVE: PCH_SUSWARN_L */
	LP_GPIO_NATIVE,        /* 31: NATIVE: ACPRESENT */
	LP_GPIO_NATIVE,        /* 32: NATIVE: LPC_CLKRUN_L */
	LP_GPIO_NATIVE,        /* 33: NATIVE: DEVSLP0 */
	LP_GPIO_ACPI_SMI,      /* 34: EC_SMI_L */
	LP_GPIO_ACPI_SMI,      /* 35: PCH_NMI_DBG_L (route in NMI_EN) */
	LP_GPIO_ACPI_SCI,      /* 36: EC_SCI_L */
	LP_GPIO_UNUSED,        /* 37: UNUSED */
	LP_GPIO_UNUSED,        /* 38: UNUSED */
	LP_GPIO_UNUSED,        /* 39: UNUSED */
	LP_GPIO_NATIVE,        /* 40: NATIVE: USB_OC0# */
	LP_GPIO_UNUSED,        /* 41: UNUSED */
	LP_GPIO_NATIVE,        /* 42: NATIVE: USB_OC2# */
	LP_GPIO_UNUSED,        /* 43: UNUSED */
	LP_GPIO_OUT_LOW,       /* 44: PP3300_SSD_EN (iSSD VCC_FLASH) */
	LP_GPIO_OUT_HIGH,      /* 45: PP3300_CODEC_EN */
	LP_GPIO_OUT_HIGH,      /* 46: WLAN_DISABLE_L */
	LP_GPIO_INPUT,         /* 47: RAM_ID2 */
	LP_GPIO_UNUSED,        /* 48: UNUSED */
	LP_GPIO_OUT_LOW,       /* 49: PP3300_SSD_IO_EN (iSSD VCC_IO) */
	LP_GPIO_UNUSED,        /* 50: UNUSED */
	LP_GPIO_INPUT,         /* 51: ALS_INT_L */
	LP_GPIO_INPUT,         /* 52: SIM_DET */
	LP_GPIO_PIRQ,          /* 53: TRACKPAD_INT_DX (PIRQV) */
	LP_GPIO_PIRQ,          /* 54: TOUCH_INT_L_DX (PIRQW) */
	LP_GPIO_UNUSED,        /* 55: UNUSED */
	LP_GPIO_UNUSED,        /* 56: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 57: PP3300_CCD_EN */
	LP_GPIO_INPUT,         /* 58: PCH_SPI_WP_D */
	LP_GPIO_OUT_HIGH,      /* 59: LTE_DISABLE_L */
	LP_GPIO_NATIVE,        /* 60: NATIVE: SML0ALERT */
	LP_GPIO_NATIVE,        /* 61: NATIVE: PCH_SUS_STAT */
	LP_GPIO_NATIVE,        /* 62: NATIVE: PCH_SUS_CLK */
	LP_GPIO_NATIVE,        /* 63: NATIVE: PCH_SLP_S5_L */
	LP_GPIO_UNUSED,        /* 64: UNUSED */
	LP_GPIO_UNUSED,        /* 65: UNUSED */
	LP_GPIO_UNUSED,        /* 66: UNUSED */
	LP_GPIO_UNUSED,        /* 67: UNUSED */
	LP_GPIO_UNUSED,        /* 68: UNUSED */
	LP_GPIO_UNUSED,        /* 69: UNUSED */
	LP_GPIO_UNUSED,        /* 70: UNUSED */
	LP_GPIO_NATIVE,        /* 71: NATIVE: MODPHY_EN */
	LP_GPIO_NATIVE,        /* 72: NATIVE: PCH_BATLOW# */
	LP_GPIO_NATIVE,        /* 73: NATIVE: SMB1ALERT# */
	LP_GPIO_NATIVE,        /* 74: NATIVE: SMB_ME1_DAT */
	LP_GPIO_NATIVE,        /* 75: NATIVE: SMB_ME1_CLK */
	LP_GPIO_UNUSED,        /* 76: UNUSED */
	LP_GPIO_UNUSED,        /* 77: UNUSED */
	LP_GPIO_UNUSED,        /* 78: UNUSED */
	LP_GPIO_UNUSED,        /* 79: UNUSED */
	LP_GPIO_UNUSED,        /* 80: UNUSED */
	LP_GPIO_NATIVE,        /* 81: NATIVE: SPKR */
	LP_GPIO_NATIVE,        /* 82: NATIVE: EC_RCIN_L */
	LP_GPIO_UNUSED,        /* 83: UNUSED */
	LP_GPIO_UNUSED,        /* 84: UNUSED */
	LP_GPIO_UNUSED,        /* 85: UNUSED */
	LP_GPIO_UNUSED,        /* 86: UNUSED (STRAP) */
	LP_GPIO_UNUSED,        /* 87: UNUSED */
	LP_GPIO_UNUSED,        /* 88: UNUSED */
	LP_GPIO_UNUSED,        /* 89: UNUSED */
	LP_GPIO_UNUSED,        /* 90: UNUSED */
	LP_GPIO_UNUSED,        /* 91: UNUSED */
	LP_GPIO_UNUSED,        /* 92: UNUSED */
	LP_GPIO_UNUSED,        /* 93: UNUSED */
	LP_GPIO_UNUSED,        /* 94: UNUSED */
	LP_GPIO_END
};

#endif
