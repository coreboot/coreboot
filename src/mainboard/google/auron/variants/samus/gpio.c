/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/lynxpoint/lp_gpio.h>

const struct pch_lp_gpio_map mainboard_gpio_map[] = {
	LP_GPIO_UNUSED,        /* 0: UNUSED */
	LP_GPIO_UNUSED,        /* 1: UNUSED */
	LP_GPIO_UNUSED,        /* 2: UNUSED */
	LP_GPIO_UNUSED,        /* 3: UNUSED */
	LP_GPIO_NATIVE,        /* 4: NATIVE: I2C0_SDA_GPIO4 */
	LP_GPIO_NATIVE,        /* 5: NATIVE: I2C0_SCL_GPIO5 */
	LP_GPIO_NATIVE,        /* 6: NATIVE: I2C1_SDA_GPIO6 */
	LP_GPIO_NATIVE,        /* 7: NATIVE: I2C1_SCL_GPIO7 */
	LP_GPIO_ACPI_SCI,      /* 8: PCH_LTE_WAKE_L */
	LP_GPIO_INPUT_INVERT,  /* 9: TRACKPAD_INT_L (WAKE) */
	LP_GPIO_ACPI_SCI,      /* 10: PCH_WLAN_WAKE_L */
	LP_GPIO_UNUSED,        /* 11: UNUSED */
	LP_GPIO_UNUSED,        /* 12: UNUSED */
	LP_GPIO_PIRQ,          /* 13: TRACKPAD_INT_L (PIRQL) */
	LP_GPIO_PIRQ,          /* 14: TOUCH_INT_L (PIRQM) */
	LP_GPIO_UNUSED,        /* 15: UNUSED (STRAP) */
	LP_GPIO_INPUT,         /* 16: PCH_WP */
	LP_GPIO_UNUSED,        /* 17: UNUSED */
	LP_GPIO_UNUSED,        /* 18: UNUSED */
	LP_GPIO_UNUSED,        /* 19: UNUSED */
	LP_GPIO_NATIVE,        /* 20: PCIE_WLAN_CLKREQ_L */
	LP_GPIO_OUT_HIGH,      /* 21: PP3300_SSD_EN */
	LP_GPIO_UNUSED,        /* 22: UNUSED */
	LP_GPIO_OUT_LOW,       /* 23: PP3300_AUTOBAHN_EN */
	LP_GPIO_UNUSED,        /* 24: UNUSED */
	LP_GPIO_INPUT,         /* 25: EC_IN_RW */
	LP_GPIO_UNUSED,        /* 26: UNUSED */
	LP_GPIO_ACPI_SCI,      /* 27: PCH_WAKE_L */
	LP_GPIO_UNUSED,        /* 28: UNUSED */
	LP_GPIO_UNUSED,        /* 29: UNUSED */
	LP_GPIO_NATIVE,        /* 30: NATIVE: PCH_SUSWARN_L */
	LP_GPIO_NATIVE,        /* 31: NATIVE: ACOK_BUF */
	LP_GPIO_NATIVE,        /* 32: NATIVE: LPC_CLKRUN_L */
	LP_GPIO_NATIVE,        /* 33: NATIVE: SSD_DEVSLP */
	LP_GPIO_ACPI_SMI,      /* 34: EC_SMI_L */
	LP_GPIO_ACPI_SMI,      /* 35: PCH_NMI_DBG_L (route in NMI_EN) */
	LP_GPIO_ACPI_SCI,      /* 36: EC_SCI_L */
	LP_GPIO_UNUSED,        /* 37: UNUSED */
	LP_GPIO_UNUSED,        /* 38: UNUSED */
	LP_GPIO_UNUSED,        /* 39: UNUSED */
	LP_GPIO_NATIVE,        /* 40: NATIVE: PCH_USB1_OC_L */
	LP_GPIO_NATIVE,        /* 41: NATIVE: PCH_USB2_OC_L */
	LP_GPIO_UNUSED,        /* 42: WLAN_DISABLE_L */
	LP_GPIO_OUT_HIGH,      /* 43: PP1800_CODEC_EN */
	LP_GPIO_UNUSED,        /* 44: UNUSED */
	LP_GPIO_ACPI_SCI,      /* 45: DSP_INT - Codec Wake */
	LP_GPIO_PIRQ,          /* 46: HOTWORD_DET_L_3V3 (PIRQO) - Codec IRQ */
	LP_GPIO_OUT_LOW,       /* 47: SSD_RESET_L */
	LP_GPIO_UNUSED,        /* 48: UNUSED */
	LP_GPIO_UNUSED,        /* 49: UNUSED */
	LP_GPIO_UNUSED,        /* 50: UNUSED */
	LP_GPIO_UNUSED,        /* 51: UNUSED */
	LP_GPIO_INPUT,         /* 52: SIM_DET */
	LP_GPIO_UNUSED,        /* 53: UNUSED */
	LP_GPIO_UNUSED,        /* 54: UNUSED */
	LP_GPIO_UNUSED,        /* 55: UNUSED */
	LP_GPIO_UNUSED,        /* 56: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 57: CODEC_RESET_L */
	LP_GPIO_UNUSED,        /* 58: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 59: LTE_DISABLE_L */
	LP_GPIO_UNUSED,        /* 60: UNUSED */
	LP_GPIO_NATIVE,        /* 61: NATIVE: PCH_SUS_STAT */
	LP_GPIO_NATIVE,        /* 62: NATIVE: PCH_SUSCLK */
	LP_GPIO_NATIVE,        /* 63: NATIVE: PCH_SLP_S5_L */
	LP_GPIO_UNUSED,        /* 64: UNUSED */
	LP_GPIO_INPUT,         /* 65: RAM_ID3 */
	LP_GPIO_INPUT,         /* 66: RAM_ID3_OLD (STRAP) */
	LP_GPIO_INPUT,         /* 67: RAM_ID0 */
	LP_GPIO_INPUT,         /* 68: RAM_ID1 */
	LP_GPIO_INPUT,         /* 69: RAM_ID2 */
	LP_GPIO_UNUSED,        /* 70: UNUSED */
	LP_GPIO_NATIVE,        /* 71: NATIVE: MODPHY_EN */
	LP_GPIO_UNUSED,        /* 72: UNUSED */
	LP_GPIO_UNUSED,        /* 73: UNUSED */
	LP_GPIO_UNUSED,        /* 74: UNUSED */
	LP_GPIO_UNUSED,        /* 75: UNUSED */
	LP_GPIO_UNUSED,        /* 76: UNUSED */
	LP_GPIO_UNUSED,        /* 77: UNUSED */
	LP_GPIO_UNUSED,        /* 78: UNUSED */
	LP_GPIO_UNUSED,        /* 79: UNUSED */
	LP_GPIO_UNUSED,        /* 80: UNUSED */
	LP_GPIO_UNUSED,        /* 81: UNUSED */
	LP_GPIO_NATIVE,        /* 82: NATIVE: EC_RCIN_L */
	LP_GPIO_NATIVE,        /* 83: GSPI0_CS */
	LP_GPIO_NATIVE,        /* 84: GSPI0_CLK */
	LP_GPIO_NATIVE,        /* 85: GSPI0_MISO */
	LP_GPIO_NATIVE,        /* 86: GSPI0_MOSI (STRAP) */
	LP_GPIO_UNUSED,        /* 87: UNUSED */
	LP_GPIO_UNUSED,        /* 88: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 89: PP3300_SD_EN */
	LP_GPIO_UNUSED,        /* 90: UNUSED */
	LP_GPIO_UNUSED,        /* 91: UNUSED */
	LP_GPIO_UNUSED,        /* 92: UNUSED */
	LP_GPIO_UNUSED,        /* 93: UNUSED */
	LP_GPIO_UNUSED,        /* 94: UNUSED */
	LP_GPIO_END
};
