/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/lynxpoint/lp_gpio.h>

const struct pch_lp_gpio_map mainboard_lp_gpio_map[] = {
	LP_GPIO_UNUSED,        /* 0: UNUSED */
	LP_GPIO_UNUSED,        /* 1: UNUSED */
	LP_GPIO_UNUSED,        /* 2: UNUSED */
	LP_GPIO_UNUSED,        /* 3: UNUSED */
	LP_GPIO_NATIVE,        /* 4: NATIVE: I2C0_SDA_GPIO4 */
	LP_GPIO_NATIVE,        /* 5: NATIVE: I2C0_SCL_GPIO5 */
	LP_GPIO_NATIVE,        /* 6: NATIVE: I2C1_SDA_GPIO6 */
	LP_GPIO_NATIVE,        /* 7: NATIVE: I2C1_SCL_GPIO7 */
	LP_GPIO_ACPI_SCI,      /* 8: LAN_WAKE_L_Q */
	LP_GPIO_OUT_HIGH,      /* 9: PP3300_WLAN_EN */
	LP_GPIO_ACPI_SCI,      /* 10: WLAN_WAKE_L_Q */
	LP_GPIO_UNUSED,        /* 11: SMBALERT */
	LP_GPIO_INPUT_INVERT,  /* 12: RECOVERY_L */
	LP_GPIO_UNUSED,        /* 13: UNUSED */
	LP_GPIO_UNUSED,        /* 14: UNUSED */
	LP_GPIO_UNUSED,        /* 15: UNUSED (STRAP) */
	LP_GPIO_UNUSED,        /* 16: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 17: PP3300_VP8_EN */
	LP_GPIO_UNUSED,        /* 18: UNUSED */
	LP_GPIO_UNUSED,        /* 19: UNUSED */
	LP_GPIO_NATIVE,        /* 20: NATIVE: CLK_PCIE_REQ2# */
	LP_GPIO_NATIVE,        /* 21: NATIVE: CLK_PCIE_REQ3# */
	LP_GPIO_NATIVE,        /* 22: NATIVE: CLK_PCIE_REQ4# */
	LP_GPIO_UNUSED,        /* 23: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 24: WLAN_OFF_L */
	LP_GPIO_UNUSED,        /* 25: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 26: USB_CTL_1 */
	LP_GPIO_UNUSED,        /* 27: UNUSED */
	LP_GPIO_OUT_LOW,      /* 28: USB_ILIM_SEL */
	LP_GPIO_UNUSED,        /* 29: UNUSED */
	LP_GPIO_NATIVE,        /* 30: NATIVE: PCH_SUSPWRACK_L */
	LP_GPIO_NATIVE,        /* 31: NATIVE: PCH_ACPRESENT */
	LP_GPIO_NATIVE,        /* 32: NATIVE: CLKRUN# */
	LP_GPIO_NATIVE,        /* 33: NATIVE: DEVSLP0 */
	LP_GPIO_ACPI_SMI,      /* 34: EC_SMI_L */
	LP_GPIO_ACPI_SMI,      /* 35: PCH_NMI_DBG_L (route in NMI_EN) */
	LP_GPIO_ACPI_SCI,      /* 36: EC_SCI_L */
	LP_GPIO_UNUSED,        /* 37: UNUSED */
	LP_GPIO_UNUSED,        /* 38: UNUSED */
	LP_GPIO_UNUSED,        /* 39: UNUSED */
	LP_GPIO_NATIVE,        /* 40: NATIVE: USB_OC0# */
	LP_GPIO_NATIVE,        /* 41: NATIVE: USB_OC1# */
	LP_GPIO_NATIVE,        /* 42: NATIVE: USB_OC2# */
	LP_GPIO_NATIVE,        /* 43: NATIVE: USB_OC3# */
	LP_GPIO_UNUSED,        /* 44: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 45: PP5000_CODEC_EN */
	LP_GPIO_OUT_HIGH,      /* 46: BT_DISABLE_L */
	LP_GPIO_OUT_HIGH,      /* 47: USB1_PWR_EN */
	LP_GPIO_OUT_HIGH,      /* 48: USB4_PWR_EN */
	LP_GPIO_OUT_LOW,       /* 49: POWER_LED */
	LP_GPIO_OUT_HIGH,      /* 50: VP8_DISABLE_L */
	LP_GPIO_UNUSED,        /* 51: UNUSED */
	LP_GPIO_UNUSED,        /* 52: UNUSED */
	LP_GPIO_UNUSED,        /* 53: UNUSED */
	LP_GPIO_UNUSED,        /* 54: UNUSED */
	LP_GPIO_UNUSED,        /* 55: UNUSED */
	LP_GPIO_OUT_HIGH,      /* 56: USB2_PWR_EN */
	LP_GPIO_OUT_HIGH,      /* 57: USB3_PWR_EN */
	LP_GPIO_INPUT,         /* 58: PCH_SPI_WP_D */
	LP_GPIO_OUT_HIGH,      /* 59: PP3300_LAN_EN */
	LP_GPIO_NATIVE,        /* 60: NATIVE: SMB0ALERT# */
	LP_GPIO_UNUSED,        /* 61: UNUSED */
	LP_GPIO_UNUSED,        /* 62: UNUSED */
	LP_GPIO_NATIVE,        /* 63: NATIVE: PCH_SLP_S5_L */
	LP_GPIO_UNUSED,        /* 64: UNUSED */
	LP_GPIO_UNUSED,        /* 65: UNUSED */
	LP_GPIO_UNUSED,        /* 66: UNUSED (STRAP) */
	LP_GPIO_UNUSED,        /* 67: UNUSED */
	LP_GPIO_UNUSED,        /* 68: UNUSED */
	LP_GPIO_UNUSED,        /* 69: UNUSED */
	LP_GPIO_UNUSED,        /* 70: UNUSED */
	LP_GPIO_NATIVE,        /* 71: NATIVE: MODPHY_EN */
	LP_GPIO_UNUSED,        /* 72: UNUSED */
	LP_GPIO_UNUSED,        /* 73: UNUSED */
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
