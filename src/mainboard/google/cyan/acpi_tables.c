/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <arch/ioapic.h>
#include <boardid.h>
#include <soc/acpi.h>
#include <soc/nvs.h>
#include <soc/device_nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* Enable DPTF */
	gnvs->dpte = 1;

	/* Disable PMIC I2C port for ACPI for all boards except cyan */
	struct device_nvs *dev_nvs = acpi_get_device_nvs();
	if (!CONFIG(BOARD_GOOGLE_CYAN))
		dev_nvs->lpss_en[LPSS_NVS_I2C2] = 0;

	gnvs->bdid = board_id();
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	current = acpi_madt_irq_overrides(current);

	return current;
}
