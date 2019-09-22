/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include "include/gpio.h"

/* Early pad configuration */
static const struct pad_config early_gpio_table[] = {
	// GPIO (ISH_GP2) = DGPU_PRESENT
	PAD_CFG_GPI_TRIG_OWN(GPP_A20, NONE, DEEP, OFF, ACPI),
	// GPIO (VRALERT#) <= DGPU_PWROK
	PAD_CFG_GPI_TRIG_OWN(GPP_B2, NONE, DEEP, OFF, ACPI),
	// GPIO (CPU_GP3) => DGPU_HOLD_RST#
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	// GPIO (GSPI1_MISO) => DGPU_PWR_EN#
	PAD_CFG_TERM_GPO(GPP_B21, 1, DN_20K, DEEP),
	// UART2_RXD = LPSS_UART2_RXD
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	// UART2_TXD = LPSS_UART2_TXD
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	// SATALED# = SATA_LED#
	PAD_CFG_NF(GPP_E8, NONE, DEEP, NF1),
};

void mainboard_config_stage_gpios(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}
