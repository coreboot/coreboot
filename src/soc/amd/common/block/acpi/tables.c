/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/acpi.h>
#include <amdblocks/chip.h>
#include <assert.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <types.h>

unsigned long southbridge_write_acpi_tables(const struct device *device,
		unsigned long current,
		struct acpi_rsdp *rsdp)
{
	return acpi_write_hpet(device, current, rsdp);
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

/* Used by \_SB.PCI0._CRS */
void acpi_fill_root_complex_tom(const struct device *device)
{
	msr_t msr;
	const char *scope;

	assert(device);

	scope = acpi_device_scope(device);
	assert(scope);
	acpigen_write_scope(scope);

	msr = rdmsr(TOP_MEM);
	acpigen_write_name_dword("TOM1", msr.lo);
	msr = rdmsr(TOP_MEM2);
	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", (msr.hi << 12) | msr.lo >> 20);
	acpigen_pop_len();
}
