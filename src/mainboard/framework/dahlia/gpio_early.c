/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <gpio.h>
#include <mainboard/bootblock.h>

static const struct pad_config gpio_table[] = {
	/* UART0 for console */
	PAD_CFG_NF(GPP_H08, NONE, DEEP, NF1),	/* UART0_RXD */
	PAD_CFG_NF(GPP_H09, NONE, DEEP, NF1),	/* UART0_TXD */

	/* SMBus for DDR5 SPD (SOC_SMBCLK to SO-DIMM) */
	PAD_CFG_NF(GPP_C00, NONE, DEEP, NF1),	/* SMBCLK */
	PAD_CFG_NF(GPP_C01, NONE, DEEP, NF1),	/* SMBDATA */

	/*
	 * Power sequencing for the NVMe SSD and the touchscreen:
	 * Turn the rails on here and hold both devices in reset.
	 * The ramstage table in gpio.c releases the resets.
	 * FSP-M delay should give both enough time to power on before reset is
	 * released.
	 */
	PAD_CFG_GPO(GPP_H18, 1, DEEP),		/* SOC_SSD_EN - SSD rail on */
	PAD_CFG_GPO(GPP_A08, 0, DEEP),		/* RTD3_SSD_PLT_RST# - hold SSD in reset */
	PAD_CFG_GPO(GPP_E14, 1, PLTRST),	/* TCHSCR_PWR_EN - touchscreen rail on */
	PAD_CFG_GPO(GPP_F10, 1, DEEP),		/* SOC_TCHSCR_EN - panel-side enable (via eDP connector) */
	PAD_CFG_GPO(GPP_E16, 0, PLTRST),	/* SOC_TCHSCR_RST# - hold touchscreen in reset */
};

void mainboard_configure_early_gpios(void)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
