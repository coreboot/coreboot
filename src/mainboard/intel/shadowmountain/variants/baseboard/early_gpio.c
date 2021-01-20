/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {

	/* UART0 RX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* UART0 TX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* A7 : MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_A7, NONE, DEEP),
	/* A17 : MEM_CH_SEL */
	PAD_CFG_GPI(GPP_A17, NONE, DEEP),
	/* A19 : MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_A19, NONE, DEEP),
	/* A20 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_A20, NONE, DEEP),
	/* B11 : PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_B11, NONE, DEEP),
	/* C0 : EN_PP3300_WLAN */
	PAD_CFG_GPO(GPP_C0, 1, DEEP),
	/* C3 : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C3, NONE, PLTRST, LEVEL, INVERT),
	/* D10 : EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_D10, 1, DEEP),
	/* D15 : MEM_STRAP_3 */
	PAD_NC(GPP_D15, NONE),
	/* D16 : EN_PP3300_SD */
	PAD_NC(GPP_D16, UP_20K),
	/* E10 : PCH_GSPI0_H1_TPM_CS_L */
	PAD_CFG_NF(GPP_E10, NONE, DEEP, NF7),
	/* E11 : PCH_GSPI0_H1_TPM_CLK */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF7),
	/* E12 : PCH_GSPIO_H1_TPM_MISO */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF7),
	/* E13 : PCH_GSPI0_H1_TPM_MOSI_STRAP */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF7),
	/* F14 : WLAN_PERST_L */
	PAD_CFG_GPO(GPP_F14, 1, DEEP),
	/* F20 : WWAN_RST_ODL
	   To meet timing constraints - drive reset low.
	   Deasserted in ramstage. */
	PAD_CFG_GPO(GPP_F20, 0, DEEP),
};

void variant_configure_early_gpio_pads(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
