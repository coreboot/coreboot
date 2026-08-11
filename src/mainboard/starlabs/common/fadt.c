/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <ec/starlabs/merlin/ec.h>

void mainboard_fill_fadt(acpi_fadt_t *fadt)
{
	/* Both supported EC families use the same fixed PNP address and keyboard LDN. */
	if (is_dev_enabled(dev_find_slot_pnp(ITE_FIXED_ADDR, ITE_KBCK)))
		fadt->iapc_boot_arch |= ACPI_FADT_8042;
}
