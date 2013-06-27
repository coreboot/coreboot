#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/hypertransport.h>
#include <stdlib.h>
#include <string.h>
#include <cbmem.h>
#include <cpu/cpu.h>
#include "chip.h"
#include "northbridge.h"
#include "e7520.h"

static unsigned int max_bus;

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	uint32_t pci_tolm;

        pci_tolm = find_pci_tolm(dev->link_list);

	printk(BIOS_DEBUG, "PCI mem marker = %x\n", pci_tolm);

	/* FIXME Me temporary hack */
	if(pci_tolm > 0xe0000000)
		pci_tolm = 0xe0000000;
	/* Ensure pci_tolm is 128M aligned */
	pci_tolm &= 0xf8000000;
	mc_dev = dev->link_list->children;
	if (mc_dev) {
		/* Figure out which areas are/should be occupied by RAM.
		 * This is all computed in kilobytes and converted to/from
		 * the memory controller right at the edges.
		 * Having different variables in different units is
		 * too confusing to get right.  Kilobytes are good up to
		 * 4 Terabytes of RAM...
		 */
		uint16_t tolm_r, remapbase_r, remaplimit_r, remapoffset_r;
		unsigned long tomk, tolmk;
		unsigned long remapbasek, remaplimitk, remapoffsetk;

		/* Get the Top of Memory address, units are 128M */
		tomk = ((unsigned long)pci_read_config16(mc_dev, TOM)) << 17;
		/* Compute the Top of Low Memory */
		tolmk = (pci_tolm  & 0xf8000000) >> 10;

		if (tolmk >= tomk) {
			/* The PCI hole does not overlap memory
			 * we won't use the remap window.
			 */
			tolmk = tomk;
			remapbasek   = 0x3ff << 16;
			remaplimitk  = 0 << 16;
			remapoffsetk = 0 << 16;
		}
		else {
			/* The PCI memory hole overlaps memory
			 * setup the remap window.
			 */
			/* Find the bottom of the remap window
			 * is it above 4G?
			 */
			remapbasek = 4*1024*1024;
			if (tomk > remapbasek) {
				remapbasek = tomk;
			}
			/* Find the limit of the remap window */
			remaplimitk  = (remapbasek + (4*1024*1024 - tolmk) - (1 << 16));
			/* Find the offset of the remap window from tolm */
			remapoffsetk = remapbasek - tolmk;
		}
		/* Write the ram configruation registers,
		 * preserving the reserved bits.
		 */
		tolm_r = pci_read_config16(mc_dev, 0xc4);
		tolm_r = ((tolmk >> 17) << 11) | (tolm_r & 0x7ff);
		pci_write_config16(mc_dev, 0xc4, tolm_r);

		remapbase_r = pci_read_config16(mc_dev, 0xc6);
		remapbase_r = (remapbasek >> 16) | (remapbase_r & 0xfc00);
		pci_write_config16(mc_dev, 0xc6, remapbase_r);

		remaplimit_r = pci_read_config16(mc_dev, 0xc8);
		remaplimit_r = (remaplimitk >> 16) | (remaplimit_r & 0xfc00);
		pci_write_config16(mc_dev, 0xc8, remaplimit_r);

		remapoffset_r = pci_read_config16(mc_dev, 0xca);
		remapoffset_r = (remapoffsetk >> 16) | (remapoffset_r & 0xfc00);
		pci_write_config16(mc_dev, 0xca, remapoffset_r);

		/* Report the memory regions */
    		ram_resource(dev, 3,   0, 640);
		ram_resource(dev, 4, 768, (tolmk - 768));
		if (tomk > 4*1024*1024) {
			ram_resource(dev, 5, 4096*1024, tomk - 4*1024*1024);
		}
		if (remaplimitk >= remapbasek) {
			ram_resource(dev, 6, remapbasek,
				(remaplimitk + 64*1024) - remapbasek);
		}

		set_top_of_ram(tolmk * 1024);
	}
	assign_resources(dev->link_list);
}

static u32 e7520_domain_scan_bus(device_t dev, u32 max)
{
	max_bus = pci_domain_scan_bus(dev, max);
	return max_bus;
}

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = e7520_domain_scan_bus,
	.ops_pci_bus      = pci_bus_default_ops,
};

static void mc_read_resources(device_t dev)
{
	struct resource *resource;

	pci_dev_read_resources(dev);

	resource = new_resource(dev, 0xcf);
	resource->base = 0xe0000000;
	resource->size = max_bus * 4096*256;
	resource->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |  IORESOURCE_ASSIGNED;
}

static void mc_set_resources(device_t dev)
{
	struct resource *resource;

	resource = find_resource(dev, 0xcf);
	if (resource) {
		report_resource_stored(dev, resource, "<mmconfig>");
	}
	pci_dev_set_resources(dev);
}

static void intel_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem = intel_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = mc_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = 0,
	.scan_bus         = 0,
	.ops_pci          = &intel_pci_ops,
};

static const struct pci_driver mc_driver __pci_driver = {
	.ops = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x3590,
};

static void cpu_bus_init(device_t dev)
{
        initialize_cpus(dev->link_list);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
        .read_resources   = cpu_bus_noop,
        .set_resources    = cpu_bus_noop,
        .enable_resources = cpu_bus_noop,
        .init             = cpu_bus_init,
        .scan_bus         = 0,
};


static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_e7520_ops = {
	CHIP_NAME("Intel E7520 Northbridge")
	.enable_dev = enable_dev,
};
