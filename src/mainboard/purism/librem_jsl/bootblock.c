/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <delay.h>
#include <device/pci_ops.h>
#include <ec/acpi/ec.h>
#include <gpio.h>
#include <soc/pci_devs.h>

static const struct pad_config early_gpio_table[] = {
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),	/* UART2_RXD */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),	/* UART2_TXD */
};

void bootblock_mainboard_early_init(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

void bootblock_mainboard_init(void)
{
	/*
	 * Ensure the panel is powered on.  ACPI _PTS disables it, if the system
	 * is powered up again we need to turn it on.
	 */
	ec_write(0xa3, 0);
}
