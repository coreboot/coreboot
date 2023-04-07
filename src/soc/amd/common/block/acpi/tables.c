/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/acpi.h>
#include <amdblocks/chip.h>
#include <assert.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <types.h>

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	return acpi_write_hpet(device, current, rsdp);
}

/* Used by \_SB.PCI0._CRS */
void acpi_fill_root_complex_tom(const struct device *device)
{
	const char *scope;

	assert(device);

	scope = acpi_device_scope(device);
	assert(scope);
	acpigen_write_scope(scope);

	acpigen_write_name_dword("TOM1", get_top_of_mem_below_4gb());

	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", get_top_of_mem_above_4gb() >> 20);
	acpigen_pop_len();
}
