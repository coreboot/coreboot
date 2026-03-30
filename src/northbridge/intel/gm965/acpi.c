/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

#include "gm965.h"

unsigned long northbridge_write_acpi_tables(const struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp)
{
	/* GM965 (Crestline) does not support VT-d, so no DMAR table. */
	return start;
}
