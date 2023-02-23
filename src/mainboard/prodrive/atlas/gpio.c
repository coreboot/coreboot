/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

#include "gpio.h"

/*
 * Pad configuration in ramstage
 * Intel Client PCIe* controller expects each device should drive the SRCCLKREQ#.
 * If the GPIO is set to native mode for a device, which do not support SRCCLKREQ#,
 * then during RTD3 exit link would not be established. Because controller samples
 * the SRCCLKREQ# before detecting the device and break L1 as the system might enter L1SS
 * as controller detects SRCCLKREQ# as de-asserted.
 * As a workaround the Pins must not be configured in Native Mode (CLKREQ mode).
 * Therefore here they are not configured at all.
 * source: 689882 (intel document ID)
 *
 * So apparently hardware doesn't sample SRCCLKREQ Pin if it's not configured as such.
 * That workaround suggestion however also brought a patch to FSP, which in
 * turn causes the same Bug (even if SRCLKREQ are not configured). Usually
 * in order to make use of root port power saving features (e.g.
 * clock gating), the Root port must either be disabled or a CLKREQ Pin
 * must be configured. The patch however removed that check before
 * enabling power management for the rootport.
 * Workaround (until FSP is fixed):
 *
 * Workaround:
 * pretend to FSP that the rootports have a CLKREQ Pin attached, by supplying them in the
 * FSP UPDs. That will cause FSP to configure the Pin or CLKREQ and enable power
 * management for said rootport, but it will not crash on L1 entry/exit. That has been done
 * on the Atlas board (as workaround) for a short period of time (before applying FSP Fix)
 * like this:
 *      - memupd->FspmConfig.PcieClkSrcUsage[2] = 4; // RP 5 (the rootport you want to fix)
 *      - memupd->FspmConfig.PcieClkSrcClkReq[2] = 0; // e.g. choose a clkreq pin that is not routed out
 */
