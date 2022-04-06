/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <soc/nvs.h>
#include <string.h>
#include <types.h>

static struct global_nvs *gnvs;
static void *dnvs;

static void acpi_create_gnvs(void *unused)
{
	const size_t gnvs_size = ALIGN_UP(sizeof(struct global_nvs), sizeof(uint64_t));
	const size_t dnvs_size = ALIGN_UP(size_of_dnvs(), sizeof(uint64_t));

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (gnvs)
		return;

	/* Allocate for both GNVS and DNVS OpRegions. */
	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, gnvs_size + dnvs_size);
	if (!gnvs)
		return;

	memset(gnvs, 0, gnvs_size + dnvs_size);

	if (dnvs_size)
		dnvs = (char *)gnvs + gnvs_size;

	if (CONFIG(CONSOLE_CBMEM))
		gnvs->cbmc = (uintptr_t)cbmem_find(CBMEM_ID_CONSOLE);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, acpi_create_gnvs, NULL);

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

void *acpi_get_device_nvs(void)
{
	return dnvs;
}

/* Implemented under platform. */
__weak void soc_fill_gnvs(struct global_nvs *gnvs_) { }
__weak void mainboard_fill_gnvs(struct global_nvs *gnvs_) { }
__weak size_t size_of_dnvs(void) { return 0; }

/* Called from write_acpi_tables() only on normal boot path. */
void acpi_fill_gnvs(void)
{
	const struct opregion gnvs_op = OPREGION("GNVS", SYSTEMMEMORY, (uintptr_t)gnvs,
						 sizeof(struct global_nvs));
	const struct opregion dnvs_op = OPREGION("DNVS", SYSTEMMEMORY, (uintptr_t)dnvs,
						 size_of_dnvs());

	if (!gnvs)
		return;

	soc_fill_gnvs(gnvs);
	mainboard_fill_gnvs(gnvs);

	acpigen_write_scope("\\");
	acpigen_write_opregion(&gnvs_op);
	if (dnvs)
		acpigen_write_opregion(&dnvs_op);
	acpigen_pop_len();
}

int acpi_reset_gnvs_for_wake(struct global_nvs **gnvs_)
{
	if (!gnvs)
		return -1;

	/* Set unknown wake source */
	gnvs->pm1i = -1;
	gnvs->gpei = -1;

	*gnvs_ = gnvs;
	return 0;
}
