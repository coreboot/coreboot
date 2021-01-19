/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <cbmem.h>
#include <console/console.h>
#include <soc/nvs.h>
#include <stdint.h>
#include <string.h>
#include <types.h>
#include <vendorcode/google/chromeos/gnvs.h>

static struct global_nvs *gnvs;

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

__weak void soc_fill_gnvs(struct global_nvs *gnvs_) { }
__weak void mainboard_fill_gnvs(struct global_nvs *gnvs_) { }

void acpi_create_gnvs(void)
{
	size_t gnvs_size;

	if (cbmem_find(CBMEM_ID_ACPI_GNVS))
		return;

	/* Match with OpRegion declared in global_nvs.asl. */
	gnvs_size = sizeof(struct global_nvs);
	if (gnvs_size < 0x100)
		gnvs_size = 0x100;
	if (CONFIG(ACPI_HAS_DEVICE_NVS))
		gnvs_size = 0x2000;
	else if (CONFIG(MAINBOARD_HAS_CHROMEOS))
		gnvs_size = 0x1000;

	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, gnvs_size);
	if (!gnvs)
		return;

	memset(gnvs, 0, gnvs_size);

	if (CONFIG(CONSOLE_CBMEM))
		gnvs->cbmc = (uintptr_t)cbmem_find(CBMEM_ID_CONSOLE);

	if (CONFIG(CHROMEOS))
		gnvs_assign_chromeos((u8 *)gnvs + GNVS_CHROMEOS_ACPI_OFFSET);
}

void *acpi_get_device_nvs(void)
{
	return (u8 *)gnvs + GNVS_DEVICE_NVS_OFFSET;
}

void acpi_fill_gnvs(void)
{
	if (!gnvs)
		return;

	soc_fill_gnvs(gnvs);
	mainboard_fill_gnvs(gnvs);

	acpigen_write_scope("\\");
	acpigen_write_name_dword("NVB0", (uintptr_t)gnvs);
	acpigen_write_name_dword("NVS0", CONFIG(MAINBOARD_HAS_CHROMEOS) ? 0x1000 : 0x100);
	acpigen_pop_len();

	if (CONFIG(ACPI_HAS_DEVICE_NVS)) {
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVB1", (uintptr_t)gnvs + GNVS_DEVICE_NVS_OFFSET);
		acpigen_write_name_dword("NVS1", 0x1000);
		acpigen_pop_len();
	}
}