static const struct pad_config gpio_table[] = {
	/*
	 * Do not program virtual wire CPU CLKREQ pins, since CLKREQ of cpu rootports are not
	 * connected on this mainboard.
	 */

	/* ------- GPIO Group GPP_A ------- */
	PAD_NC(GPP_A7, NONE),
	PAD_CFG_GPI(GPP_A8, NONE, DEEP), // HSID_0 / CLKREQ 7
	//PAD_NC(GPP_A12, NONE), CLKREQ 9B ???
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1), // USB_2_3_OC_N
	PAD_CFG_NF(GPP_A15, NONE, DEEP, NF1), // USB_4_5_OC_N
	PAD_CFG_NF(GPP_A16, NONE, DEEP, NF1), // USB_6_7_OC_N
	PAD_CFG_NF(GPP_A18, NONE, DEEP, NF1), // DP1_HPD
	PAD_CFG_NF(GPP_A19, NONE, DEEP, NF1), // DP2_HPD
	PAD_CFG_NF(GPP_A20, NONE, DEEP, NF1), // DP3_HPD
	PAD_NC(GPP_A23, NONE),                // ESPI_CS1

	/* ------- GPIO Group GPP_B ------- */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1), // AUX_VID0
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1), // AUX_VID1
	PAD_NC(GPP_B5, NONE),
	PAD_NC(GPP_B6, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_B8, NONE),
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1), // SATA_SPKR_N
	PAD_NC(GPP_B15, NONE),
	PAD_NC(GPP_B16, NONE),
	PAD_NC(GPP_B17, NONE),
	PAD_NC(GPP_B18, NONE),
	PAD_NC(GPP_B23, NONE),               // SML1_ALERT

	/* ------- GPIO Group GPP_C ------- */
	PAD_NC(GPP_C5, NONE),                // SML0_ALERT
	PAD_CFG_NF(GPP_C6, NONE, DEEP, NF1), // SML1_CLK
	PAD_CFG_NF(GPP_C7, NONE, DEEP, NF1), // SML1_DATA

	/* ------- GPIO Group GPP_D ------- */
	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_D1, NONE),
	PAD_NC(GPP_D2, NONE),
	PAD_NC(GPP_D3, NONE),
	//PAD_NC(GPP_D5, NONE), CLKREQ 0
	//PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1), // PHY0_CLKREQ / CLKREQ 1
	//PAD_NC(GPP_D7, NONE), CLKREQ 2
	//PAD_NC(GPP_D8, NONE), CLKREQ 3
	PAD_NC(GPP_D9, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D12, NONE),
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D19, NONE),

	/* ------- GPIO Group GPP_E ------- */
	//PAD_NC(GPP_E0, NONE), CLKREQ 9
	PAD_NC(GPP_E1, NONE),
	PAD_NC(GPP_E2, NONE),
	PAD_CFG_GPI(GPP_E3, NONE, DEEP),			/* TPM INT (todo: check) */
	PAD_NC(GPP_E4, NONE),
	PAD_NC(GPP_E5, NONE),
	PAD_CFG_GPI(GPP_E6, NONE, DEEP),
	PAD_CFG_GPI_SMI_LOW(GPP_E7, NONE, DEEP, EDGE_SINGLE),	/* EC SMI# */
	PAD_CFG_GPO(GPP_E8, 1, PLTRST),				/* PERST_CB_RESET_N */
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),			/* USB_0_1_OC_N */
	PAD_NC(GPP_E10, NONE),
	PAD_NC(GPP_E11, NONE),
	PAD_NC(GPP_E12, NONE),
	PAD_NC(GPP_E13, NONE),
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),			/* DP0_HPD (VGA_RED) */
	PAD_NC(GPP_E15, NONE),
	//PAD_NC(GPP_E16, NONE), CLKREQ 8
	PAD_NC(GPP_E17, NONE),
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),			/* DP3_DDC_CTRLCLK */
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF1),			/* DP3_DDC_CTRLDATA */
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),			/* DP2_DDC_CTRLCLK */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),			/* DP2_DDC_CTRLDATA */
	PAD_CFG_NF(GPP_E22, NONE, DEEP, NF1),			/* DP0_DDC_CTRLCLK */
	PAD_CFG_NF(GPP_E23, NONE, DEEP, NF1),			/* DP0_DDC_CTRLDATA */

	/* ------- GPIO Group GPP_F ------- */
	PAD_NC(GPP_F0, NONE),
	PAD_NC(GPP_F1, NONE),
	PAD_CFG_GPI(GPP_F2, NONE, DEEP),
	PAD_NC(GPP_F3, NONE),
	PAD_NC(GPP_F5, NONE),
	PAD_CFG_GPO(GPP_F9, 1, DEEP),				/* EC_SLP_S0_CS_N */
	PAD_NC(GPP_F11, NONE),
	PAD_CFG_GPI(GPP_F12, NONE, DEEP),
	PAD_CFG_GPI(GPP_F13, NONE, DEEP),
	PAD_NC(GPP_F17, NONE),
	PAD_NC(GPP_F18, NONE),
	PAD_CFG_GPI(GPP_F19, NONE, DEEP), // HSID_1 / CLKREQ 6
	PAD_NC(GPP_F20, NONE),
	PAD_NC(GPP_F21, NONE),
	PAD_CFG_GPO(GPP_F22, 1, DEEP),				/* PERST_PHY0_N */

	/* ------- GPIO Group GPP_H ------- */
	PAD_NC(GPP_H4, NONE),
	PAD_NC(GPP_H5, NONE),
	PAD_NC(GPP_H6, NONE),
	PAD_NC(GPP_H7, NONE),
	PAD_NC(GPP_H12, NONE),
	PAD_NC(GPP_H13, NONE),
	PAD_CFG_NF(GPP_H15, NONE, DEEP, NF1),			/* DDPB_CTRLCLK */
	PAD_CFG_NF(GPP_H17, NONE, DEEP, NF1),			/* DDPB_CTRLDATA */
	PAD_CFG_GPI(GPP_H19, NONE, DEEP), // HSID_3 / CLKREQ 4
	PAD_CFG_GPI(GPP_H23, NONE, DEEP), // HSID_2 / CLKREQ 5

	/* ------- GPIO Group GPP_R ------- */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF1), // HDA_BCLK
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF1), // HDA_SYNC
	PAD_CFG_NF(GPP_R2, NONE, DEEP, NF1), // HDA_SDOUT
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF1), // HDA_SDIN0
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF1), // HDA_RST
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF1), // HDA_SDIN1
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),

	/* ------- GPIO Group GPP_T ------- */
	PAD_NC(GPP_T2, NONE),
	PAD_NC(GPP_T3, NONE),

	/* ------- GPIO Group GPD ------- */
	PAD_NC(GPD8, NONE),					/* SUSCLK */
	PAD_CFG_GPO(GPD11, 1, PLTRST),				/* LAN_DISABLE_N */
};

void configure_gpio_pads(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
