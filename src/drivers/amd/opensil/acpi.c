/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <vendorcode/amd/opensil/opensil.h>

#include "opensil.h"

void amd_opensil_fill_fadt_io_ports(acpi_fadt_t *fadt)
{
	opensil_fill_fadt(fadt);
}

unsigned long acpi_add_opensil_tables(unsigned long current, acpi_rsdp_t *rsdp)
{
	return add_opensil_acpi_table(current, rsdp);
}
