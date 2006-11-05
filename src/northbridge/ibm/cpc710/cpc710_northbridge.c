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

static void ram_resource(device_t dev, unsigned long index,
	unsigned long basek, unsigned long sizek)
{
	struct resource *resource;

	if (!sizek) {
		return;
	}
	resource = new_resource(dev, index);
	resource->base	= ((resource_t)basek) << 10;
	resource->size	= ((resource_t)sizek) << 10;
	resource->flags =  IORESOURCE_MEM | IORESOURCE_CACHEABLE | \
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void pci_domain_set_resources(device_t dev)
{
	int idx;

	/* Report the memory regions */
	idx = 10;
	ram_resource(dev, idx++, 0, 1024*1024); /* FIXME */

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

struct chip_operations northbridge_ibm_cpc710_ops = {
	CHIP_NAME("IBM CPC710 Northbridge")
	.enable_dev = enable_dev,
};
