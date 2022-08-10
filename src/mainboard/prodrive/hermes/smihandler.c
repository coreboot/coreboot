/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <intelblocks/gpio.h>
#include <intelblocks/smihandler.h>
#include <soc/gpio.h>

static const struct pad_config sgpio_table[] = {
	PAD_CFG_GPO(GPP_F10, 0, DEEP),
	PAD_CFG_GPO(GPP_F11, 0, DEEP),
	PAD_CFG_GPO(GPP_F12, 0, DEEP),
	PAD_CFG_GPO(GPP_F13, 0, DEEP),
};

void mainboard_smi_sleep(u8 slp_typ)
{
	/*
	 * Pull SGPIO pins low to prevent cross-powering the +5V rail
	 * through the SATA backplane when the power supply is off.
	 */
	if (slp_typ >= ACPI_S3)
		gpio_configure_pads(sgpio_table, ARRAY_SIZE(sgpio_table));
}
