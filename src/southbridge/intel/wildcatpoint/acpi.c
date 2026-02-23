/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <soc/device_nvs.h>
#include <types.h>

size_t size_of_dnvs(void)
{
	return sizeof(struct device_nvs);
}
