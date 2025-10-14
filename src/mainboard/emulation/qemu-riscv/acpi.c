/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <smbios.h>

smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	return SMBIOS_ENCLOSURE_MAIN_SERVER_CHASSIS;
}

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
}

unsigned long acpi_fill_madt(unsigned long current)
{
	return current;
}
