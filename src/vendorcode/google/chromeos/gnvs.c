/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <fmap.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>

#include "chromeos.h"
#include "gnvs.h"

static chromeos_acpi_t *chromeos_acpi;

static size_t chromeos_vpd_region(const char *region, uintptr_t *base)
{
	struct region_device vpd;

	if (fmap_locate_area_as_rdev(region, &vpd))
		return 0;

	*base = (uintptr_t)rdev_mmap_full(&vpd);

	return region_device_sz(&vpd);
}

void chromeos_init_chromeos_acpi(chromeos_acpi_t *init)
{
	size_t vpd_size;
	uintptr_t vpd_base = 0;

	chromeos_acpi = init;

	vpd_size = chromeos_vpd_region("RO_VPD", &vpd_base);
	if (vpd_size && vpd_base) {
		chromeos_acpi->vpd_ro_base = vpd_base;
		chromeos_acpi->vpd_ro_size = vpd_size;
	}

	vpd_size = chromeos_vpd_region("RW_VPD", &vpd_base);
	if (vpd_size && vpd_base) {
		chromeos_acpi->vpd_rw_base = vpd_base;
		chromeos_acpi->vpd_rw_size = vpd_size;
	}
}

void chromeos_set_me_hash(u32 *hash, int len)
{
	if ((len*sizeof(u32)) > sizeof(chromeos_acpi->mehh))
		return;

	/* Copy to NVS. */
	if (chromeos_acpi)
		memcpy(chromeos_acpi->mehh, hash, len*sizeof(u32));
}

chromeos_acpi_t *chromeos_get_chromeos_acpi(void)
{
	return chromeos_acpi;
}
