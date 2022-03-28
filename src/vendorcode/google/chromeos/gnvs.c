/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <bootmode.h>
#include <types.h>
#include <string.h>
#include <stdlib.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <fmap.h>
#include <security/vboot/vbnv.h>
#include <security/vboot/vboot_common.h>
#include <smbios.h>

#include "chromeos.h"
#include "gnvs.h"

static struct chromeos_acpi *chromeos_acpi;

static size_t chromeos_vpd_region(const char *region, uintptr_t *base)
{
	struct region_device vpd;

	if (fmap_locate_area_as_rdev(region, &vpd))
		return 0;

	*base = (uintptr_t)rdev_mmap_full(&vpd);

	return region_device_sz(&vpd);
}

void chromeos_init_chromeos_acpi(void)
{
	size_t vpd_size;
	uintptr_t vpd_base = 0;

	chromeos_acpi = cbmem_add(CBMEM_ID_ACPI_CNVS, sizeof(struct chromeos_acpi));
	if (!chromeos_acpi)
		return;

	/* Retain CNVS contents on S3 resume path. */
	if (acpi_is_wakeup_s3())
		return;

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

void chromeos_set_ramoops(void *ram_oops, size_t size)
{
	if (!chromeos_acpi)
		return;

	printk(BIOS_DEBUG, "Ramoops buffer: 0x%zx@%p.\n", size, ram_oops);
	chromeos_acpi->ramoops_base = (uintptr_t)ram_oops;
	chromeos_acpi->ramoops_len = size;
}

void smbios_type0_bios_version(uintptr_t address)
{
	if (!chromeos_acpi)
		return;
	/* Location of smbios_type0.bios_version() string filled with spaces. */
	chromeos_acpi->vbt10 = address;
}

void acpi_fill_cnvs(void)
{
	const struct opregion cnvs_op = OPREGION("CNVS", SYSTEMMEMORY, (uintptr_t)chromeos_acpi,
						 sizeof(*chromeos_acpi));

	if (!chromeos_acpi)
		return;

	acpigen_write_scope("\\");
	acpigen_write_opregion(&cnvs_op);
	acpigen_pop_len();

	/* Usually this creates OIPG package for GPIOs. */
	mainboard_chromeos_acpi_generate();
}
