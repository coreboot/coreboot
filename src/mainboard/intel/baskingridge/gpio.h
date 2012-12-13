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

#ifndef BASKING_RIDGE_GPIO_H
#define BASKING_RIDGE_GPIO_H

#include "southbridge/intel/lynxpoint/gpio.h"

const struct pch_gpio_set1 pch_gpio_set1_mode = {
	.gpio0 = GPIO_MODE_GPIO,    /* PCH_GPIO0_R -> S_GPIO -> J9F4 */
	.gpio1 = GPIO_MODE_GPIO,    /* SMC_EXTSMI_N */
	.gpio2 = GPIO_MODE_GPIO,    /* TP_RSVD_TESTMODE  - float */
	.gpio3 = GPIO_MODE_NATIVE,  /* PCH_PCI_IRQ_N -> SIO GPIO12/SMI# */
	.gpio4 = GPIO_MODE_GPIO,    /* EXTTS_SNI_DRV0_PCH  - float */
	.gpio5 = GPIO_MODE_GPIO,    /* EXTTS_SNI_DRV1_PCH  - float */
	.gpio6 = GPIO_MODE_GPIO,    /* DGPU_HPD_INTR_N */
	.gpio7 = GPIO_MODE_GPIO,    /* SMC_RUNTIME_SCI_N */
	.gpio8 = GPIO_MODE_GPIO,    /* PCH_GPIO8 -> DDR Voltage Select Bit 0 */
	.gpio9  = GPIO_MODE_NATIVE, /* USB_OC_10_11_R_N */
	.gpio10 = GPIO_MODE_NATIVE, /* USB_OC_12_13_R_N */
	.gpio11 = GPIO_MODE_GPIO,   /* PCH_GPIO11 -> HOST_ALERT2_N -> PCIE_RSVD_2_N (3GIO_X1) slot 4 */
	.gpio12 = GPIO_MODE_GPIO,   /* PM_LANPHY_ENABLE */
	.gpio13 = GPIO_MODE_NATIVE, /* HDA_DOCK_RST_N */
	.gpio14 = GPIO_MODE_GPIO,   /* SMC_WAKE_SCI_N (not stuffed) & USB_8_9_PWR */
	.gpio15 = GPIO_MODE_GPIO,   /* Always GPIO: HOST_ALERT1_R_N -> PCIE_RSV_1_N */
	.gpio16 = GPIO_MODE_NATIVE, /* SATA_DET4_R_N */
	.gpio17 = GPIO_MODE_GPIO,   /* DGPU_PWROK */
	.gpio18 = GPIO_MODE_NATIVE, /* CK_SLOT1_OE_N_R */
	.gpio19 = GPIO_MODE_GPIO,   /* BBS_BIT0_R - STRAP */
	.gpio20 = GPIO_MODE_NATIVE, /* CK_SLOT2_OE_N_R */
	.gpio21 = GPIO_MODE_GPIO,   /* SATA_DET0_R_N -> J9H4 */
	.gpio22 = GPIO_MODE_GPIO,   /* BIOS_REC -> J8G1 */
	.gpio23 = GPIO_MODE_NATIVE, /* PCH_DRQ1_N */
	.gpio24 = GPIO_MODE_GPIO,   /* Always GPIO: PCH_GPIO24_R1 -> DDR Voltage Select Bit 2 */
	.gpio25 = GPIO_MODE_NATIVE, /* CK_SLOT3_OE_N */
	.gpio26 = GPIO_MODE_NATIVE, /* CK_SLOT4_OE_N */
	.gpio27 = GPIO_MODE_GPIO,   /* Always GPIO: PCH_GPIO_27 -> SMC_WAKE_SCI_N & LANWAKE_N */
	.gpio28 = GPIO_MODE_GPIO,   /* Always GPIO: PLL_ODVR_EN -> PCH_AUDIO_PWR_N */
	.gpio29 = GPIO_MODE_NATIVE, /* PCH_SLP_WLAN_N */
	.gpio30 = GPIO_MODE_NATIVE, /* SUS_PWR_ACK_R */
	.gpio31 = GPIO_MODE_NATIVE, /* AC_PRESENT_R */
};

