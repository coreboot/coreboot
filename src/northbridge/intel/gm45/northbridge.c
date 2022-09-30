/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <boot/tables.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/smm_reloc.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <stdint.h>

#include "chip.h"
#include "gm45.h"

static uint64_t get_touud(void)
{
	uint64_t touud = pci_read_config16(__pci_0_00_0, D0F0_TOUUD);
	touud <<= 20;
	return touud;
}

static void mch_domain_read_resources(struct device *dev)
{
	u64 tom, touud;
	u32 tomk, tolud, uma_sizek = 0, delta_cbmem;
	int idx = 3;

	/* Total Memory 2GB example:
	 *
	 *  00000000  0000MB-2014MB  2014MB  RAM     (writeback)
	 *  7de00000  2014MB-2016MB     2MB  GFX GTT (uncached)
	 *  7e000000  2016MB-2048MB    32MB  GFX UMA (uncached)
	 *  80000000   2048MB TOLUD
	 *  80000000   2048MB TOM
	 *
	 * Total Memory 4GB example:
	 *
	 *  00000000  0000MB-3038MB  3038MB  RAM     (writeback)
	 *  bde00000  3038MB-3040MB     2MB  GFX GTT (uncached)
	 *  be000000  3040MB-3072MB    32MB  GFX UMA (uncached)
	 *  be000000   3072MB TOLUD
	 * 100000000   4096MB TOM
	 * 100000000  4096MB-5120MB  1024MB  RAM     (writeback)
	 * 140000000   5120MB TOUUD
	 */

	pci_domain_read_resources(dev);

	struct device *mch = pcidev_on_root(0, 0);

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

	tomk = tolud >> 10;

	/* Graphics memory comes next */
	const u16 ggc = pci_read_config16(mch, D0F0_GGC);
	if (!(ggc & 2)) {
		printk(BIOS_DEBUG, "IGD decoded, subtracting ");

		/* Graphics memory */
		const u32 gms_sizek = decode_igd_memory_size((ggc >> 4) & 0xf);
		printk(BIOS_DEBUG, "%uM UMA, ", gms_sizek >> 10);
		tomk -= gms_sizek;

		/* GTT Graphics Stolen Memory Size (GGMS) */
		const u32 gsm_sizek = decode_igd_gtt_size((ggc >> 8) & 0xf);
		printk(BIOS_DEBUG, "%uM GTT", gsm_sizek >> 10);
		tomk -= gsm_sizek;

		uma_sizek = gms_sizek + gsm_sizek;
	}
	const u8 esmramc = pci_read_config8(mch, D0F0_ESMRAMC);
	const u32 tseg_sizek = decode_tseg_size(esmramc);
	printk(BIOS_DEBUG, " and %uM TSEG\n", tseg_sizek >> 10);
	tomk -= tseg_sizek;
	uma_sizek += tseg_sizek;

	/* cbmem_top can be shifted downwards due to alignment.
	   Mark the region between cbmem_top and tomk as unusable */
	delta_cbmem = tomk - ((uintptr_t)cbmem_top() >> 10);
	tomk -= delta_cbmem;
	uma_sizek += delta_cbmem;

	printk(BIOS_DEBUG, "Unused RAM between cbmem_top and TOM: 0x%xK\n",
	       delta_cbmem);

	printk(BIOS_INFO, "Available memory below 4GB: %uM\n", tomk >> 10);

	/* Report lowest memory region */
	ram_resource_kb(dev, idx++, 0, 0xa0000 / KiB);

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: Legacy VGA
	 * 0xc0000 - 0xfffff: RAM
	 */
	mmio_resource_kb(dev, idx++, 0xa0000 / KiB, (0xc0000 - 0xa0000) / KiB);
	reserved_ram_resource_kb(dev, idx++, 0xc0000 / KiB, (1*MiB - 0xc0000) / KiB);

	/* Report < 4GB memory */
	ram_resource_kb(dev, idx++, 1*MiB / KiB, tomk - 1*MiB / KiB);

	/*
	 * If >= 4GB installed then memory from TOLUD to 4GB
	 * is remapped above TOM, TOUUD will account for both
	 */
	upper_ram_end(dev, idx++, touud);

	printk(BIOS_DEBUG, "Adding UMA memory area base=0x%llx "
	       "size=0x%llx\n", ((u64)tomk) << 10, ((u64)uma_sizek) << 10);
	/* Don't use uma_resource_kb() as our UMA touches the PCI hole. */
	fixed_mem_resource_kb(dev, idx++, tomk, uma_sizek, IORESOURCE_RESERVE);

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

	assign_resources(dev->link_list);
}

static void mch_domain_init(struct device *dev)
{
	struct device *mch = pcidev_on_root(0, 0);

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
	struct device *dev = pcidev_on_root(0, 0);

	if (!dev)
		die("could not find pci 00:00.0!\n");

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

static struct device_operations pci_domain_ops = {
	.read_resources   = mch_domain_read_resources,
	.set_resources    = mch_domain_set_resources,
	.init             = mch_domain_init,
	.scan_bus         = pci_domain_scan_bus,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt   = pci_domain_ssdt,
	.acpi_name        = northbridge_acpi_name,
};

static struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = mp_cpu_bus_init,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

static void gm45_init(void *const chip_info)
{
	int dev, fn, bit_base;

	struct device *const d0f0 = pcidev_on_root(0x0, 0);

	/* Hide internal functions based on devicetree info. */
	for (dev = 3; dev > 0; --dev) {
		switch (dev) {
		case 3: /* ME */
			fn = 3;
			bit_base = 6;
			break;
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
			/* FIXME: Using bitwise ops changes the binary */
			pci_write_config32(d0f0, D0F0_DEVEN,
				pci_read_config32(d0f0, D0F0_DEVEN) & ~(1 << (bit_base + fn)));
		}
	}

	const u32 deven = pci_read_config32(d0f0, D0F0_DEVEN);
	if (!(deven & (0xf << 6)))
		pci_write_config32(d0f0, D0F0_DEVEN, deven & ~(1 << 14));
}

struct chip_operations northbridge_intel_gm45_ops = {
	CHIP_NAME("Intel GM45 Northbridge")
	.enable_dev = enable_dev,
	.init = gm45_init,
};
