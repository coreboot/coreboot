#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include "chip.h"

/*
 * pci_domain_read_resources needs to create two resource regions,
 * one for memory and one for I/O. These are required for the 
 * resource allocation code to function correctly. The regions should
 * be large enough to hold all expected resources for all PCI
 * devices.
 */
static void pci_domain_read_resources(device_t dev)
{
	struct resource *resource;

	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->base	= 0;
	resource->limit = 0xffffUL;
	resource->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->base	= 0x80000000ULL;
	resource->limit = 0xfeffffffULL; /* We can put pci resources in the system controll area */
	resource->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

/*
 * pci_domain_set_resources creates memory resources describing the
 * fixed memory on the system. This is not actually used anywhere
 * except when the linuxbios table is generated.
 */
static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;

	/* Get the memory controller device */
	mc_dev = dev->link[0].children;
	if (mc_dev) {
		/* Figure out which areas occupied by ram. */
		int i, idx;
		uint32_t memstart1, memstart2, extmemstart1, extmemstart2;
		uint32_t memend1, memend2, extmemend1, extmemend2;
		uint8_t bank_enable;
		unsigned long start, end, size;

		/* Find the memory setup */
		memstart1    = pci_read_config32(mc_dev, 0x80);
		memstart2    = pci_read_config32(mc_dev, 0x84);
		extmemstart1 = pci_read_config32(mc_dev, 0x88);
		extmemstart2 = pci_read_config32(mc_dev, 0x8c);
		memend1      = pci_read_config32(mc_dev, 0x90);
		memend2      = pci_read_config32(mc_dev, 0x94);
		extmemend1   = pci_read_config32(mc_dev, 0x98);
		extmemend2   = pci_read_config32(mc_dev, 0x9c);
		bank_enable  = pci_read_config32(mc_dev, 0xa0);
	
		/* Report the memory regions */
		idx = 10; /* Why does idx start at 10? */

		for(i = 0; i < 8; i++) {
			struct resource *res;
			/* Ignore banks that are not enabled */
			if (!(bank_enable & (1 << i))) {
				continue;
			}
			/* Find the start and end of each bank */
			if (i < 4) {
				int shift = (8*i);
				start =  ((memstart1    >> shift) & 0xff) << 20;
				start |= ((extmemstart1 >> shift) & 0xff) << 28;
				end   =  ((memend1      >> shift) & 0xff) << 20;
				end   |= ((extmemend1   >> shift) & 0xff) << 28;
			} else {
				int shift = (8*(i - 4));
				start =  ((memstart2    >> shift) & 0xff) << 20;
				start |= ((extmemstart2 >> shift) & 0xff) << 28;
				end   =  ((memend2      >> shift) & 0xff) << 20;
				end   |= ((extmemend2   >> shift) & 0xff) << 28;
			}
			/* Comput the size of the bank */
			size = (end + (1 << 20)) - start;

			/* And now report the memory region */
			res = new_resource(dev, idx++);
			res->base = start;
			res->size = size;
			res->flags = IORESOURCE_MEM | IORESOURCE_CACHEABLE | \
				IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
		}
	}
	/* And assign the resources */
	assign_resources(&dev->link[0]);
}


static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
	max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
	return max;
}

static struct device_operations pci_domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.enable_resources = enable_childrens_resources,
	.init		  = 0,
	.scan_bus	  = pci_domain_scan_bus,
	.ops_pci_bus	  = &pci_ppc_conf1,
};  

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(&dev->link[0]);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = cpu_bus_noop,
	.set_resources	  = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init		  = cpu_bus_init,
	.scan_bus	  = 0,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
	else if (dev->path.type == DEVICE_PATH_CPU_BUS) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_motorola_mpc107_ops = {
	CHIP_NAME("Motorola MPC107 Northbridge")
	.enable_dev = enable_dev,
};
