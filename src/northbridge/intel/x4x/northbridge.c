/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <console/console.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <device/device.h>
#include <boot/tables.h>
#include <acpi/acpi.h>
#include <northbridge/intel/x4x/memmap.h>
#include <northbridge/intel/x4x/chip.h>
#include <northbridge/intel/x4x/x4x.h>
#include <cpu/intel/smm_reloc.h>

static void mch_domain_read_resources(struct device *dev)
{
	u8 index;
	u64 tom, touud;
	u32 tomk, tolud, delta_cbmem;
	u32 uma_sizek = 0;

	const u32 top32memk = 4 * (GiB / KiB);
	index = 3;

	pci_domain_read_resources(dev);

	struct device *mch = pcidev_on_root(0, 0);

	/* Top of Upper Usable DRAM, including remap */
	touud = pci_read_config16(mch, D0F0_TOUUD);
	touud <<= 20;

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config16(mch, D0F0_TOLUD) & 0xfff0;
	tolud <<= 16;

	/* Top of Memory - does not account for any UMA */
	tom = pci_read_config16(mch, D0F0_TOM) & 0x01ff;
	tom <<= 26;

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx\n", touud, tolud, tom);

	tomk = tolud >> 10;

	/* Graphics memory comes next */

	const u16 ggc = pci_read_config16(mch, D0F0_GGC);
	printk(BIOS_DEBUG, "IGD decoded, subtracting ");

	/* Graphics memory */
	const u32 gms_sizek = decode_igd_memory_size((ggc >> 4) & 0xf);
	printk(BIOS_DEBUG, "%uM UMA", gms_sizek >> 10);
	tomk -= gms_sizek;
	uma_sizek += gms_sizek;

	/* GTT Graphics Stolen Memory Size (GGMS) */
	const u32 gsm_sizek = decode_igd_gtt_size((ggc >> 8) & 0xf);
	printk(BIOS_DEBUG, " and %uM GTT\n", gsm_sizek >> 10);
	tomk -= gsm_sizek;
	uma_sizek += gsm_sizek;

	printk(BIOS_DEBUG, "TSEG decoded, subtracting ");
	const u32 tseg_sizek = decode_tseg_size(
		pci_read_config8(dev, D0F0_ESMRAMC)) >> 10;
	uma_sizek += tseg_sizek;
	tomk -= tseg_sizek;

	printk(BIOS_DEBUG, "%dM\n", tseg_sizek >> 10);

	/* cbmem_top can be shifted downwards due to alignment.
	   Mark the region between cbmem_top and tomk as unusable */
	delta_cbmem = tomk - ((uintptr_t)cbmem_top() >> 10);
	tomk -= delta_cbmem;
	uma_sizek += delta_cbmem;

	printk(BIOS_DEBUG, "Unused RAM between cbmem_top and TOM: 0x%xK\n", delta_cbmem);

	printk(BIOS_INFO, "Available memory below 4GB: %uM\n", tomk >> 10);

	/* Report the memory regions */
	ram_from_to(dev, index++, 0, 0xa0000);
	mmio_from_to(dev, index++, 0xa0000, 0xc0000);
	reserved_ram_from_to(dev, index++, 0xc0000, 1 * MiB);
	ram_resource_kb(dev, index++, 0x100000 >> 10, (tomk - (0x100000 >> 10)));

	/*
	 * If >= 4GB installed then memory from TOLUD to 4GB
	 * is remapped above TOM, TOUUD will account for both
	 */
	upper_ram_end(dev, index++, touud);

	printk(BIOS_DEBUG, "Adding UMA memory area base=0x%08x size=0x%08x\n",
	       tomk << 10, uma_sizek << 10);
	uma_resource_kb(dev, index++, tomk, uma_sizek);

	/* Reserve high memory where the NB BARs are up to 4GiB */
	fixed_mem_resource_kb(dev, index++, DEFAULT_HECIBAR >> 10,
				top32memk - (DEFAULT_HECIBAR >> 10),
				IORESOURCE_RESERVE);

	mmconf_resource(dev, index++);
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

static struct device_operations pci_domain_ops = {
	.read_resources   = mch_domain_read_resources,
	.set_resources    = mch_domain_set_resources,
	.init             = mch_domain_init,
	.scan_bus         = pci_domain_scan_bus,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt   = generate_cpu_entries,
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
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		dev->ops = &pci_domain_ops;
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &cpu_bus_ops;
}

static void hide_pci_fn(const int dev_bit_base, const struct device *dev)
{
	if (!dev || dev->enabled)
		return;
	const unsigned int fn = PCI_FUNC(dev->path.pci.devfn);
	const struct device *const d0f0 = pcidev_on_root(0, 0);
	pci_update_config32(d0f0, D0F0_DEVEN, ~(1 << (dev_bit_base + fn)), 0);
}

static void hide_pci_dev(const int dev, int functions, const int dev_bit_base)
{
	for (; functions >= 0; functions--)
		hide_pci_fn(dev_bit_base, pcidev_on_root(dev, functions));
}

static void x4x_init(void *const chip_info)
{
	struct device *const d0f0 = pcidev_on_root(0x0, 0);

	/* Hide internal functions based on devicetree info. */
	hide_pci_dev(6, 0, 13); /* PEG1: only on P45 */
	hide_pci_dev(3, 3, 6); /* ME */
	hide_pci_dev(2, 1, 3); /* IGD */
	hide_pci_dev(1, 0, 1); /* PEG0 */

	const u32 deven = pci_read_config32(d0f0, D0F0_DEVEN);
	if (!(deven & (0xf << 6)))
		pci_write_config32(d0f0, D0F0_DEVEN, deven & ~(1 << 14));
}

struct chip_operations northbridge_intel_x4x_ops = {
	CHIP_NAME("Intel 4-Series Northbridge")
	.enable_dev = enable_dev,
	.init = x4x_init,
};