const struct pch_gpio_set1 pch_gpio_set1_direction = {
	.gpio0 = GPIO_DIR_INPUT,
	.gpio1 = GPIO_DIR_INPUT,
	.gpio2 = GPIO_DIR_OUTPUT,
	/* .gpio3 NATIVE */
	.gpio4 = GPIO_DIR_OUTPUT,
	.gpio5 = GPIO_DIR_OUTPUT,
	.gpio6 = GPIO_DIR_INPUT,
	.gpio7 = GPIO_DIR_INPUT,
	.gpio8 = GPIO_DIR_OUTPUT,
	.gpio9 = GPIO_DIR_INPUT,
	/* .gpio10  NATIVE */
	.gpio11 = GPIO_DIR_INPUT,
	.gpio12 = GPIO_DIR_OUTPUT,
	/* .gpio13  NATIVE */
	.gpio14 = GPIO_DIR_INPUT,
	.gpio15 = GPIO_DIR_INPUT,
	/* .gpio16  NATIVE */
	.gpio17 = GPIO_DIR_INPUT,
	/* .gpio18  NATIVE */
	.gpio19 = GPIO_DIR_INPUT,
	.gpio20 = GPIO_DIR_INPUT,
	.gpio21 = GPIO_DIR_INPUT,
	.gpio22 = GPIO_DIR_INPUT,
	/* .gpio23  NATIVE */
	.gpio24 = GPIO_DIR_OUTPUT,
	/* .gpio25  NATIVE */
	/* .gpio26  NATIVE */
	.gpio27 = GPIO_DIR_INPUT,
	.gpio28 = GPIO_DIR_INPUT,
	/* .gpio29  NATIVE */
	/* .gpio30  NATIVE */
	/* .gpio31  NATIVE */
};

const struct pch_gpio_set1 pch_gpio_set1_level = {
	.gpio2 = GPIO_LEVEL_HIGH,
	.gpio4 = GPIO_LEVEL_HIGH,
	.gpio5 = GPIO_LEVEL_HIGH,
	.gpio8 = GPIO_LEVEL_HIGH,
	.gpio12 = GPIO_LEVEL_LOW,
	.gpio24 = GPIO_LEVEL_LOW,
};

const struct pch_gpio_set1 pch_gpio_set1_invert = {
};

const struct pch_gpio_set2 pch_gpio_set2_mode = {
	.gpio32 = GPIO_MODE_NATIVE, /* Always GPIO on desktop. Mobile Native. PM_CLKRUN_N */
	.gpio33 = GPIO_MODE_NATIVE, /* HDADOCKEN_R_N */
	.gpio34 = GPIO_MODE_GPIO,   /* PCH_GPIO34 -> SATA_PWR_EN0_N */
	.gpio35 = GPIO_MODE_GPIO,   /* SATA_PWR_EN1_R_N  */
	.gpio36 = GPIO_MODE_NATIVE, /* SATA_ODD_PRSNT_R_N */
	.gpio37 = GPIO_MODE_NATIVE, /* SATA_ODD_DA_N_R */
	.gpio38 = GPIO_MODE_GPIO,   /* MFG_MODE */
	.gpio39 = GPIO_MODE_GPIO,   /* GFX_CRB_DET */
	.gpio40 = GPIO_MODE_NATIVE, /* USB_OC_2_5_R_N */
	.gpio41 = GPIO_MODE_GPIO,   /* USB_0_1_PWR */
	.gpio42 = GPIO_MODE_NATIVE, /* USB_OC_6_7_R_N */
	.gpio43 = GPIO_MODE_NATIVE, /* USB_OSC_8_9_R_N */
	.gpio44 = GPIO_MODE_NATIVE, /* CK_SLOT5_OE_N */
	.gpio45 = GPIO_MODE_NATIVE, /* CK_PCIE_LAN_REQ_N */
	.gpio46 = GPIO_MODE_GPIO,   /* PCH_GPIO46_R -> DDR Voltage Select Bit 1 */
	.gpio47 = GPIO_MODE_NATIVE, /* PEGA_CKREQ_N */
	.gpio48 = GPIO_MODE_GPIO,   /* BIOS_RESP -> J8E3 */
	.gpio49 = GPIO_MODE_GPIO,   /* PCH_GP_49 -> CRIT_TEMP_REP_N */
	.gpio50 = GPIO_MODE_GPIO,   /* DGPU_HOLD_RST_N */
	.gpio51 = GPIO_MODE_GPIO,   /* BBS_BIT1 Strap */
	.gpio52 = GPIO_MODE_GPIO,   /* DGPU_SELECT_N */
	.gpio53 = GPIO_MODE_GPIO,   /* DGPU_PWM_SELECT_N -> PEG_JTAG5 */
	.gpio54 = GPIO_MODE_GPIO,   /* DGPU_PWR_EN_N -> PEG_RSVD5 */
	.gpio55 = GPIO_MODE_GPIO,   /* STP_A16OVR Strap */
	.gpio56 = GPIO_MODE_NATIVE, /* MC_CKREQ_N */
	.gpio57 = GPIO_MODE_GPIO,   /* Always GPIO. NFC_IRQ_R */
	.gpio58 = GPIO_MODE_NATIVE, /* SML1_CK */
	.gpio59 = GPIO_MODE_NATIVE, /* USB_OC_0_1_R_N */
	.gpio60 = GPIO_MODE_GPIO,   /* DRAMRST_CNTRL_PCH */
	.gpio61 = GPIO_MODE_NATIVE, /* PM_SUS_STAT_N */
	.gpio62 = GPIO_MODE_NATIVE, /* SUS_CK */
	.gpio63 = GPIO_MODE_NATIVE, /* SLP_S5_R_N */
};

