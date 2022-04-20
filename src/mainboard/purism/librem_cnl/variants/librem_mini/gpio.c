/* SPDX-License-Identifier: GPL-2.0-only */

#include "../../variant.h"

/* Pad configuration was generated automatically using intelp2m utility */
static const struct pad_config gpio_table[] = {

	/* ------- GPIO Group GPP_A ------- */

	/* GPP_A0 - RCIN# */
	PAD_CFG_NF(GPP_A0, NONE, DEEP, NF1),

	/* GPP_A1 - LAD0 */
	PAD_CFG_NF(GPP_A1, NATIVE, DEEP, NF1),

	/* GPP_A2 - LAD1 */
	PAD_CFG_NF(GPP_A2, NATIVE, DEEP, NF1),

	/* GPP_A3 - LAD2 */
	PAD_CFG_NF(GPP_A3, NATIVE, DEEP, NF1),

	/* GPP_A4 - LAD3 */
	PAD_CFG_NF(GPP_A4, NATIVE, DEEP, NF1),

	/* GPP_A5 - LFRAME# */
	PAD_CFG_NF(GPP_A5, NONE, DEEP, NF1),

	/* GPP_A6 - SERIRQ */
	PAD_CFG_NF(GPP_A6, NONE, DEEP, NF1),

	/* GPP_A7 - NC */
	PAD_NC(GPP_A7, NONE),

	/* GPP_A8 - CLKRUN# */
	PAD_CFG_NF(GPP_A8, NONE, DEEP, NF1),

	/* GPP_A9 - CLKOUT_LPC0 */
	PAD_CFG_NF(GPP_A9, DN_20K, DEEP, NF1),

	/* GPP_A10 - CLKOUT_LPC1 */
	PAD_CFG_NF(GPP_A10, DN_20K, DEEP, NF1),

	/* GPP_A11 - NC */
	PAD_NC(GPP_A11, UP_20K),

	/* GPP_A12 - NC */
	PAD_NC(GPP_A12, UP_20K),

	/* GPP_A13 - NC */
	PAD_NC(GPP_A13, NONE),

	/* GPP_A14 - SUS_STAT# */
	PAD_CFG_NF(GPP_A14, NONE, DEEP, NF1),

	/* GPP_A15 - NC */
	PAD_NC(GPP_A15, NONE),

	/* GPP_A16 - NC */
	PAD_NC(GPP_A16, UP_20K),

	/* GPP_A17 - NC */
	PAD_NC(GPP_A17, UP_20K),

	/* GPP_A18 - NC */
	PAD_NC(GPP_A18, UP_20K),

	/* GPP_A19 - NC */
	PAD_NC(GPP_A19, UP_20K),

	/* GPP_A20 - NC */
	PAD_NC(GPP_A20, UP_20K),

	/* GPP_A21 - NC */
	PAD_NC(GPP_A21, UP_20K),

	/* GPP_A22 - NC */
	PAD_NC(GPP_A22, UP_20K),

	/* GPP_A23 - NC */
	PAD_NC(GPP_A23, UP_20K),

	/* ------- GPIO Group GPP_B ------- */

	/* GPP_B0 - Reserved */
	PAD_CFG_NF(GPP_B0, NONE, DEEP, NF1),

	/* GPP_B1 - Reserved */
	PAD_CFG_NF(GPP_B1, NONE, DEEP, NF1),

	/* GPP_B2 - NC */
	PAD_NC(GPP_B2, UP_20K),

	/* GPP_B3 - NC */
	PAD_NC(GPP_B3, NONE),

	/* GPP_B4 - NC */
	PAD_NC(GPP_B4, UP_20K),

	/* GPP_B5 - NC */
	PAD_NC(GPP_B5, NONE),

	/* GPP_B6 - SRCCLKREQ1# / SSD_CLK_REQ# */
	PAD_CFG_NF(GPP_B6, NONE, DEEP, NF1),

	/* GPP_B7 - NC */
	PAD_NC(GPP_B7, NONE),

	/* GPP_B8 - SRCCLKREQ3# / LAN2_CLK_REQ# */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF1),

	/* GPP_B9 - NC */
	PAD_NC(GPP_B9, NONE),

	/* GPP_B10 - NC */
	PAD_NC(GPP_B10, NONE),

	/* GPP_B11 - NC */
	PAD_NC(GPP_B11, NONE),

	/* GPP_B12 - SLP_S0# */
	PAD_CFG_NF(GPP_B12, NONE, DEEP, NF1),

	/* GPP_B13 - PLTRST# */
	PAD_CFG_NF(GPP_B13, NONE, DEEP, NF1),

	/* GPP_B14 - SPKR */
	PAD_CFG_NF(GPP_B14, NONE, DEEP, NF1),

	/* GPP_B15 - NC */
	PAD_NC(GPP_B15, NONE),

	/* GPP_B16 - NC */
	PAD_NC(GPP_B16, NONE),

	/* GPP_B17 - NC */
	PAD_NC(GPP_B17, NONE),

	/* GPP_B18 - NC */
	PAD_NC(GPP_B18, NONE),

	/* GPP_B19 - NC */
	PAD_NC(GPP_B19, NONE),

	/* GPP_B20 - NC */
	PAD_NC(GPP_B20, NONE),

	/* GPP_B21 - NC */
	PAD_NC(GPP_B21, NONE),

	/* GPP_B22 - NC */
	PAD_NC(GPP_B22, UP_20K),

	/* GPP_B23 - NC */
	PAD_NC(GPP_B23, UP_20K),

	/* ------- GPIO Group GPP_G ------- */

	/* GPP_G0 - NC */
	PAD_NC(GPP_G0, UP_20K),

	/* GPP_G1 - NC */
	PAD_NC(GPP_G1, NONE),

	/* GPP_G2 - NC */
	PAD_NC(GPP_G2, UP_20K),

	/* GPP_G3 - NC */
	PAD_NC(GPP_G3, UP_20K),

	/* GPP_G4 - NC */
	PAD_NC(GPP_G4, UP_20K),

	/* GPP_G5 - NC */
	PAD_NC(GPP_G5, UP_20K),

	/* GPP_G6 - NC */
	PAD_NC(GPP_G6, UP_20K),

	/* GPP_G7 - NC */
	PAD_NC(GPP_G7, DN_20K),

	/* ------- GPIO Group GPP_D ------- */

	/* GPP_D0 - NC */
	PAD_NC(GPP_D0, UP_20K),

	/* GPP_D1 - NC */
	PAD_NC(GPP_D1, UP_20K),

	/* GPP_D2 - NC */
	PAD_NC(GPP_D2, UP_20K),

	/* GPP_D3 - NC */
	PAD_NC(GPP_D3, UP_20K),

	/* GPP_D4 - NC */
	PAD_NC(GPP_D4, UP_20K),

	/* GPP_D5 - NC */
	PAD_NC(GPP_D5, NONE),

	/* GPP_D6 - NC */
	PAD_NC(GPP_D6, NONE),

	/* GPP_D7 - NC */
	PAD_NC(GPP_D7, UP_20K),

	/* GPP_D8 - NC */
	PAD_NC(GPP_D8, NONE),

	/* GPP_D9 - NC */
	PAD_NC(GPP_D9, NONE),

	/* GPP_D10 - NC */
	PAD_NC(GPP_D10, NONE),

	/* GPP_D11 - NC */
	PAD_NC(GPP_D11, UP_20K),

	/* GPP_D12 - NC */
	PAD_NC(GPP_D12, UP_20K),

	/* GPP_D13 - NC */
	PAD_NC(GPP_D13, DN_20K),

	/* GPP_D14 - NC */
	PAD_NC(GPP_D14, DN_20K),

	/* GPP_D15 - NC */
	PAD_NC(GPP_D15, UP_20K),

	/* GPP_D16 - NC */
	PAD_NC(GPP_D16, UP_20K),

	/* GPP_D17 - NC */
	PAD_NC(GPP_D17, NONE),

	/* GPP_D18 - NC */
	PAD_NC(GPP_D18, NONE),

	/* GPP_D19 - NC */
	PAD_NC(GPP_D19, NONE),

	/* GPP_D20 - NC */
	PAD_NC(GPP_D20, NONE),

	/* GPP_D21 - NC */
	PAD_NC(GPP_D21, UP_20K),

	/* GPP_D22 - NC */
	PAD_NC(GPP_D22, UP_20K),

	/* GPP_D23 - NC */
	PAD_NC(GPP_D23, UP_20K),

	/* ------- GPIO Group GPP_F ------- */

	/* GPP_F0 - NC */
	PAD_NC(GPP_F0, UP_20K),

	/* GPP_F1 - NC */
	PAD_NC(GPP_F1, UP_20K),

	/* GPP_F2 - NC */
	PAD_NC(GPP_F2, UP_20K),

	/* GPP_F3 - NC */
	PAD_NC(GPP_F3, UP_20K),

	/* GPP_F4 - NC */
	PAD_NC(GPP_F4, UP_20K),

	/* GPP_F5 - NC */
	PAD_NC(GPP_F5, UP_20K),

	/* GPP_F6 - NC */
	PAD_NC(GPP_F6, NONE),

	/* GPP_F7 - NC */
	PAD_NC(GPP_F7, NONE),

	/* GPP_F8 - NC */
	PAD_NC(GPP_F8, UP_20K),

	/* GPP_F9 - NC */
	PAD_NC(GPP_F9, UP_20K),

	/* GPP_F10 - NC */
	PAD_NC(GPP_F10, UP_20K),

	/* GPP_F11 - NC */
	PAD_NC(GPP_F11, UP_20K),

	/* GPP_F12 - NC */
	PAD_NC(GPP_F12, UP_20K),

	/* GPP_F13 - NC */
	PAD_NC(GPP_F13, UP_20K),

	/* GPP_F14 - NC */
	PAD_NC(GPP_F14, UP_20K),

	/* GPP_F15 - NC */
	PAD_NC(GPP_F15, UP_20K),

	/* GPP_F16 - NC */
	PAD_NC(GPP_F16, UP_20K),

	/* GPP_F17 - NC */
	PAD_NC(GPP_F17, UP_20K),

	/* GPP_F18 - NC */
	PAD_NC(GPP_F18, UP_20K),

	/* GPP_F19 - NC */
	PAD_NC(GPP_F19, UP_20K),

	/* GPP_F20 - NC */
	PAD_NC(GPP_F20, UP_20K),

	/* GPP_F21 - NC */
	PAD_NC(GPP_F21, UP_20K),

	/* GPP_F22 - NC */
	PAD_NC(GPP_F22, UP_20K),

	/* GPP_F23 - NC */
	PAD_NC(GPP_F23, UP_20K),

	/* ------- GPIO Group GPP_H ------- */

	/* GPP_H0 - NC */
	PAD_NC(GPP_H0, UP_20K),

	/* GPP_H1 - NC# */
	PAD_NC(GPP_H1, NONE),

	/* GPP_H2 - NC */
	PAD_NC(GPP_H2, NONE),

	/* GPP_H3 - NC */
	PAD_NC(GPP_H3, UP_20K),

	/* GPP_H4 - NC */
	PAD_NC(GPP_H4, UP_20K),

	/* GPP_H5 - NC */
	PAD_NC(GPP_H5, UP_20K),

	/* GPP_H6 - NC */
	PAD_NC(GPP_H6, UP_20K),

	/* GPP_H7 - NC */
	PAD_NC(GPP_H7, UP_20K),

	/* GPP_H8 - NC */
	PAD_NC(GPP_H8, UP_20K),

	/* GPP_H9 - NC */
	PAD_NC(GPP_H9, UP_20K),

	/* GPP_H10 - NC */
	PAD_NC(GPP_H10, NONE),

	/* GPP_H11 - NC */
	PAD_NC(GPP_H11, NONE),

	/* GPP_H12 - NC */
	PAD_NC(GPP_H12, UP_20K),

	/* GPP_H13 - NC */
	PAD_NC(GPP_H13, UP_20K),

	/* GPP_H14 - NC */
	PAD_NC(GPP_H14, UP_20K),

	/* GPP_H15 - NC */
	PAD_NC(GPP_H15, UP_20K),

	/* GPP_H16 - NC */
	PAD_NC(GPP_H16, UP_20K),

	/* GPP_H17 - NC */
	PAD_NC(GPP_H17, UP_20K),

	/* GPP_H18 - NC */
	PAD_NC(GPP_H18, UP_20K),

	/* GPP_H19 - NC */
	PAD_NC(GPP_H19, UP_20K),

	/* GPP_H20 - NC */
	PAD_NC(GPP_H20, UP_20K),

	/* GPP_H21 - NC */
	PAD_NC(GPP_H21, NONE),

	/* GPP_H22 - NC */
	PAD_NC(GPP_H22, UP_20K),

	/* GPP_H23 - NC */
	PAD_NC(GPP_H23, NONE),

	/* ------- GPIO Group GPD ------- */

	/* GPD0 - BATLOW# */
	PAD_CFG_NF(GPD0, NONE, DEEP, NF1),

	/* GPD1 - ACPRESENT */
	PAD_CFG_NF(GPD1, NATIVE, DEEP, NF1),

	/* GPD2 - NC */
	PAD_NC(GPD2, NONE),

	/* GPD3 - PRWBTN# */
	PAD_CFG_NF(GPD3, UP_20K, DEEP, NF1),

	/* GPD4 - SLP_S3# */
	PAD_CFG_NF(GPD4, NONE, DEEP, NF1),

	/* GPD5 - SLP_S4# */
	PAD_CFG_NF(GPD5, NONE, DEEP, NF1),

	/* GPD6 - NC */
	PAD_NC(GPD6, UP_20K),

	/* GPD7 - NC */
	PAD_NC(GPD7, NONE),

	/* GPD8 - SUSCLK */
	PAD_CFG_NF(GPD8, NONE, DEEP, NF1),

	/* GPD9 - NC */
	PAD_NC(GPD9, UP_20K),

	/* GPD10 - NC */
	PAD_NC(GPD10, UP_20K),

	/* GPD11 - NC */
	PAD_NC(GPD11, UP_20K),

	/* ------- GPIO Group GPP_C ------- */

	/* GPP_C0 - SMBCLK */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),

	/* GPP_C1 - SMBDATA */
	PAD_CFG_NF(GPP_C1, NONE, DEEP, NF1),

	/* GPP_C2 - NC */
	PAD_NC(GPP_C2, NONE),

	/* GPP_C3 - NC */
	PAD_NC(GPP_C3, NONE),

	/* GPP_C4 - NC */
	PAD_NC(GPP_C4, NONE),

	/* GPP_C5 - NC */
	PAD_NC(GPP_C5, UP_20K),

	/* GPP_C6 - NC */
	PAD_NC(GPP_C6, NONE),

	/* GPP_C7 - NC */
	PAD_NC(GPP_C7, NONE),

	/* GPP_C8 - NC */
	PAD_NC(GPP_C8, NONE),

	/* GPP_C9 - NC */
	PAD_NC(GPP_C9, NONE),

	/* GPP_C10 - NC */
	PAD_NC(GPP_C10, UP_20K),

	/* GPP_C11 - NC */
	PAD_NC(GPP_C11, UP_20K),

	/* GPP_C12 - NC */
	PAD_NC(GPP_C12, UP_20K),

	/* GPP_C13 - NC */
	PAD_NC(GPP_C13, UP_20K),

	/* GPP_C14 - NC */
	PAD_NC(GPP_C14, UP_20K),

	/* GPP_C15 - NC */
	PAD_NC(GPP_C15, UP_20K),

	/* GPP_C16 - NC */
	PAD_NC(GPP_C16, NONE),

	/* GPP_C17 - NC */
	PAD_NC(GPP_C17, NONE),

	/* GPP_C18 - NC */
	PAD_NC(GPP_C18, UP_20K),

	/* GPP_C19 - NC */
	PAD_NC(GPP_C19, UP_20K),

	/* GPP_C20 - NC */
	PAD_NC(GPP_C20, NONE),

	/* GPP_C21 - NC */
	PAD_NC(GPP_C21, NONE),

	/* GPP_C22 - USB3_P1_PWREN */
	PAD_CFG_GPO(GPP_C22, 1, PLTRST),

	/* GPP_C23 - NC */
	PAD_NC(GPP_C23, UP_20K),

	/* ------- GPIO Group GPP_E ------- */

	/* GPP_E0 - NC */
	PAD_NC(GPP_E0, UP_20K),

	/* GPP_E1 - NC */
	PAD_NC(GPP_E1, UP_20K),

	/* GPP_E2 - SATAXPCIE2 */
	PAD_CFG_NF(GPP_E2, UP_20K, PLTRST, NF1),

	/* GPP_E3 - NC */
	PAD_NC(GPP_E3, UP_20K),

	/* GPP_E4 - SATA_DEVSLP0 */
	PAD_CFG_NF(GPP_E4, NONE, PLTRST, NF1),

	/* GPP_E5 - NC */
	PAD_NC(GPP_E5, UP_20K),

	/* GPP_E6 - NC */
	PAD_NC(GPP_E6, UP_20K),

	/* GPP_E7 - NC */
	PAD_NC(GPP_E7, NONE),

	/* GPP_E8 - STATUSLED# */
	PAD_CFG_GPO(GPP_E8, 1, PLTRST),

	/* GPP_E9 - USB2_OC0# */
	PAD_CFG_NF(GPP_E9, NONE, DEEP, NF1),

	/* GPP_E10 - USB2_OC1# */
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF1),

	/* GPP_E11 - USB2_OC2# */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF1),

	/* GPP_E12 - USB2_OC3# */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),

	/* GPP_E13 - DDPB_HPD0 */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),

	/* GPP_E14 - DDPC_HPD1 */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF1),

	/* GPP_E15 - NC */
	PAD_NC(GPP_E15, NONE),

	/* GPP_E16 - GPIO */
	PAD_CFG_GPI_SCI(GPP_E16, UP_20K, PLTRST, LEVEL, INVERT),

	/* GPP_E17 - NC */
	PAD_NC(GPP_E17, NONE),

	/* GPP_E18 - DPPB_CTRLCLK */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF1),

	/* GPP_E19 - DPPB_CTRLDATA */
	PAD_CFG_NF(GPP_E19, NONE, DEEP, NF1),

	/* GPP_E20 - NC */
	PAD_NC(GPP_E20, NONE),

	/* GPP_E21 - NC */
	PAD_NC(GPP_E21, NONE),

	/* GPP_E22 - NC */
	PAD_NC(GPP_E22, UP_20K),

	/* GPP_E23 - NC */
	PAD_NC(GPP_E23, NONE),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
