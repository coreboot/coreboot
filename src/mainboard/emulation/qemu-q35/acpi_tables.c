/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>

#include "../qemu-i440fx/fw_cfg.h"
#include "../qemu-i440fx/acpi.h"

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	/* Do not advertise SMI even if installed. */
	fadt->smi_cmd = 0;
	fadt->acpi_enable = 0;
	fadt->acpi_disable = 0;
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
					     CONFIG_MMCONF_BASE_ADDRESS, 0, 0,
					     CONFIG_MMCONF_BUS_NUMBER - 1);
	return current;
}
