/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>

static void power_off_lte_module(void)
{
	gpio_output(GPP_H17, 0);
	mdelay(20);
	gpio_output(GPP_A10, 0);
}

void variant_smi_sleep(u8 slp_typ)
{
	/*
	 * Once the FW_CONFIG is provisioned, power off LTE module only under
	 * the situation where it is stuffed.
	 */
	if (slp_typ == ACPI_S5)
		power_off_lte_module();
}
