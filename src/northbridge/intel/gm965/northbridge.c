/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <boot/tables.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <static.h>
#include <stdint.h>

#include "chip.h"
#include "gm965.h"

static uint64_t get_touud(void)
{
	uint64_t touud = pci_read_config16(__pci_0_00_0, D0F0_TOUUD);
	touud <<= 20;
	return touud;
}

static void mch_domain_read_resources(struct device *dev)
{
	u64 tom, touud;
	u32 tolud;
	int idx = 3;

	/* Total Memory 2GB example (32MB UMA, 2MB TSEG):
	 *
	 *  00000000  0000MB-2012MB  2012MB  RAM     (writeback)
	 *  7dc00000  2012MB-2014MB     2MB  TSEG    (cbmem_top aligned down to 4MB)
	 *  7de00000  2014MB-2016MB          (alignment gap)
	 *  7e000000  2016MB-2048MB    32MB  GFX UMA (uncached, BSM=0x7e000000)
	 *  80000000   2048MB TOLUD
	 *  80000000   2048MB TOM
	 *
	 * Total Memory 4GB example (32MB UMA, 2MB TSEG):
	 *
	 *  00000000  0000MB-3038MB  3038MB  RAM     (writeback)
	 *  bdc00000  3036MB-3038MB     2MB  TSEG
	 *  be000000  3040MB-3072MB    32MB  GFX UMA (uncached, BSM=0xbe000000)
	 *  be000000   3072MB TOLUD
	 * 100000000   4096MB TOM
	 * 100000000  4096MB-5120MB  1024MB  RAM     (writeback)
	 * 140000000   5120MB TOUUD
	 *
	 * Note: unlike GM45, gm965 has no separately configurable GTT stolen
	 * region (GGMS).  BSM directly encodes the base of UMA stolen memory.
	 */

	pci_domain_read_resources(dev);

	struct device *mch = __pci_0_00_0;

	/* Top of Upper Usable DRAM, including remap */
	touud = get_touud();

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config16(mch, D0F0_TOLUD) & 0xfff0;
	tolud <<= 16;

	/* Top of Memory - does not account for any UMA */
	tom = pci_read_config16(mch, D0F0_TOM) & 0x1ff;
	tom <<= 27;

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx\n",
	       touud, tolud, tom);

	/* Report lowest memory region */
	ram_range(dev, idx++, 0, 0xa0000);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: Legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_from_to(dev, idx++, 0xa0000, 0xc0000);
	reserved_ram_from_to(dev, idx++, 0xc0000, 1*MiB);

	/* Report < 4GB memory */
	ram_range(dev, idx++, 1*MiB, cbmem_top());

	/* TSEG */
	uintptr_t tseg_base;
	size_t tseg_size;
	smm_region(&tseg_base, &tseg_size);
	mmio_range(dev, idx++, tseg_base, tseg_size);

	/* cbmem_top can be shifted downwards due to alignment.
	   Mark the region between cbmem_top and tseg_base as unusable */
	if (cbmem_top() < tseg_base) {
		printk(BIOS_DEBUG, "Unused RAM between cbmem_top and TOM: 0x%lx\n",
		       tseg_base - cbmem_top());
		mmio_from_to(dev, idx++, cbmem_top(), tseg_base);
	}

	/* graphic memory above TSEG */
	if (tseg_base + tseg_size < tolud)
		mmio_from_to(dev, idx++, tseg_base + tseg_size, tolud);

	/*
	 * If >= 4GB installed then memory from TOLUD to 4GB
	 * is remapped above TOM, TOUUD will account for both
	 */
	upper_ram_end(dev, idx++, touud);

	mmconf_resource(dev, idx++);
}

static void mch_domain_set_resources(struct device *dev)
{
	struct resource *resource;
	int i;

	for (i = 3; i <= 9; ++i) {
		/* Report read resources. */
		resource = probe_resource(dev, i);
		if (resource)
			report_resource_stored(dev, resource, "");
	}

	assign_resources(dev->downstream);
}

static void mch_domain_init(struct device *dev)
{
	struct device *mch = __pci_0_00_0;

	/* Enable SERR */
	pci_or_config16(mch, PCI_COMMAND, PCI_COMMAND_SERR);
}

static const char *northbridge_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (!is_pci_dev_on_bus(dev, 0))
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0, 0):
		return "MCHC";
	}

	return NULL;
}

void northbridge_write_smram(u8 smram)
{
	struct device *dev = __pci_0_00_0;

	pci_write_config8(dev, D0F0_SMRAM, smram);
}

static void set_above_4g_pci(const struct device *dev)
{
	const uint64_t touud = get_touud();
	const uint64_t len = POWER_OF_2(cpu_phys_address_size()) - touud;

	const char *scope = acpi_device_path(dev);
	acpigen_write_scope(scope);
	acpigen_write_name_qword("A4GB", touud);
	acpigen_write_name_qword("A4GS", len);
	acpigen_pop_len();

	printk(BIOS_DEBUG, "PCI space above 4GB MMIO is at 0x%llx, len = 0x%llx\n", touud, len);
}

static void pci_domain_ssdt(const struct device *dev)
{
	generate_cpu_entries(dev);
	set_above_4g_pci(dev);
}

struct device_operations gm965_pci_domain_ops = {
	.read_resources   = mch_domain_read_resources,
	.set_resources    = mch_domain_set_resources,
	.init             = mch_domain_init,
	.scan_bus         = pci_host_bridge_scan_bus,
	.acpi_fill_ssdt   = pci_domain_ssdt,
	.acpi_name        = northbridge_acpi_name,
};

struct device_operations gm965_cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = mp_cpu_bus_init,
};

static void gm965_init(void *const chip_info)
{
	int dev, fn, bit_base;

	struct device *const d0f0 = __pci_0_00_0;

	/* Always hide ME devices */
	pci_and_config32(d0f0, D0F0_DEVEN, ~(DEVEN_D3F0 | DEVEN_D3F1 | DEVEN_D3F2 | DEVEN_D3F3));

	/* Hide internal functions based on devicetree info. */
	for (dev = 2; dev > 0; --dev) {
		switch (dev) {
		case 2: /* IGD */
			fn = 1;
			bit_base = 3;
			break;
		case 1: /* PEG */
			fn = 0;
			bit_base = 1;
			break;
		}
		for (; fn >= 0; --fn) {
			const struct device *const d = pcidev_on_root(dev, fn);
			if (!d || d->enabled)
				continue;
			pci_and_config32(d0f0, D0F0_DEVEN, ~(1 << (bit_base + fn)));
		}
	}

	/* Initialize PEG port if enabled */
	struct device *peg = pcidev_on_root(1, 0);
	if (peg && peg->enabled)
		gm965_pcie_init(peg);
}

struct chip_operations northbridge_intel_gm965_ops = {
	.name = "Intel GM965 Northbridge",
	.init = gm965_init,
};

bool northbridge_support_slfm(void)
{
	return false;
}
