/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#ifndef INTEL_WTM2_GPIO_H
#define INTEL_WTM2_GPIO_H

#include "southbridge/intel/lynxpoint/lp_gpio.h"

static const struct pch_lp_gpio_map mainboard_gpio_map[] = {
	LP_GPIO_NATIVE,        /* 0: LPSS_UART1_RXD */
	LP_GPIO_NATIVE,        /* 1: LPSS_UART1_TXD */
	LP_GPIO_NATIVE,        /* 2: LPSS_UART1_RTS_N_R */
	LP_GPIO_NATIVE,        /* 3: LPSS_UART1_CTS_N */
	LP_GPIO_NATIVE,        /* 4: LPSS_I2C0_SDA_R */
	LP_GPIO_NATIVE,        /* 5: LPSS_I2C0_SCL */
	LP_GPIO_NATIVE,        /* 6: LPSS_I2C1_SDA */
	LP_GPIO_NATIVE,        /* 7: LPSS_I2C1_SCL */
	LP_GPIO_UNUSED,        /* 8: NGFF_SLTA_WIFI_WAKE_N */
	LP_GPIO_UNUSED,        /* 9: ACCEL_INT2_MCP */
	LP_GPIO_ACPI_SCI,      /* 10: SMC_RUNTIME_SCI_N */
	LP_GPIO_UNUSED,        /* 11: AMB_THRM_R_N */
	LP_GPIO_NATIVE,        /* 12: PM_LANPHY_ENABLE */
	LP_GPIO_OUT_HIGH,      /* 13: USB32_P0_PWREN */
	LP_GPIO_IRQ_EDGE,      /* 14: SH_INT_ACCEL_DRDY_USB_INT_N */
	LP_GPIO_OUT_HIGH,      /* 15: LAN_PWREN_N */
	LP_GPIO_OUT_HIGH,      /* 16: LAN_RST_N */
	LP_GPIO_OUT_LOW,       /* 17: CRIT_TEMP_REP_R_N */
	LP_GPIO_UNUSED,        /* 18: TBT_FORCE_PWR */
	LP_GPIO_INPUT,         /* 19: EC_IN_RW */
	LP_GPIO_NATIVE,        /* 20: CK_REQ_P2_NGFFSLTA_N_R */
	LP_GPIO_NATIVE,        /* 21: CK_PCIE_LAN_REQ_N */
	LP_GPIO_NATIVE,        /* 22: CK_REQ_P4_TBT_N */
	LP_GPIO_NATIVE,        /* 23: CK_REQ_P5_N */
	LP_GPIO_OUT_LOW,       /* 24: ME_PG_LED */
	LP_GPIO_INPUT,         /* 25: USB_WAKEOUT_N */
	LP_GPIO_IRQ_EDGE,      /* 26: NFC_IRQ_MGP5 */
	LP_GPIO_ACPI_SCI,      /* 27: SMC_WAKE_SCI_N */
	LP_GPIO_OUT_LOW,       /* 28: PCH_NFC_RESET */
	LP_GPIO_NATIVE,        /* 29: PCH_SLP_WLAN_N */
	LP_GPIO_NATIVE,        /* 30: SUS_PWR_ACK_R */
	LP_GPIO_NATIVE,        /* 31: AC_PRESENT_R */
	LP_GPIO_NATIVE,        /* 32: PM_CKRUN_N */
	LP_GPIO_OUT_LOW,       /* 33: SATA0_PHYSLP */
	LP_GPIO_INPUT,         /* 34: ESATA_DET_N */
	LP_GPIO_INPUT,         /* 35: SATA_DIRECT_PRSNT_R_N */
	LP_GPIO_INPUT,         /* 36: NGFF_SSD_SATA2_PCIE1_DET_N */
	LP_GPIO_INPUT,         /* 37: NGFF_SSD_SATA3_PCIE0_DET_N */
	LP_GPIO_OUT_LOW,       /* 38: SATA1_PHYSLP_DIRECT */
	LP_GPIO_ACPI_SMI,      /* 39: SMC_EXTSMI_N_R */
	LP_GPIO_NATIVE,        /* 40: USB_OC_0_1_R_N */
	LP_GPIO_NATIVE,        /* 41: USB_OC_2_6_R_N */
	LP_GPIO_INPUT,         /* 42: TBT_CIO_PLUG_SMI_N_R */
	LP_GPIO_OUT_HIGH,      /* 43: USB32_P1_PWREN */
	LP_GPIO_INPUT,         /* 44: SENSOR_HUB_RST_N */
	LP_GPIO_INPUT,         /* 45: GYRO_INT2_MCP_R */
	LP_GPIO_OUT_HIGH,      /* 46: SNSR_HUB_PWREN */
	LP_GPIO_IRQ_EDGE,      /* 47: SPI_TPM_HDR_IRQ_N */
	LP_GPIO_OUT_HIGH,      /* 48: PCIE_TBT_RST_N */
	LP_GPIO_INPUT,         /* 49: COMBO_JD */
	LP_GPIO_IRQ_EDGE,      /* 50: TOUCH_PANEL_INTR_N */
	LP_GPIO_OUT_HIGH,      /* 51: PCH_WIFI_RF_KILL_N */
	LP_GPIO_OUT_HIGH,      /* 52: TOUCH_PNL_RST_N_R */
	LP_GPIO_INPUT,         /* 53: SNSR_HUB_I2C_WAKE / ALS_INT_MCP */
	LP_GPIO_ACPI_SCI,      /* 54: NGFF_SLTB_SSD_MC_WAKE_N */
	LP_GPIO_IRQ_EDGE,      /* 55: TOUCHPAD_INTR_N */
	LP_GPIO_INPUT,         /* 56: NGFF_SLTB_WWAN_SSD_DET1 */
	LP_GPIO_OUT_HIGH,      /* 57: NGFF_SLTB_WWAN_PWREN */
	LP_GPIO_OUT_LOW,       /* 58: SLATEMODE_HALLOUT_R */
	LP_GPIO_OUT_HIGH,      /* 59: USB2_CAM_PWREN */
	LP_GPIO_OUT_LOW,       /* 60: USB_CR_PWREN_N */
	LP_GPIO_NATIVE,        /* 61: PM_SUS_STAT_N */
	LP_GPIO_NATIVE,        /* 62: SUS_CK */
	LP_GPIO_NATIVE,        /* 63: SLP_S5_R_N */
	LP_GPIO_NATIVE,        /* 64: LPSS_SDIO_CLK_CMNHDR_R */
	LP_GPIO_NATIVE,        /* 65: LPSS_SDIO_CMD_CMNHDR_R */
	LP_GPIO_NATIVE,        /* 66: LPSS_SDIO_D0_CMNHDR_R */
	LP_GPIO_NATIVE,        /* 67: LPSS_SDIO_D1_CMNHDR_R */
	LP_GPIO_NATIVE,        /* 68: LPSS_SDIO_D2_CMNHDR_R */
	LP_GPIO_NATIVE,        /* 69: LPSS_SDIO_D3_CMNHDR_R1 */
	LP_GPIO_NATIVE,        /* 70: NGFF_SLTA_WIFI_PWREN_N_R */
	LP_GPIO_OUT_HIGH,      /* 71: MPHY_PWREN */
	LP_GPIO_NATIVE,        /* 72: PM_BATLOW_R_N */
	LP_GPIO_NATIVE,        /* 73: PCH_NOT_N */
	LP_GPIO_NATIVE,        /* 74: SML1_DATA */
	LP_GPIO_NATIVE,        /* 75: SML1_CK */
	LP_GPIO_OUT_HIGH,      /* 76: PCH_AUDIO_PWR_R */
	LP_GPIO_OUT_LOW,       /* 77: PC_SLTB_SSD_RST_N_R */
	LP_GPIO_INPUT,         /* 78: PM_EXTTS0_EC_N */
	LP_GPIO_IRQ_EDGE,      /* 79: SIO1007_IRQ_N */
	LP_GPIO_INPUT,         /* 80: PM_EXTTS1_R_N */
	LP_GPIO_NATIVE,        /* 81: PCH_HDA_SPKR */
	LP_GPIO_NATIVE,        /* 82: H_RCIN_N */
	LP_GPIO_NATIVE,        /* 83: LPSS_GSPI0_CS_R_N */
	LP_GPIO_NATIVE,        /* 84: LPSS_GSPI0_CLK_R */
	LP_GPIO_NATIVE,        /* 85: LPSS_GSPI0_MISO_R */
	LP_GPIO_NATIVE,        /* 86: LPSS_GSPI0_MOSI_BBS0_R */
	LP_GPIO_NATIVE,        /* 87: LPSS_GSPI1_CS_R_N */
	LP_GPIO_NATIVE,        /* 88: LPSS_GSPI1_CLK_R */
	LP_GPIO_NATIVE,        /* 89: LPSS_GSPI1_MISO_R */
	LP_GPIO_OUT_LOW,       /* 90: NGFF_SLTA_WIFI_RST_N */
	LP_GPIO_NATIVE,        /* 91: LPSS_UART0_RXD */
	LP_GPIO_NATIVE,        /* 92: LPSS_UART0_TXD */
	LP_GPIO_NATIVE,        /* 93: LPSS_UART0_RTS_N */
	LP_GPIO_NATIVE,        /* 94: LPSS_UART0_CTS_N */
	LP_GPIO_END
};

#endif
