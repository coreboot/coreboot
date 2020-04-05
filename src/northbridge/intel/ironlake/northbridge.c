/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <arch/acpi.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <delay.h>
#include <cpu/intel/model_2065x/model_2065x.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "ironlake.h"
#include <cpu/intel/smm_reloc.h>

static int bridge_revision_id = -1;

int bridge_silicon_revision(void)
{
	if (bridge_revision_id < 0) {
		uint8_t stepping = cpuid_eax(1) & 0xf;
		uint8_t bridge_id =
		    pci_read_config16(pcidev_on_root(0, 0),
				      PCI_DEVICE_ID) & 0xf0;
		bridge_revision_id = bridge_id | stepping;
	}
	return bridge_revision_id;
}

/* Reserve everything between A segment and 1MB:
 *
 * 0xa0000 - 0xbffff: legacy VGA
 * 0xc0000 - 0xcffff: VGA OPROM (needed by kernel)
 * 0xe0000 - 0xfffff: SeaBIOS, if used, otherwise DMI
 */
static const int legacy_hole_base_k = 0xa0000 / 1024;

static void add_fixed_resources(struct device *dev, int index)
{
	struct resource *resource;

	/* 0xe0000000-0xf0000000 PCIe config.
	   0xfed10000-0xfed14000 MCH
	   0xfed17000-0xfed18000 HECI
	   0xfed18000-0xfed19000 DMI
	   0xfed19000-0xfed1a000 EPBAR
	   0xfed1c000-0xfed20000 RCBA
	   0xfed90000-0xfed94000 IOMMU
	   0xff800000-0xffffffff ROM. */

	resource = new_resource(dev, index++);
	resource->base = (resource_t) 0xfed00000;
	resource->size = (resource_t) 0x00100000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
	  IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	mmio_resource(dev, index++, legacy_hole_base_k,
		      (0xc0000 >> 10) - legacy_hole_base_k);
	reserved_ram_resource(dev, index++, 0xc0000 >> 10,
			      (0x100000 - 0xc0000) >> 10);

#if CONFIG(CHROMEOS_RAMOOPS)
	reserved_ram_resource(dev, index++,
			      CONFIG_CHROMEOS_RAMOOPS_RAM_START >> 10,
			      CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE >> 10);
#endif
}

static void pci_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

#if CONFIG(HAVE_ACPI_TABLES)
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
#endif

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = northbridge_acpi_name,
#endif
};

static void mc_read_resources(struct device *dev)
{
	uint32_t tseg_base;
	uint64_t TOUUD;
	uint16_t reg16;

	pci_dev_read_resources(dev);

	mmconf_resource(dev, 0x50);

	tseg_base = pci_read_config32(pcidev_on_root(0, 0), TSEG);
	TOUUD = pci_read_config16(pcidev_on_root(0, 0),
				  D0F0_TOUUD);

	printk(BIOS_DEBUG, "ram_before_4g_top: 0x%x\n", tseg_base);
	printk(BIOS_DEBUG, "TOUUD: 0x%x\n", (unsigned int)TOUUD);

	/* Report the memory regions */
	ram_resource(dev, 3, 0, 640);
	ram_resource(dev, 4, 768, ((tseg_base >> 10) - 768));

	mmio_resource(dev, 5, tseg_base >> 10, CONFIG_SMM_TSEG_SIZE >> 10);

	reg16 = pci_read_config16(pcidev_on_root(0, 0), D0F0_GGC);
	const int uma_sizes_gtt[16] =
	    { 0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4, 42, 42, 42, 42 };
	/* Igd memory */
	const int uma_sizes_igd[16] = {
		0, 0, 0, 0, 0, 32, 48, 64, 128, 256, 96, 160, 224, 352, 256, 512
	};
	u32 igd_base, gtt_base;
	int uma_size_igd, uma_size_gtt;

	uma_size_igd = uma_sizes_igd[(reg16 >> 4) & 0xF];
	uma_size_gtt = uma_sizes_gtt[(reg16 >> 8) & 0xF];

	igd_base =
	    pci_read_config32(pcidev_on_root(0, 0), D0F0_IGD_BASE);
	gtt_base =
	    pci_read_config32(pcidev_on_root(0, 0), D0F0_GTT_BASE);
	mmio_resource(dev, 6, gtt_base >> 10, uma_size_gtt << 10);
	mmio_resource(dev, 7, igd_base >> 10, uma_size_igd << 10);

	if (TOUUD > 4096)
		ram_resource(dev, 8, (4096 << 10), ((TOUUD - 4096) << 10));

	/* This memory is not DMA-capable. */
	if (TOUUD >= 8192 - 64)
	    bad_ram_resource(dev, 9, 0x1fc000000ULL >> 10, 0x004000000 >> 10);

	add_fixed_resources(dev, 10);
}

