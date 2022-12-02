/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <cpu/intel/haswell/haswell.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>
#include <cpu/intel/turbo.h>
#include <soc/acpi.h>
#include <soc/device_nvs.h>
#include <soc/iomap.h>
#include <soc/lpc.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/systemagent.h>
#include <soc/intel/broadwell/chip.h>

static unsigned long acpi_fill_dmar(unsigned long current)
{
	struct device *const igfx_dev = pcidev_path_on_root(SA_DEVFN_IGD);
	const u32 gfxvtbar = mchbar_read32(GFXVTBAR) & ~0xfff;
	const u32 vtvc0bar = mchbar_read32(VTVC0BAR) & ~0xfff;
	const bool gfxvten = mchbar_read32(GFXVTBAR) & 0x1;
	const bool vtvc0en = mchbar_read32(VTVC0BAR) & 0x1;

	/* iGFX has to be enabled; GFXVTBAR set, enabled, in 32-bit space */
	const bool emit_igd =
			igfx_dev && igfx_dev->enabled &&
			gfxvtbar && gfxvten &&
			!mchbar_read32(GFXVTBAR + 4);

	/* First, add DRHD entries */
	if (emit_igd) {
		const unsigned long tmp = current;

		current += acpi_create_dmar_drhd(current, 0, 0, gfxvtbar);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);

		acpi_dmar_drhd_fixup(tmp, current);
	}

	/* VTVC0BAR has to be set, enabled, and in 32-bit space */
	if (vtvc0bar && vtvc0en && !mchbar_read32(VTVC0BAR + 4)) {
		const unsigned long tmp = current;
		current += acpi_create_dmar_drhd(current,
				DRHD_INCLUDE_PCI_ALL, 0, vtvc0bar);
		current += acpi_create_dmar_ds_ioapic_from_hw(current,
				IO_APIC_ADDR, PCH_IOAPIC_PCI_BUS, PCH_IOAPIC_PCI_SLOT, 0);
		size_t i;
		for (i = 0; i < 8; ++i)
			current += acpi_create_dmar_ds_msi_hpet(current,
					0, PCH_HPET_PCI_BUS,
					PCH_HPET_PCI_SLOT, i);
		acpi_dmar_drhd_fixup(tmp, current);
	}

	/* Then, add RMRR entries after all DRHD entries */
	if (emit_igd) {
		const unsigned long tmp = current;

		const struct device *sa_dev = pcidev_on_root(0, 0);

		/* Bit 0 is lock bit, not part of address */
		const u32 tolud = pci_read_config32(sa_dev, TOLUD) & ~1;
		const u32 bgsm  = pci_read_config32(sa_dev,  BGSM) & ~1;

		current += acpi_create_dmar_rmrr(current, 0, bgsm, tolud - 1);
		current += acpi_create_dmar_ds_pci(current, 0, 2, 0);
		acpi_dmar_rmrr_fixup(tmp, current);
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

size_t size_of_dnvs(void)
{
	return sizeof(struct device_nvs);
}
