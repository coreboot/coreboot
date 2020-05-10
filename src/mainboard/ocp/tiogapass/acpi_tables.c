/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/acpi.h>
#include <soc/acpi.h>

extern const unsigned char AmlCode[];

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	acpi_init_gnvs(gnvs);
}
