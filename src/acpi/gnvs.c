/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <cbmem.h>
#include <console/console.h>
#include <string.h>
#include <types.h>
#include <vendorcode/google/chromeos/gnvs.h>

static void *gnvs;

void *acpi_get_gnvs(void)
{
	if (gnvs)
		return gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (gnvs)
		return gnvs;

	printk(BIOS_ERR, "Unable to locate Global NVS\n");
	return NULL;
}

static void gnvs_assign_cbmc(void)
{
	uint32_t *gnvs_cbmc = gnvs_cbmc_ptr(gnvs);
	if (gnvs_cbmc)
		*gnvs_cbmc = (uintptr_t)cbmem_find(CBMEM_ID_CONSOLE);
}

/* Needs implementation in platform code. */
__weak uint32_t *gnvs_cbmc_ptr(struct global_nvs *gnvs_)
{
	return NULL;
}

__weak void soc_fill_gnvs(struct global_nvs *gnvs_) { }
__weak void mainboard_fill_gnvs(struct global_nvs *gnvs_) { }

void *gnvs_get_or_create(void)
{
	size_t gnvs_size;

	if (gnvs)
		return gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (gnvs)
		return gnvs;

	gnvs_size = gnvs_size_of_array();
	if (!gnvs_size)
		return NULL;

	/* Match with OpRegion declared in global_nvs.asl. */
	if (gnvs_size < 0x100)
		gnvs_size = 0x100;
	if (gnvs_size > 0x1000)
		gnvs_size = 0x2000;
	else if (CONFIG(MAINBOARD_HAS_CHROMEOS))
		gnvs_size = 0x1000;

	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, gnvs_size);
	if (!gnvs)
		return NULL;

	memset(gnvs, 0, gnvs_size);

	if (CONFIG(CONSOLE_CBMEM))
		gnvs_assign_cbmc();

	if (CONFIG(CHROMEOS))
		gnvs_assign_chromeos((u8 *)gnvs + GNVS_CHROMEOS_ACPI_OFFSET);

	return gnvs;
}

void acpi_fill_gnvs(void)
{
	if (!gnvs)
		return;

	soc_fill_gnvs(gnvs);
	mainboard_fill_gnvs(gnvs);

	acpigen_write_scope("\\");
	acpigen_write_name_dword("NVSA", (uintptr_t)gnvs);
	acpigen_pop_len();
}
