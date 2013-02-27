#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cbmem.h>
#include <cpu/cpu.h>
#include <pc80/keyboard.h>
#include "northbridge.h"
#include "i440bx.h"

static void northbridge_init(device_t dev)
{
	printk(BIOS_SPEW, "Northbridge Init\n");
}

static struct device_operations northbridge_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = northbridge_init,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x7190,
};

static void i440bx_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	uint32_t pci_tolm;

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children;
	if (mc_dev) {
		unsigned long tomk, tolmk;
		int idx;

		/* Figure out which areas are/should be occupied by RAM. The
		 * value of the highest DRB denotes the end of the physical
		 * memory (in units of 8MB).
		 */
		tomk = ((unsigned long)pci_read_config8(mc_dev, DRB7));

		/* Convert to KB. */
		tomk *= (8 * 1024);

		printk(BIOS_DEBUG, "Setting RAM size to %ld MB\n", tomk / 1024);

		/* Compute the top of low memory. */
		tolmk = pci_tolm / 1024;

		if (tolmk >= tomk) {
			/* The PCI hole does not overlap the memory. */
			tolmk = tomk;
		}

		/* Report the memory regions. */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 768, tolmk - 768);

		/* Leave some space for ACPI, PIRQ and MP tables */
		high_tables_base = (tomk * 1024) - HIGH_MEMORY_SIZE;
		high_tables_size = HIGH_MEMORY_SIZE;
	}
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources		= pci_domain_read_resources,
	.set_resources		= i440bx_domain_set_resources,
	.enable_resources	= NULL,
	.init			= NULL,
	.scan_bus		= pci_domain_scan_bus,
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

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
		pci_set_method(dev);
	}
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_i440bx_ops = {
	CHIP_NAME("Intel 82443BX (440BX) Northbridge")
	.enable_dev = enable_dev,
};
