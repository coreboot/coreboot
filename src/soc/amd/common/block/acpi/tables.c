/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/chip.h>
#include <device/device.h>
#include <types.h>

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	return acpi_write_hpet(device, current, rsdp);
}

unsigned long acpi_fill_mcfg(unsigned long current)
{

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
					     CONFIG_MMCONF_BASE_ADDRESS,
					     0,
					     0,
					     CONFIG_MMCONF_BUS_NUMBER - 1);

	return current;
}

unsigned long acpi_fill_madt_irqoverride(unsigned long current)
{
	const struct soc_amd_common_config *cfg = soc_get_common_config();
	unsigned int i;
	uint8_t irq;
	uint8_t flags;

	for (i = 0; i < ARRAY_SIZE(cfg->irq_override); ++i) {
		irq = cfg->irq_override[i].irq;
		flags = cfg->irq_override[i].flags;

		if (!flags)
			continue;

		current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)current, 0,
							irq, irq, flags);
	}

	return current;
}
