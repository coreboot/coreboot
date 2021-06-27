/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <cbmem.h>
#include <console/console.h>
#include <commonlib/bsd/helpers.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <device/device.h>
#include <boot/tables.h>
#include <acpi/acpi.h>
#include <northbridge/intel/pineview/pineview.h>
#include <cpu/intel/smm_reloc.h>

/*
 * Reserve everything between A segment and 1MB:
 *
 * 0xa0000 - 0xbffff: legacy VGA
 * 0xc0000 - 0xcffff: VGA OPROM (needed by kernel)
 * 0xe0000 - 0xfffff: SeaBIOS, if used, otherwise DMI
 */

static void add_fixed_resources(struct device *dev, int index)
{
	struct resource *resource;

	resource = new_resource(dev, index++);
	resource->base = (resource_t) HPET_BASE_ADDRESS;
	resource->size = (resource_t) 0x00100000;
	resource->flags = IORESOURCE_MEM
			| IORESOURCE_RESERVE
			| IORESOURCE_FIXED
			| IORESOURCE_STORED
			| IORESOURCE_ASSIGNED;

	mmio_from_to(dev, index++, 0xa0000, 0xc0000);
	reserved_ram_from_to(dev, index++, 0xc0000, 1 * MiB);
}

static void mch_domain_read_resources(struct device *dev)
{
	u64 tom, touud;
	u32 tomk, tolud, tseg_sizek;
	u32 cbmem_topk, delta_cbmem;
	u16 index;
	const u32 top32memk = 4 * (GiB / KiB);

	struct device *mch = pcidev_on_root(0, 0);

	index = 3;

	pci_domain_read_resources(dev);

	/* Top of Upper Usable DRAM, including remap */
	touud = pci_read_config16(mch, TOUUD);
	touud <<= 20;

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config16(mch, TOLUD) & 0xfff0;
	tolud <<= 16;

	/* Top of Memory - does not account for any UMA */
	tom = pci_read_config16(mch, TOM) & 0x01ff;
	tom <<= 27;

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx ", touud, tolud, tom);

	tomk = tolud / KiB;

	/* Graphics memory */
	const u16 ggc = pci_read_config16(mch, GGC);
	const u32 gms_sizek = decode_igd_memory_size((ggc >> 4) & 0xf);
	printk(BIOS_DEBUG, "%uM UMA", gms_sizek / KiB);
	tomk -= gms_sizek;

	/* GTT Graphics Stolen Memory Size (GGMS) */
	const u32 gsm_sizek = decode_igd_gtt_size((ggc >> 8) & 0xf);
	printk(BIOS_DEBUG, " and %uM GTT\n", gsm_sizek / KiB);
	tomk -= gsm_sizek;

	const u32 tseg_basek = pci_read_config32(mch, TSEG) / KiB;
	const u32 igd_basek = pci_read_config32(mch, GBSM) / KiB;
	const u32 gtt_basek = pci_read_config32(mch, BGSM) / KiB;

	/* Subtract TSEG size */
	tseg_sizek = gtt_basek - tseg_basek;
	tomk -= tseg_sizek;
	printk(BIOS_DEBUG, "TSEG decoded, subtracting %dM\n", tseg_sizek / KiB);

	/* cbmem_top can be shifted downwards due to alignment.
	   Mark the region between cbmem_top and tomk as unusable */
	cbmem_topk = (uint32_t)cbmem_top() / KiB;
	delta_cbmem = tomk - cbmem_topk;
	tomk -= delta_cbmem;

	printk(BIOS_DEBUG, "Unused RAM between cbmem_top and TOMK: 0x%xK\n", delta_cbmem);

	/* Report the memory regions */
	ram_resource_kb(dev, index++, 0, 0xa0000 / KiB);
	ram_resource_kb(dev, index++, 1 * MiB / KiB, tomk - 1 * MiB / KiB);
	mmio_resource_kb(dev, index++, tseg_basek, tseg_sizek);
	mmio_resource_kb(dev, index++, gtt_basek,  gsm_sizek);
	mmio_resource_kb(dev, index++, igd_basek,  gms_sizek);
	reserved_ram_resource_kb(dev, index++, cbmem_topk, delta_cbmem);

	/*
	 * If > 4GB installed then memory from TOLUD to 4GB
	 * is remapped above TOM, TOUUD will account for both
	 */
	touud >>= 10; /* Convert to KB */
	if (touud > top32memk) {
		ram_resource_kb(dev, index++, top32memk, touud - top32memk);
		printk(BIOS_INFO, "Available memory above 4GB: %lluM\n",
			(touud - top32memk) / KiB);
	}

	mmconf_resource(dev, index++);

	add_fixed_resources(dev, index);
}

void northbridge_write_smram(u8 smram)
{
	struct device *dev = pcidev_on_root(0, 0);

	if (dev == NULL)
		die("could not find pci 00:00.0!\n");

	pci_write_config8(dev, SMRAM, smram);
}

static void mch_domain_set_resources(struct device *dev)
{
	struct resource *res;

	for (res = dev->resource_list; res; res = res->next)
		report_resource_stored(dev, res, "");

	assign_resources(dev->link_list);
}

static void mch_domain_init(struct device *dev)
{
	/* Enable SERR */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);
}

static const char *northbridge_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI || dev->bus->secondary != 0)
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0, 0):
		return "MCHC";
	}

	return NULL;
}

static struct device_operations pci_domain_ops = {
	.read_resources	= mch_domain_read_resources,
	.set_resources	= mch_domain_set_resources,
	.init		= mch_domain_init,
	.scan_bus	= pci_domain_scan_bus,
	.acpi_fill_ssdt	= generate_cpu_entries,
	.acpi_name	= northbridge_acpi_name,
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

struct chip_operations northbridge_intel_pineview_ops = {
	CHIP_NAME("Intel Pineview Northbridge")
	.enable_dev = enable_dev,
};