static void mc_set_resources(struct device *dev)
{
	/* And call the normal set_resources */
	pci_dev_set_resources(dev);
}

static void northbridge_dmi_init(struct device *dev)
{
	u32 reg32;

	/* Clear error status bits */
	DMIBAR32(0x1c4) = 0xffffffff;
	DMIBAR32(0x1d0) = 0xffffffff;

	/* Steps prior to DMI ASPM */
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		reg32 = DMIBAR32(0x250);
		reg32 &= ~((1 << 22) | (1 << 20));
		reg32 |= (1 << 21);
		DMIBAR32(0x250) = reg32;
	}

	reg32 = DMIBAR32(0x238);
	reg32 |= (1 << 29);
	DMIBAR32(0x238) = reg32;

	if (bridge_silicon_revision() >= SNB_STEP_D0) {
		reg32 = DMIBAR32(0x1f8);
		reg32 |= (1 << 16);
		DMIBAR32(0x1f8) = reg32;
	} else if (bridge_silicon_revision() >= SNB_STEP_D1) {
		reg32 = DMIBAR32(0x1f8);
		reg32 &= ~(1 << 26);
		reg32 |= (1 << 16);
		DMIBAR32(0x1f8) = reg32;

		reg32 = DMIBAR32(0x1fc);
		reg32 |= (1 << 12) | (1 << 23);
		DMIBAR32(0x1fc) = reg32;
	}

	/* Enable ASPM on SNB link, should happen before PCH link */
	if ((bridge_silicon_revision() & BASE_REV_MASK) == BASE_REV_SNB) {
		reg32 = DMIBAR32(0xd04);
		reg32 |= (1 << 4);
		DMIBAR32(0xd04) = reg32;
	}

	reg32 = DMIBAR32(0x88);
	reg32 |= (1 << 1) | (1 << 0);
	DMIBAR32(0x88) = reg32;
}

static void northbridge_init(struct device *dev)
{
	northbridge_dmi_init(dev);
}

/* Disable unused PEG devices based on devicetree before PCI enumeration */
static void ironlake_init(void *const chip_info)
{
	u32 deven_mask = UINT32_MAX;
	const struct device *dev;

	dev = pcidev_on_root(1, 0);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG10.\n");
		deven_mask &= ~DEVEN_PEG10;
	}
	dev = pcidev_on_root(2, 0);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling IGD.\n");
		deven_mask &= ~DEVEN_IGD;
	}
	const struct device *const d0f0 = pcidev_on_root(0, 0);
	if (d0f0)
		pci_update_config32(d0f0, D0F0_DEVEN, deven_mask, 0);

}

static struct pci_operations intel_pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources = mc_read_resources,
	.set_resources = mc_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = northbridge_init,
	.acpi_fill_ssdt = generate_cpu_entries,
	.ops_pci = &intel_pci_ops,
};

static const struct pci_driver mc_driver_ard __pci_driver = {
	.ops = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x0044,	/* Arrandale DRAM controller */
};

static struct device_operations cpu_bus_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.init = mp_cpu_bus_init,
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

struct chip_operations northbridge_intel_ironlake_ops = {
	CHIP_NAME("Intel i7 (Arrandale) integrated Northbridge")
	.enable_dev = enable_dev,
	.init = ironlake_init,
};
