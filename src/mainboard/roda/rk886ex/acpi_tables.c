/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <southbridge/intel/i82801gx/nvs.h>

void acpi_create_gnvs(struct global_nvs *gnvs)
{
	/* Enable both COM ports */
	gnvs->cmap = 0x01;
	gnvs->cmbp = 0x01;

}
