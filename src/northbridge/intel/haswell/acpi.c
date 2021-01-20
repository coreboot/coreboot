/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <acpi/acpi.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include "haswell.h"
#include <southbridge/intel/lynxpoint/pch.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
			CONFIG_MMCONF_BASE_ADDRESS, 0, 0, CONFIG_MMCONF_BUS_NUMBER - 1);

	return current;
}

static unsigned long acpi_fill_dmar(unsigned long current)
{
	struct device *const igfx_dev = pcidev_on_root(2, 0);
	const u32 gfxvtbar = MCHBAR32(GFXVTBAR) & ~0xfff;
	const u32 vtvc0bar = MCHBAR32(VTVC0BAR) & ~0xfff;
	const bool gfxvten = MCHBAR32(GFXVTBAR) & 0x1;
	const bool vtvc0en = MCHBAR32(VTVC0BAR) & 0x1;

	/* iGFX has to be enabled; GFXVTBAR set, enabled, in 32-bit space */
	if (igfx_dev && igfx_dev->enabled && gfxvtbar && gfxvten && !MCHBAR32(GFXVTBAR + 4)) {

		const unsigned long tmp = current;

		current += acpi_create_dmar_drhd(current, 0, 0, gfxvtbar);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	/* VTVC0BAR has to be set, enabled, and in 32-bit space */
	if (vtvc0bar && vtvc0en && !MCHBAR32(VTVC0BAR + 4)) {

		const unsigned long tmp = current;
		current += acpi_create_dmar_drhd(current, DRHD_INCLUDE_PCI_ALL, 0, vtvc0bar);
		current += acpi_create_dmar_ds_ioapic(current, 2, PCH_IOAPIC_PCI_BUS,
						      PCH_IOAPIC_PCI_SLOT, 0);

		size_t i;
		for (i = 0; i < 8; ++i)
			current += acpi_create_dmar_ds_msi_hpet(current, 0, PCH_HPET_PCI_BUS,
								PCH_HPET_PCI_SLOT, i);
		acpi_dmar_drhd_fixup(tmp, current);
	}

	return current;
}

unsigned long northbridge_write_acpi_tables(const struct device *const dev,
					    unsigned long current,
					    struct acpi_rsdp *const rsdp)
{
	/* Create DMAR table only if we have VT-d capability. */
	const u32 capid0_a = pci_read_config32(dev, CAPID0_A);
	if (capid0_a & VTD_DISABLE)
		return current;

	acpi_dmar_t *const dmar = (acpi_dmar_t *)current;
	printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
	acpi_create_dmar(dmar, DMAR_INTR_REMAP, acpi_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	return current;
}
