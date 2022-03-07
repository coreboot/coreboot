/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <acpi/acpi.h>
#include <arch/hpet.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <cpu/intel/model_2065x/model_2065x.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "chip.h"
#include <commonlib/bsd/helpers.h>
#include "ironlake.h"
#include <cpu/intel/smm_reloc.h>

static int bridge_revision_id = -1;

int bridge_silicon_revision(void)
{
	if (bridge_revision_id < 0) {
		uint8_t stepping = cpuid_eax(1) & 0x0f;
		uint8_t bridge_id = pci_read_config16(pcidev_on_root(0, 0), PCI_DEVICE_ID);
		bridge_revision_id = (bridge_id & 0xf0) | stepping;
	}
	return bridge_revision_id;
}

/*
 * Reserve everything between A segment and 1MB:
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
	resource->base = (resource_t) HPET_BASE_ADDRESS;
	resource->size = (resource_t) 0x00100000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE | IORESOURCE_FIXED |
			  IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	mmio_resource(dev, index++, legacy_hole_base_k, (0xc0000 / KiB) - legacy_hole_base_k);
	reserved_ram_resource(dev, index++, 0xc0000 / KiB, (0x100000 - 0xc0000) / KiB);
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
	.read_resources	= pci_domain_read_resources,
	.set_resources	= pci_domain_set_resources,
	.scan_bus	= pci_domain_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name	= northbridge_acpi_name,
#endif
};

static void mc_read_resources(struct device *dev)
{
	uint32_t tseg_base, tseg_end;
	uint64_t touud;
	uint16_t reg16;
	int index = 3;

	pci_dev_read_resources(dev);

	mmconf_resource(dev, 0x50);

	tseg_base = pci_read_config32(pcidev_on_root(0, 0), TSEG);
	tseg_end = tseg_base + CONFIG_SMM_TSEG_SIZE;
	touud = pci_read_config16(pcidev_on_root(0, 0),
				  TOUUD);

	printk(BIOS_DEBUG, "ram_before_4g_top: 0x%x\n", tseg_base);
	printk(BIOS_DEBUG, "TOUUD: 0x%x\n", (unsigned int)touud);

	/* Report the memory regions */
	ram_resource(dev, index++, 0, 0xa0000 / KiB);
	ram_resource(dev, index++, 1 * MiB / KiB, (tseg_base - 1 * MiB) / KiB);

	mmio_resource(dev, index++, tseg_base / KiB, CONFIG_SMM_TSEG_SIZE / KiB);

	reg16 = pci_read_config16(pcidev_on_root(0, 0), GGC);
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
	    pci_read_config32(pcidev_on_root(0, 0), IGD_BASE);
	gtt_base =
	    pci_read_config32(pcidev_on_root(0, 0), GTT_BASE);
	if (gtt_base > tseg_end) {
		/* Reserve the gap. MMIO doesn't work in this range. Keep
		   it uncacheable, though, for easier MTRR allocation. */
		mmio_resource(dev, index++, tseg_end / KiB, (gtt_base - tseg_end) / KiB);
	}
	mmio_resource(dev, index++, gtt_base / KiB, uma_size_gtt * KiB);
	mmio_resource(dev, index++, igd_base / KiB, uma_size_igd * KiB);

	if (touud > 4096)
		ram_resource(dev, index++, (4096 * KiB), ((touud - 4096) * KiB));

	/* This memory is not DMA-capable. */
	if (touud >= 8192 - 64)
		bad_ram_resource(dev, index++, 0x1fc000000ULL / KiB, 0x004000000 / KiB);

	add_fixed_resources(dev, index);
}

static void northbridge_init(struct device *dev)
{
	/* Clear error status bits */
	dmibar_write32(DMIUESTS, 0xffffffff);
	dmibar_write32(DMICESTS, 0xffffffff);

	dmibar_setbits32(DMILLTC, 1 << 29);

	dmibar_setbits32(0x1f8, 1 << 16);

	dmibar_setbits32(DMILCTL, 1 << 1 | 1 << 0);
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
		pci_update_config32(d0f0, DEVEN, deven_mask, 0);

}

static struct device_operations mc_ops = {
	.read_resources		= mc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= northbridge_init,
	.acpi_fill_ssdt		= generate_cpu_entries,
	.ops_pci		= &pci_dev_ops_pci,
};

/*
 * The host bridge PCI device ID can be changed by the firmware. There
 * is no documentation about it, though. There's 'official' IDs, which
 * appear in spec updates and Windows drivers, and 'mysterious' IDs,
 * which Intel doesn't want OSes to know about and thus are not listed.
 *
 * The current coreboot code seems to be able to change the device ID
 * of the host bridge, but it seems to be missing a warm reset so that
 * the device ID changes. Account for the 'mysterious' device IDs in
 * the northbridge driver, so that booting an OS has a chance to work.
 */
static const unsigned short pci_device_ids[] = {
	/* 'Official' DIDs */
	0x0040, /* Clarkdale */
	0x0044, /* Arrandale */
	0x0048, /* Unknown, but it appears in OS drivers and raminit */

	/* Mysterious DIDs, taken from Linux' intel-agp driver */
	0x0062, /* Arrandale A-? */
	0x0069, /* Clarkdale K-0 */
	0x006a, /* Arrandale K-0 */
	0
};

static const struct pci_driver mc_driver_ilk __pci_driver = {
	.ops     = &mc_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = pci_device_ids,
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
	CHIP_NAME("Intel Ironlake integrated Northbridge")
	.enable_dev = enable_dev,
	.init = ironlake_init,
};