const struct pch_gpio_set2 pch_gpio_set2_direction = {
	/* .gpio32  NATIVE */
	/* .gpio33  NATIVE */
	.gpio34 = GPIO_DIR_OUTPUT,
	.gpio35 = GPIO_DIR_OUTPUT,
	/* .gpio36  NATIVE */
	/* .gpio37  NATIVE */
	.gpio38 = GPIO_DIR_INPUT,
	.gpio39 = GPIO_DIR_INPUT,
	/* .gpio40  NATIVE */
	.gpio41 = GPIO_DIR_OUTPUT,
	/* .gpio42  NATIVE */
	/* .gpio43  NATIVE */
	/* .gpio44  NATIVE */
	/* .gpio45  NATIVE */
	.gpio46 = GPIO_DIR_OUTPUT,
	/* .gpio47  NATIVE */
	.gpio48 = GPIO_DIR_INPUT,
	.gpio49 = GPIO_DIR_OUTPUT,
	.gpio50 = GPIO_DIR_OUTPUT,
	.gpio51 = GPIO_DIR_OUTPUT,
	.gpio52 = GPIO_DIR_OUTPUT,
	.gpio53 = GPIO_DIR_OUTPUT,
	.gpio54 = GPIO_DIR_OUTPUT,
	.gpio55 = GPIO_DIR_OUTPUT,
	/* .gpio56  NATIVE */
	.gpio57 = GPIO_DIR_INPUT,
	/* .gpio58  NATIVE */
	/* .gpio59  NATIVE */
	.gpio60 = GPIO_DIR_OUTPUT,
	/* .gpio61  NATIVE */
	/* .gpio62  NATIVE */
	/* .gpio63  NATIVE */
};

const struct pch_gpio_set2 pch_gpio_set2_level = {
	.gpio34 = GPIO_LEVEL_LOW,
	.gpio41 = GPIO_LEVEL_HIGH,
	.gpio35 = GPIO_LEVEL_LOW,
	.gpio46 = GPIO_LEVEL_HIGH,
	.gpio49 = GPIO_LEVEL_HIGH,
	.gpio50 = GPIO_LEVEL_HIGH,
	.gpio51 = GPIO_LEVEL_LOW,
	.gpio52 = GPIO_LEVEL_LOW,
	.gpio53 = GPIO_LEVEL_LOW,
	.gpio54 = GPIO_LEVEL_LOW,
	.gpio55 = GPIO_LEVEL_LOW,
	.gpio60 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_set3 pch_gpio_set3_mode = {
	.gpio64 = GPIO_MODE_NATIVE, /* CK_PCH_SIO_DOCK_R -> TP_CK_PCI_SIO_DOCK */
	.gpio65 = GPIO_MODE_NATIVE, /* CK_FLEX1 */
	.gpio66 = GPIO_MODE_GPIO,   /* CK_FLEX2 */
	.gpio67 = GPIO_MODE_GPIO,   /* DGPU_PRSNT_N -> PEG_RSVD3 */
	.gpio68 = GPIO_MODE_GPIO,   /* SATA_ODD_PWRGT */
	.gpio69 = GPIO_MODE_GPIO,   /* SV_DET -> J8E2 */
	.gpio70 = GPIO_MODE_GPIO,   /* USB3_DET_P2_N */
	.gpio71 = GPIO_MODE_GPIO,   /* USB3_DET_P3_N */
	.gpio72 = GPIO_MODE_NATIVE, /* PM_BATLOW_R_N */
	.gpio73 = GPIO_MODE_NATIVE, /* CK_REQ_DOCK_N */
	.gpio74 = GPIO_MODE_NATIVE, /* PCH_GPIO74_R -> AMB_THM2_R_N (PCHHOT) */
	.gpio75 = GPIO_MODE_NATIVE, /* SM1_DATA */
};

const struct pch_gpio_set3 pch_gpio_set3_direction = {
	.gpio64 = GPIO_DIR_OUTPUT,
	/* .gpio65  NATIVE */
	.gpio66 = GPIO_DIR_OUTPUT,
	.gpio67 = GPIO_DIR_INPUT,
	.gpio68 = GPIO_DIR_OUTPUT,
	.gpio69 = GPIO_DIR_INPUT,
	.gpio70 = GPIO_DIR_INPUT,
	.gpio71 = GPIO_DIR_INPUT,
	/* .gpio72  NATIVE */
	/* .gpio73  NATIVE */
	/* .gpio74  NATIVE */
	/* .gpio75  NATIVE */
};

const struct pch_gpio_set3 pch_gpio_set3_level = {
	.gpio64 = GPIO_LEVEL_LOW,
	.gpio66 = GPIO_LEVEL_LOW,
	.gpio68 = GPIO_LEVEL_HIGH,
};

const struct pch_gpio_map mainboard_gpio_map = {
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
