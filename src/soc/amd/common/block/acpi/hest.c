/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_apei.h>
#include <acpi/acpi_gnvs.h>
#include <cbmem.h>
#include <console/console.h>

uintptr_t acpi_soc_fill_hest(acpi_hest_t *hest, uintptr_t current, void *log_mem)
{
	hest->error_source_count = 0;
	return current;
}
